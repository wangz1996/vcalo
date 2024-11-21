#include "VAnaManager.hh"
#include "Sequencer.hh"
VAnaManager::VAnaManager() {
  fSequencer = new Sequencer(Sequencer::Config());
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
    
    return 1;
}