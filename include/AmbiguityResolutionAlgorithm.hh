// This file is part of the Acts project.
//
// Copyright (C) 2022 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "Track.hh"
#include "IAlgorithm.hh"

#include <string>
#include <vector>

/// Evicts tracks that seem to be duplicated.
///
/// The implementation works as follows:
///  1) Calculate shared hits per track.
///  2) If the maximum shared hits criteria is met, we are done.
///     This is the configurable amount of shared hits we are ok with
///     in our experiment.
///  3) Else, remove the track with the highest relative shared hits (i.e.
///     shared hits / hits).
///  4) Back to square 1.
class AmbiguityResolutionAlgorithm final : public IAlgorithm {
 public:
  struct Config {
    /// Input trajectories collection.
    std::string inputTracks;
    /// Output trajectories collection.
    std::string outputTracks;

    /// Maximum amount of shared hits per track.
    std::uint32_t maximumSharedHits = 2;
    /// Maximum number of iterations
    std::uint32_t maximumIterations = 1000;

    /// Minumum number of measurement to form a track.
    size_t nMeasurementsMin = 3;
  };

  /// Construct the ambiguity resolution algorithm.
  ///
  /// @param cfg is the algorithm configuration
  /// @param lvl is the logging level
  AmbiguityResolutionAlgorithm(Config cfg , Acts::Logging::Level lvl=Acts::Logging::Level::VERBOSE);

  /// Run the ambiguity resolution algorithm.
  ///
  /// @param cxt is the algorithm context with event information
  /// @return a process code indication success or failure
  ProcessCode execute(const ConstTrackContainer& inputTracks);

  ProcessCode execute(const AlgorithmContext &context) const final;

  /// Const access to the config
  const Config& config() const { return m_cfg; }

 private:
  Config m_cfg;
  // ConstTrackContainer m_inputTracks;
  // ConstTrackContainer m_outputTracks;
};
