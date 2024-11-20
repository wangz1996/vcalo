#ifndef VANAMANAGER_HH
#define VANAMANAGER_HH

#include <iostream>
#include <string>
#include <vector>
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TGeoManager.h"
#include <fstream>
#include <memory>
#include <ranges>
#include <filesystem>

//Acts
#include <Acts/Geometry/TrackingGeometry.hpp>
#include <Acts/Surfaces/PlaneSurface.hpp>
#include <Acts/TrackFitting/KalmanFitter.hpp>
#include <Acts/Utilities/Logger.hpp>
#include <Acts/EventData/TrackParameters.hpp>
// #include <Acts/EventData/Measurement.hpp>
#include <Acts/Surfaces/RectangleBounds.hpp>
using namespace Acts::UnitLiterals;

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

};

#endif