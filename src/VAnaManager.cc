#include "VAnaManager.hh"

VAnaManager::VAnaManager() {
  //std::cout << "VAnaManager::VAnaManager()" << std::endl;
}

VAnaManager::~VAnaManager() {
  //std::cout << "VAnaManager::~VAnaManager()" << std::endl;
}

void VAnaManager::setInputFile(const std::string& fname) {
    fInputFile = TFile::Open(TString(fname));
    fTree = (TTree*)fInputFile->Get("vtree");
}

int VAnaManager::run(){
    fTree->GetEntry(0);
    auto *geo = (TGeoManager*)fInputFile->Get("vcalo");
    auto geometryContext = Acts::GeometryContext();
    Acts::TGeoParser tgeoParser(geo);
    auto trackingGeometry = tgeoParser.parse(geometryContext);
    return 1;
}