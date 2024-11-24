#pragma once

#include "Acts/Geometry/GeometryIdentifier.hpp"
#include "Acts/SpacePointFormation/SpacePointBuilder.hpp"
#include "IndexSourceLink.hh"
#include "Measurement.hh"
#include "SimSpacePoint.hpp"
#include "DataHandle.hh"
#include "IAlgorithm.hh"

#include <memory>
#include <string>
#include <vector>

class SpacePointMaker final : public IAlgorithm {
 public:
  struct Config {
    /// Input source links collection.
    std::string inputSourceLinks;
    /// Input measurements collection.
    std::string inputMeasurements;
    /// Output space points collection.
    std::string outputSpacePoints;
    /// Tracking geometry for transformation lookup.
    std::shared_ptr<const Acts::TrackingGeometry> trackingGeometry;
    /// For which part of the detector geometry should space points be created.
    ///
    /// Only volumes and layers can be set. Zero values can be used as wildcards
    /// to select larger parts of the hierarchy, i.e. setting only the volume
    /// selects all measurements within that volume. Adding a single identifier
    /// with all components set to zero selects all available measurements. The
    /// selection must not have duplicates.
    std::vector<Acts::GeometryIdentifier> geometrySelection;
  };

  /// Construct the space point maker.
  ///
  /// @param cfg is the algorithm configuration
  /// @param lvl is the logging level
  SpacePointMaker(Config cfg, Acts::Logging::Level lvl);

  /// Run the space point construction.
  ///
  /// @param ctx is the algorithm context with event information
  /// @return a process code indication success or failure
  ProcessCode execute(const AlgorithmContext& ctx) const override;

  /// Const access to the config
  const Config& config() const { return m_cfg; }

 private:
  Config m_cfg;

  Acts::SpacePointBuilder<SimSpacePoint> m_spacePointBuilder;

  ReadDataHandle<IndexSourceLinkContainer> m_inputSourceLinks{
      this, "InputSourceLinks"};

  ReadDataHandle<MeasurementContainer> m_inputMeasurements{this,
                                                           "InputMeasurements"};

  WriteDataHandle<SimSpacePointContainer> m_outputSpacePoints{
      this, "OutputSpacePoints"};
};