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
    static constexpr std::array<double, 6> TrackerPosZ = {
    		25.175 - 262.3, 30.135 - 262.3, 55.835 - 262.3,
    		60.795 - 262.3, 86.495 - 262.3, 91.455 - 262.3
		};
};

#endif