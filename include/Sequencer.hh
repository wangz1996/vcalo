#ifndef SEQUENCER_HH
#define SEQUENCER_HH
#include <cstddef>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <typeinfo>
#include <unordered_map>
#include <utility>
#include <vector>
#include <iostream>
#include "SequenceElement.hh"
#include "DetectorInclude.hh"
#include "TelescopeDetectorElement.hh"
#include "IndexSourceLink.hh"


using namespace Acts::UnitLiterals;
class Sequencer{
public:
    struct Config {
    };
    Sequencer(const Config& cfg);
    ~Sequencer() = default;

    void addElement(std::shared_ptr<SequenceElement>& element);
    std::shared_ptr<const Acts::TrackingGeometry> buildDetector();

    int run();
    std::vector<Acts::GeometryIdentifier> getSurfaceGids() const { return m_surfacegids; }
    Acts::GeometryContext getGeoContext() const { return m_geoctx; }

private:
    Config m_cfg;
    std::vector<std::shared_ptr<SequenceElement>> m_sequenceElements;
    static constexpr int nTracker = 3;
    static constexpr std::array<double, nTracker> TrackerPosZ = {
        // -245.775,
        -215.775,-184.775,-153.775
};
    std::vector<std::shared_ptr<TelescopeDetectorElement>> detectorStore;
    Acts::GeometryContext m_geoctx;
    std::vector<Acts::GeometryIdentifier> m_surfacegids;
};

#endif