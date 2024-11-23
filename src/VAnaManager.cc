#include "VAnaManager.hh"
#include "Sequencer.hh"
#include "Measurement.hh"
#include "TrackFinder.hh"
VAnaManager::VAnaManager() {
  fSequencer = new Sequencer(Sequencer::Config());
  fMeasurementCreator = new MeasurementCreator();
  fTrackFinder = new TrackFinder();
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
    // fTree->GetEntry(0);
    // TGeoManager::Import("../config/tracker.gdml");
    //Test
    auto det_tracker = fSequencer->buildDetector();
    auto det_volume = det_tracker->highestTrackingVolume();
    auto gid = det_volume->geometryId();
    auto vec_meas = MeasurementContainer();
    auto layArrObjs = det_tracker->findVolume(gid)->confinedLayers()->arrayObjects();
    int index=0;
    IndexSourceLinkContainer inputSourceLinks;
    for(auto& lay : layArrObjs){
        auto gid = lay->geometryId();
        auto meas = fMeasurementCreator->createMeasurement(
      Acts::SourceLink{gid,0},
      std::array<Acts::BoundIndices, 2>{static_cast<Acts::BoundIndices>(0),static_cast<Acts::BoundIndices>(1)},
      Acts::ActsVector<2>(0,0),
      Acts::ActsSymMatrix<2>::Identity());
      vec_meas.emplace_back(meas);
      IndexSourceLink isl(gid,index);
      inputSourceLinks.emplace(std::move(isl));
      index++;
    }
    //Get Initial Parameters
    TrackParametersContainer initialParams;
    auto geoContext = fSequencer->getGeoContext();
    fTrackFinder->execute(geoContext,inputSourceLinks,det_tracker,initialParams);

    // std::cout<<"Value : "<<gid.value()<<std::endl;
    // auto det_trkvolume = det_tracker->findVolume(gid);
    
    return 1;
}