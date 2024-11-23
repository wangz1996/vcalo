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

    Acts::GeometryContext getGeoContext() const { return m_geoctx; }

private:
    Config m_cfg;
    std::vector<std::shared_ptr<SequenceElement>> m_sequenceElements;
    static constexpr std::array<double, 6> TrackerPosZ = {
    		25.175 - 262.3, 30.135 - 262.3, 55.835 - 262.3,
    		60.795 - 262.3, 86.495 - 262.3, 91.455 - 262.3
		};
    std::vector<std::shared_ptr<TelescopeDetectorElement>> detectorStore;
    Acts::GeometryContext m_geoctx;
};

#endif