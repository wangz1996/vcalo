#ifndef TRACKFINDINGALGORITHM_HH
#define TRACKFINDINGALGORITHM_HH

#include "IAlgorithm.hh"
#include "Acts/EventData/TrackContainer.hpp"
#include "Acts/EventData/VectorMultiTrajectory.hpp"
#include "Acts/EventData/VectorTrackContainer.hpp"
#include "Acts/Geometry/TrackingGeometry.hpp"
#include "Acts/TrackFinding/CombinatorialKalmanFilter.hpp"
#include "Acts/TrackFinding/MeasurementSelector.hpp"
#include "Acts/TrackFinding/SourceLinkAccessorConcept.hpp"
#include "IndexSourceLink.hh"
#include "Track.hh"
#include "ProcessCode.hh"
#include "Measurement.hh"
#include "MagneticField.hh"
#include <atomic>
#include <functional>
#include <vector>

class TrackFindingAlgorithm final : public IAlgorithm {
public:
  using TrackFinderOptions =
      Acts::CombinatorialKalmanFilterOptions<IndexSourceLinkAccessor::Iterator,
                                             Acts::VectorMultiTrajectory>;
using TrackFinderResult =
      Acts::Result<std::vector<TrackContainer::TrackProxy>>;
class TrackFinderFunction {
   public:
    virtual ~TrackFinderFunction() = default;
    virtual TrackFinderResult operator()(const TrackParameters&,
                                         const TrackFinderOptions&,
                                         TrackContainer&) const = 0;
  };

  /// Create the track finder function implementation.
  ///
  /// The magnetic field is intentionally given by-value since the variant
  /// contains shared_ptr anyways.
  static std::shared_ptr<TrackFinderFunction> makeTrackFinderFunction(
      std::shared_ptr<const Acts::TrackingGeometry> trackingGeometry,
      std::shared_ptr<const Acts::MagneticFieldProvider> magneticField,
      const Acts::Logger& logger);

struct Config {
    /// Input measurements collection.
    std::string inputMeasurements;
    /// Input source links collection.
    std::string inputSourceLinks;
    /// Input initial track parameter estimates for for each proto track.
    std::string inputInitialTrackParameters;
    /// Output find trajectories collection.
    std::string outputTracks;
    /// Type erased track finder function.
    std::shared_ptr<TrackFinderFunction> findTracks;
    /// CKF measurement selector config
    Acts::MeasurementSelector::Config measurementSelectorCfg;
    /// Compute shared hit information
    bool computeSharedHits = false;
  };

  TrackFindingAlgorithm(Config config, Acts::Logging::Level level);

    ProcessCode execute(
      const AlgorithmContext& ctx) const final;

private:
  Config m_cfg;
  ProcessCode finalize() override;

  MeasurementContainer m_inputMeasurements;

  IndexSourceLinkContainer m_inputSourceLinks;

  TrackParametersContainer m_inputInitialTrackParameters;

  ConstTrackContainer m_outputTracks;

};




#endif