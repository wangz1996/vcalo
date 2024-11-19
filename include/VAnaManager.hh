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
#include <filesystem>

//Acts
#include <Acts/Plugins/TGeo/TGeoDetectorElement.hpp>
#include <Acts/Plugins/TGeo/TGeoParser.hpp>
#include <Acts/Geometry/TrackingGeometry.hpp>

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