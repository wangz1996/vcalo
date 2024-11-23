#include "TrackFindingAlgorithm.hh"
#include "Acts/EventData/MultiTrajectory.hpp"
#include "Acts/EventData/TrackContainer.hpp"
#include "Acts/EventData/VectorMultiTrajectory.hpp"
#include "Acts/EventData/VectorTrackContainer.hpp"
#include "Acts/Surfaces/PerigeeSurface.hpp"
#include "Acts/TrackFitting/GainMatrixSmoother.hpp"
#include "Acts/TrackFitting/GainMatrixUpdater.hpp"
#include "Measurement.hh"
#include "Track.hh"
#include "ProcessCode.hh"

#include <stdexcept>

#include <boost/histogram.hpp>

TrackFindingAlgorithm::TrackFindingAlgorithm(
    Config config, Acts::Logging::Level level)
    : IAlgorithm("TrackFindingAlgorithm", level),
    m_cfg(std::move(config)),
    m_outputTracks(std::make_shared<Acts::ConstVectorTrackContainer>(),std::make_shared<Acts::ConstVectorMultiTrajectory>()){
}
ProcessCode TrackFindingAlgorithm::execute(const AlgorithmContext &ctx) const
{
    const auto &measurements = m_inputMeasurements;
    const auto &sourceLinks = m_inputSourceLinks;
    const auto &initialParameters = m_inputInitialTrackParameters;

    // Construct a perigee surface as the target surface
    auto pSurface = Acts::Surface::makeShared<Acts::PerigeeSurface>(
        Acts::Vector3{0., 0., 0.});

    Acts::PropagatorPlainOptions pOptions;
    pOptions.maxSteps = 100000;

    Acts::GainMatrixUpdater kfUpdater;
    Acts::GainMatrixSmoother kfSmoother;
    Acts::MeasurementSelector measSel{m_cfg.measurementSelectorCfg};

    Acts::CombinatorialKalmanFilterExtensions<Acts::VectorMultiTrajectory>
        extensions;

    extensions.updater.connect<
        &Acts::GainMatrixUpdater::operator()<Acts::VectorMultiTrajectory>>(
        &kfUpdater);
    extensions.smoother.connect<
        &Acts::GainMatrixSmoother::operator()<Acts::VectorMultiTrajectory>>(
        &kfSmoother);
    extensions.measurementSelector
        .connect<&Acts::MeasurementSelector::select<Acts::VectorMultiTrajectory>>(
            &measSel);
    IndexSourceLinkAccessor slAccessor;
  slAccessor.container = &sourceLinks;
  Acts::SourceLinkAccessorDelegate<IndexSourceLinkAccessor::Iterator>
      slAccessorDelegate;
  slAccessorDelegate.connect<&IndexSourceLinkAccessor::range>(&slAccessor);

  // Set the CombinatorialKalmanFilter options
  TrackFindingAlgorithm::TrackFinderOptions options(
      ctx.geoContext, ctx.magFieldContext, ctx.calibContext, slAccessorDelegate,
      extensions, pOptions, &(*pSurface));
    // Perform the track finding for all initial parameters

    auto trackContainer = std::make_shared<Acts::VectorTrackContainer>();
  auto trackStateContainer = std::make_shared<Acts::VectorMultiTrajectory>();

  TrackContainer tracks(trackContainer, trackStateContainer);

  tracks.addColumn<unsigned int>("trackGroup");
  Acts::TrackAccessor<unsigned int> seedNumber("trackGroup");

  unsigned int nSeed = 0;

  for (std::size_t iseed = 0; iseed < initialParameters.size(); ++iseed) {
    auto result =
        (*m_cfg.findTracks)(initialParameters.at(iseed), options, tracks);
    // m_nTotalSeeds++;
    // nSeed++;

    if (!result.ok()) {
    //   m_nFailedSeeds++;
      ACTS_WARNING("Track finding failed for seed " << iseed << " with error"
                                                    << result.error());
      continue;
    }

    auto& tracksForSeed = result.value();
    for (auto& track : tracksForSeed) {
      seedNumber(track) = nSeed;
    }

    auto constTrackStateContainer =
      std::make_shared<Acts::ConstVectorMultiTrajectory>(
          std::move(*trackStateContainer));

  auto constTrackContainer = std::make_shared<Acts::ConstVectorTrackContainer>(
      std::move(*trackContainer));

  ConstTrackContainer constTracks{constTrackContainer,
                                  constTrackStateContainer};
    
    // m_outputTracks=std::move(constTracks);
    return ProcessCode::SUCCESS;
}
}

ProcessCode TrackFindingAlgorithm::finalize() {
  return ProcessCode::SUCCESS;
}