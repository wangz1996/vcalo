#include "VAnaManager.hh"
#include "Sequencer.hh"
#include "Measurement.hh"
#include "TrackFinder.hh"
#include "TRandom3.h"
#include <TH2.h>
#include <TH2D.h>
#include <iostream>
VAnaManager::VAnaManager()
{
  fSequencer = new Sequencer(Sequencer::Config());
  fMeasurementCreator = new MeasurementCreator();
  fTrackFinder = new TrackFinder();
  fOutputFile = new TFile("vana.root","RECREATE");
  // std::cout << "VAnaManager::VAnaManager()" << std::endl;
}

VAnaManager::~VAnaManager()
{
  // std::cout << "VAnaManager::~VAnaManager()" << std::endl;
}

void VAnaManager::setInputFile(const std::string &fname)
{
  // fInputFile = TFile::Open(TString(fname));
  // fTree = (TTree *)fInputFile->Get("vtree");
}

int VAnaManager::run()
{
  TRandom3 random(0);
  auto geoContext = fSequencer->getGeoContext();
  // std::cout<<"Start run"<<std::endl;
  Acts::Logging::Level(Acts::Logging::VERBOSE);
  // fTree->GetEntry(0);
  // TGeoManager::Import("../config/tracker.gdml");
  // Test
  auto det_geometry = fSequencer->buildDetector();
  
  // std::cout<<"Built Detector"<<std::endl;
  // auto det_tracker = std::make_shared<Acts::TrackingGeometry>(std::move(fSequencer->buildDetector()));
  auto det_volume = det_geometry->highestTrackingVolume();
  auto gid = det_volume->geometryId();
  // std::cout<<"det_volume gid: "<< gid.value()<<std::endl;
  auto vec_meas = MeasurementContainer();
  auto layArrObjs = det_geometry->findVolume(gid)->confinedLayers()->arrayObjects();
  Index index = 0;
  // std::cout<<"Creating SourceLinks"<<std::endl;
  IndexSourceLinkContainer inputSourceLinks;
  Acts::ActsSymMatrix<2> measurementCovariance = Acts::ActsSymMatrix<2>::Zero();
  measurementCovariance(0, 0) = (1_mm) * (1_mm);
  measurementCovariance(1, 1) = (1_mm) * (1_mm);
  // inputSourceLinks.emplace(IndexSourceLink{gid,index-1});

  auto fAddMeas = [measurementCovariance,&vec_meas,&index,this,&inputSourceLinks](const double& x,const double& y,const auto& gid){
    auto meas = this->fMeasurementCreator->createMeasurement(
        
        Acts::SourceLink{gid, IndexSourceLink{gid,index}},
        std::array<Acts::BoundIndices, 2>{Acts::eBoundLoc0, Acts::eBoundLoc1},
        Acts::ActsVector<2>(x,y),
        measurementCovariance);
        std::cout<<"GID Layer: "<<gid.layer()<<std::endl;
        vec_meas.emplace_back(meas);
        IndexSourceLink isl(gid, index);
        inputSourceLinks.emplace(std::move(isl));
        index++;
  };
  // std::cout<<"Creating Measurements"<<std::endl;
  //TODO change to for loop initialization with C++ 20
  auto sgids = fSequencer->getSurfaceGids();
  auto printgid = [](const auto& gid){
    std::cout<<"Surface gid: "<<gid.volume()<<" "<<gid.boundary()<<" "<<gid.layer()<<" "<<gid.approach()<<" "<<gid.sensitive()<<" "<<gid.extra()<<std::endl;
  };

  //Create measurements and space points
  for (int i=0;i<sgids.size();i++)
  {
    // if(i==0)continue;
    auto gid = sgids.at(i);
    std::cout<<"surface z: "<<det_geometry->findSurface(gid)->center(geoContext).z()<<std::endl;
    //Generate random x y positions in a plane;
    for (int i = 0; i < 1; i++){
        double x = random.Uniform(-10,10);
        double y = random.Uniform(-10,10);
        x = 0.01_um;
        y = 0.01_um;
        fAddMeas(x,y,gid);
    }
  }
  //Build Space Points
  Acts::SpacePointBuilderOptions spOpt;
  spOpt.paramCovAccessor = [&vec_meas](Acts::SourceLink slink) {
    const auto islink = slink.get<IndexSourceLink>();
    const auto& meas = vec_meas[islink.index()];
    return std::visit(
        [](const auto& measurement) {
          auto expander = measurement.expander();
          Acts::BoundVector par = expander * measurement.parameters();
          Acts::BoundSymMatrix cov =
              expander * measurement.covariance() * expander.transpose();
          return std::make_pair(par, cov);
        },
        meas);
  };
  //Check measurements
  for(auto meas: vec_meas){
    std::visit([this](auto&& measurement){
      auto pv = measurement.parameters();
      for(auto ipv:pv){std::cout<<ipv<<" ";}
      std::cout<<std::endl;
        auto gid = measurement.sourceLink().geometryId();
        std::cout<<gid.volume()<<" "<<gid.boundary()<<" "<<gid.layer()<<" "<<gid.approach()<<" "<<gid.sensitive()<<" "<<gid.extra()<<std::endl;
    },meas);
  }
  // //Check SourceLinks
  // for(auto isl: inputSourceLinks){
  //   auto gid = isl.geometryId();
  //   std::cout<<"isl gid: "<<gid.volume()<<" "<<gid.boundary()<<" "<<gid.layer()<<" "<<gid.approach()<<" "<<gid.sensitive()<<" "<<gid.extra()<<std::endl;
  // }
  // std::cout<<"Creating TrackFinder"<<std::endl;
   std::cout<<"Creating Initial Parameters"<<std::endl;
  Acts::BoundVector stddev;
    stddev[Acts::eBoundLoc0] = 1_mm;
    stddev[Acts::eBoundLoc1] = 1_mm;
    // stddev[Acts::eBoundTime] = 25_ns;
    stddev[Acts::eBoundPhi] = 0.1_degree;
    stddev[Acts::eBoundTheta] = 0.1_degree;
    stddev[Acts::eBoundQOverP] = 1 / 100._GeV;
    Acts::BoundSymMatrix cov = stddev.cwiseProduct(stddev).asDiagonal();
// Start parameters
std::vector<Acts::CurvilinearTrackParameters> startParameters;
    Acts::Vector4 mStartPos0(0.1_um, 0.1_um, -250.125_mm, 0_ns);
    startParameters = {
        {mStartPos0, 0.1_degree, 0.1_degree, 1_GeV, 1_e, cov},
    };

  // std::cout<<"Do Track Finding"<<std::endl;
  fTrackFinder->execute(geoContext, inputSourceLinks, det_geometry, startParameters, vec_meas);
  

  // std::cout<<"Value : "<<gid.value()<<std::endl;
  // auto det_trkvolume = det_tracker->findVolume(gid);

  return 1;
}