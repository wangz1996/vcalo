// This file is part of the Acts project.
//
// Copyright (C) 2022 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "AmbiguityResolutionAlgorithm.hh"

#include "Acts/EventData/MultiTrajectoryHelpers.hpp"
#include "IndexSourceLink.hh"
#include "Measurement.hh"
#include "ProcessCode.hh"
#include "WhiteBoard.hh"

#include <iterator>
#include <numeric>
#include <stdexcept>

#include <boost/container/flat_map.hpp>
#include <boost/container/flat_set.hpp>

AmbiguityResolutionAlgorithm::AmbiguityResolutionAlgorithm(
    AmbiguityResolutionAlgorithm::Config cfg,
    Acts::Logging::Level lvl)
    : IAlgorithm("AmbiguityResolutionAlgorithm", lvl),
      m_cfg(std::move(cfg)) {
}

namespace {

struct State {
  std::size_t numberOfTracks{};

  std::vector<int> trackTips;
  std::vector<float> trackChi2;
  std::vector<std::vector<std::size_t>> measurementsPerTrack;

  boost::container::flat_map<std::size_t,
                             boost::container::flat_set<std::size_t>>
      tracksPerMeasurement;
  std::vector<std::size_t> sharedMeasurementsPerTrack;

  boost::container::flat_set<std::size_t> selectedTracks;
};

State computeInitialState(const ConstTrackContainer& tracks,
                          std::size_t nMeasurementsMin) {
  State state;

  for (const auto& track : tracks) {
    auto trajState = Acts::MultiTrajectoryHelpers::trajectoryState(
        tracks.trackStateContainer(), track.tipIndex());
    if (trajState.nMeasurements < nMeasurementsMin) {
      continue;
    }
    std::vector<std::size_t> measurements;
    tracks.trackStateContainer().visitBackwards(
        track.tipIndex(), [&](const auto& hit) {
          if (hit.typeFlags().test(Acts::TrackStateFlag::MeasurementFlag)) {
            std::size_t iMeasurement =
                hit.getUncalibratedSourceLink()
                    .template get<IndexSourceLink>()
                    .index();
            measurements.push_back(iMeasurement);
          }
          return true;
        });

    state.trackTips.push_back(track.index());
    state.trackChi2.push_back(trajState.chi2Sum / trajState.NDF);
    state.measurementsPerTrack.push_back(std::move(measurements));
    state.selectedTracks.insert(state.numberOfTracks);

    ++state.numberOfTracks;
  }

  for (std::size_t iTrack = 0; iTrack < state.numberOfTracks; ++iTrack) {
    for (auto iMeasurement : state.measurementsPerTrack[iTrack]) {
      state.tracksPerMeasurement[iMeasurement].insert(iTrack);
    }
  }
  state.sharedMeasurementsPerTrack =
      std::vector<std::size_t>(state.trackTips.size(), 0);

  for (std::size_t iTrack = 0; iTrack < state.numberOfTracks; ++iTrack) {
    for (auto iMeasurement : state.measurementsPerTrack[iTrack]) {
      if (state.tracksPerMeasurement[iMeasurement].size() > 1) {
        ++state.sharedMeasurementsPerTrack[iTrack];
      }
    }
  }

  return state;
}

void removeTrack(State& state, std::size_t iTrack) {
  for (auto iMeasurement : state.measurementsPerTrack[iTrack]) {
    state.tracksPerMeasurement[iMeasurement].erase(iTrack);

    if (state.tracksPerMeasurement[iMeasurement].size() == 1) {
      auto jTrack = *std::begin(state.tracksPerMeasurement[iMeasurement]);
      --state.sharedMeasurementsPerTrack[jTrack];
    }
  }

  state.selectedTracks.erase(iTrack);
}

}  // namespace

ProcessCode AmbiguityResolutionAlgorithm::execute(
    const ConstTrackContainer& inputTracks) {
  
  const auto& tracks = std::move(inputTracks);

  ACTS_VERBOSE("number of input tracks " << tracks.size());

  auto state = computeInitialState(tracks, m_cfg.nMeasurementsMin);

  ACTS_VERBOSE("state initialized");

  auto sharedMeasurementsComperator = [&state](std::size_t a, std::size_t b) {
    return state.sharedMeasurementsPerTrack[a] <
           state.sharedMeasurementsPerTrack[b];
  };
  auto badTrackComperator = [&state](std::size_t a, std::size_t b) {
    auto relativeSharedMeasurements = [&state](std::size_t i) {
      return 1.0 * state.sharedMeasurementsPerTrack[i] /
             state.measurementsPerTrack[i].size();
    };

    if (relativeSharedMeasurements(a) != relativeSharedMeasurements(b)) {
      return relativeSharedMeasurements(a) < relativeSharedMeasurements(b);
    }
    return state.trackChi2[a] < state.trackChi2[b];
  };

  for (std::size_t i = 0; i < m_cfg.maximumIterations; ++i) {
    if (state.selectedTracks.empty()) {
      ACTS_VERBOSE("no tracks left - exit loop");
      break;
    }

    auto maximumSharedMeasurements = *std::max_element(
        state.selectedTracks.begin(), state.selectedTracks.end(),
        sharedMeasurementsComperator);
    ACTS_VERBOSE(
        "maximum shared measurements "
        << state.sharedMeasurementsPerTrack[maximumSharedMeasurements]);
    if (state.sharedMeasurementsPerTrack[maximumSharedMeasurements] <
        m_cfg.maximumSharedHits) {
      break;
    }

    auto badTrack =
        *std::max_element(state.selectedTracks.begin(),
                          state.selectedTracks.end(), badTrackComperator);
    ACTS_VERBOSE("remove track " << badTrack);
    removeTrack(state, badTrack);
  }

  ACTS_INFO("Resolved to " << state.selectedTracks.size() << " tracks from "
                           << tracks.size());

  std::shared_ptr<Acts::ConstVectorMultiTrajectory> trackStateContainer =
      tracks.trackStateContainerHolder();
  auto trackContainer = std::make_shared<Acts::VectorTrackContainer>();
  trackContainer->reserve(state.selectedTracks.size());
  // temporary empty track state container: we don't change the original one,
  // but we need one for filtering
  auto tempTrackStateContainer =
      std::make_shared<Acts::VectorMultiTrajectory>();

  TrackContainer solvedTracks{trackContainer, tempTrackStateContainer};
  solvedTracks.ensureDynamicColumns(tracks);

  for (auto iTrack : state.selectedTracks) {
    auto destProxy = solvedTracks.getTrack(solvedTracks.addTrack());
    destProxy.copyFrom(tracks.getTrack(state.trackTips.at(iTrack)));
  }

  ConstTrackContainer outputTracks{
      std::make_shared<Acts::ConstVectorTrackContainer>(
          std::move(*trackContainer)),
      trackStateContainer};

  return ProcessCode::SUCCESS;
}

ProcessCode AmbiguityResolutionAlgorithm::execute(const AlgorithmContext &context) const {
  return ProcessCode::SUCCESS;
}
