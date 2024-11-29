#ifndef VANAMANAGER_HH
#define VANAMANAGER_HH

#include <cstddef>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <typeinfo>
#include <unordered_map>
#include <utility>
#include <vector>

//ROOT
#include "TFile.h"
#include "TTree.h"
#include "TH2.h"
#include "TH2D.h"

//Acts
#include <Acts/Utilities/Logger.hpp>
#include "TGeoManager.h"
#include "Sequencer.hh"
#include "Measurement.hh"
#include "TrackFinder.hh"
#include "IndexSourceLink.hh"
#include "SpacePointMaker.hh"
#include "MeasurementCreation.hh"
#include "SpacePointMaker.hh"
#include "HoughVectors.hh"

#include <variant>
#include <algorithm>
#include <limits>
#include <memory>
#include <random>
#include <vector>

using std::cout;
using std::endl;
class TH2D;class TTree;
class TFile;
class VAnaManager
{
public:
    VAnaManager();
    ~VAnaManager();
    void setInputFile(const std::string& fname);
    int run();

private:
    TFile *fInputFile;
    TTree *fTree;
    TFile* fOutputFile;
    Sequencer *fSequencer;
    MeasurementCreator *fMeasurementCreator;
    TrackFinder *fTrackFinder;

    static constexpr std::array<double, 4> TrackerPosZ = {
        -245.775,
       -215.775,-184.775,-153.775
};
};

namespace std {
    template <>
    struct hash<std::array<double, 3>> {
        size_t operator()(const std::array<double, 3>& arr) const {
            size_t h1 = std::hash<double>{}(arr[0]);
            size_t h2 = std::hash<double>{}(arr[1]);
            size_t h3 = std::hash<double>{}(arr[2]);
            // 使用简单的合并策略，结合哈希值
            return h1 ^ (h2 << 1) ^ (h3 << 2); 
        }
    };
}

#endif