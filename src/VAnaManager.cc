#include "VAnaManager.hh"
#include "Sequencer.hh"
#include "Measurement.hh"
#include "TrackFinder.hh"
#include "TRandom3.h"
VAnaManager::VAnaManager()
{
  fSequencer = new Sequencer(Sequencer::Config());
  fMeasurementCreator = new MeasurementCreator();
  fTrackFinder = new TrackFinder();
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
  auto vec_meas = MeasurementContainer();
  auto layArrObjs = det_geometry->findVolume(gid)->confinedLayers()->arrayObjects();
  int index = 0;
  // std::cout<<"Creating SourceLinks"<<std::endl;
  IndexSourceLinkContainer inputSourceLinks;
  // const Acts::PlaneSurface* rawSeedSurface = dynamic_cast<const Acts::PlaneSurface*>(layArrObjs[0]->surfaceArray()->at(0).at(0));

  // std::cout<<typeid(seedSurface).name()<<std::endl;
  Acts::ActsSymMatrix<2> measurementCovariance = Acts::ActsSymMatrix<2>::Zero();
  measurementCovariance(0, 0) = (1_mm) * (1_mm);
  measurementCovariance(1, 1) = (1_mm) * (1_mm);

  auto fAddMeas = [measurementCovariance,&vec_meas,&index,this,&inputSourceLinks](const double& x,const double& y,const auto& gid){
    auto meas = this->fMeasurementCreator->createMeasurement(
        Acts::SourceLink{gid, index},
        std::array<Acts::BoundIndices, 2>{Acts::eBoundLoc0, Acts::eBoundLoc1},
        Acts::ActsVector<2>(x,y),
        measurementCovariance);

        vec_meas.emplace_back(meas);
        IndexSourceLink isl(gid, index);
        inputSourceLinks.emplace(std::move(isl));
        index++;
  };
  // std::cout<<"Creating Measurements"<<std::endl;
  for (auto &lay : layArrObjs)
  {
    auto gid = lay->geometryId();
    //Generate random x y positions in a plane;
    for (int i = 0; i < 1000; i++){
        double x = random.Uniform(-10,10);
        double y = random.Uniform(-10,10);
        fAddMeas(x,y,gid);
    }
  }
  // Get Initial Parameters
  //  std::cout<<"Creating Initial Parameters"<<std::endl;
  TrackParametersContainer initialParams;
  auto seedSurface = Acts::Surface::makeShared<Acts::PerigeeSurface>(
      Acts::Vector3(0., 0., -250._mm));
  // 设置初始参数
  Acts::BoundVector seedParams = Acts::BoundVector::Zero();
  seedParams[Acts::eBoundLoc0] = 0;
  seedParams[Acts::eBoundLoc1] = 0;
  seedParams[Acts::eBoundPhi] = 0.1_mrad;       // ϕ = 0，沿 x-z 平面
  seedParams[Acts::eBoundTheta] = 0.1_mrad;     // θ = 0，沿正 z 方向
  seedParams[Acts::eBoundQOverP] = 1 / (1_GeV); // 假设动量为 1 GeV/c，电荷为 +1 e
  Acts::BoundSymMatrix cov = Acts::BoundSymMatrix::Zero();
  cov(Acts::eBoundLoc0, Acts::eBoundLoc0) = (0.1_mm) * (0.1_mm);
  cov(Acts::eBoundLoc1, Acts::eBoundLoc1) = (0.1_mm) * (0.1_mm);
  cov(Acts::eBoundPhi, Acts::eBoundPhi) = (1000_mrad) * (1000_mrad);
  cov(Acts::eBoundTheta, Acts::eBoundTheta) = (1000_mrad) * (1000_mrad);
  cov(Acts::eBoundQOverP, Acts::eBoundQOverP) = (0.1 / (1_GeV)) * (0.1 / (1_GeV));
  for(int i=0;i<100;i++){
    double x = random.Uniform(-10,10);
    double y = random.Uniform(-10,10);
    seedParams[Acts::eBoundLoc0] = x;
    seedParams[Acts::eBoundLoc1] = y;
    initialParams.emplace_back(seedSurface, seedParams, 1, cov);
  }
  // initialParams.emplace_back(seedSurface, seedParams, 1, cov);

  // std::cout<<"Do Track Finding"<<std::endl;
  fTrackFinder->execute(geoContext, inputSourceLinks, det_geometry, initialParams);

  // std::cout<<"Value : "<<gid.value()<<std::endl;
  // auto det_trkvolume = det_tracker->findVolume(gid);

  return 1;
}