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

//Acts
#include <Acts/Utilities/Logger.hpp>
#include "TFile.h"
#include "TTree.h"
#include "TGeoManager.h"
#include "Sequencer.hh"
#include "Measurement.hh"
#include "TrackFinder.hh"
#include "IndexSourceLink.hh"
#include <variant>

using std::cout;
using std::endl;

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
    Sequencer *fSequencer;
    MeasurementCreator *fMeasurementCreator;
    TrackFinder *fTrackFinder;
};

#endif