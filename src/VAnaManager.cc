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
    TGeoManager::Import("../config/tracker.gdml");
    //Test
    auto siliconSurface = Acts::Surface::makeShared<Acts::PlaneSurface>(
        Acts::Vector3(0., 0., 0.), // 中心位置
        Acts::RectangleBounds(285.0 / 2.0, 285.0 / 2.0) // 几何尺寸
    );
    return 1;
}