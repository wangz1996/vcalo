#include "VAnaManager.hh"
#include "Sequencer.hh"
#include "Measurement.hh"
#include "TrackFinder.hh"

#include "TRandom3.h"
#include <TH2.h>
#include <TH2D.h>
#include <iostream>
using HoughHist = vector2D<std::pair<int, std::unordered_set<unsigned>>>;
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
   auto printgid = [](const auto& gid){
    std::cout<<"Surface gid: "<<gid.volume()<<" "<<gid.boundary()<<" "<<gid.layer()<<" "<<gid.approach()<<" "<<gid.sensitive()<<" "<<gid.extra()<<std::endl;
  };
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
  printgid(gid);
  // std::cout<<"det_volume gid: "<< gid.value()<<std::endl;
  auto vec_meas = MeasurementContainer();
  auto layArrObjs = det_geometry->findVolume(gid)->confinedLayers()->arrayObjects();
  Index index = 0;
  std::cout<<"Creating SourceLinks "<<layArrObjs.size()<<std::endl;
  IndexSourceLinkContainer inputSourceLinks;
  Acts::ActsSymMatrix<2> measurementCovariance = Acts::ActsSymMatrix<2>::Zero();
  measurementCovariance(0, 0) = (50_um) * (50_um);
  // measurementCovariance(0, 1) = (0.5_mm) * (0.5_mm);
  measurementCovariance(1, 1) = (50_um) * (50_um);
  // measurementCovariance(1, 0) = (0.5_mm) * (0.5_mm);
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
  
  std::cout<<"Check size: "<<sgids.size()<<std::endl;
  
  // HoughHist h_HT(1000,1000);
    // TH2D *h_HT = new TH2D("h_HT","h_HT",1000,0.,3.1415926,1000,-500.,500);
  //Create measurements and space points

  // std::vector<std::array<double,3>> sps;
  for (int i=0;i<sgids.size();i++)
  {
    // if(i==0)continue;
    // if(i==1)continue;
    auto gid = sgids.at(i);
    
    // std::cout<<"surface x: "<<det_geometry->findSurface(gid)->center(geoContext).x()<<std::endl;
    // return 1;
    // double zinit = det_geometry->findSurface(sgids.at(0))->center(geoContext).z();
    double x = det_geometry->findSurface(gid)->center(geoContext).x();
    //Generate random x y positions in a plane;
    double u = 10_mm;
    double v = 10_mm;
    fAddMeas(u,v,gid);
    // if(i==0)for(int j=0;j<100;j++)fAddMeas(u-j,v,gid);
  }
  auto fz = [](const double& theta,const double& rho,const double& z){
          return -1./tan(theta)*z+rho/sin(theta);
        };
  //Check measurments by looking at spacepoints
  // SimSpacePointContainer spacePoints;
  // Acts::SpacePointBuilderOptions spOpt;
  // spOpt.paramCovAccessor = [&vec_meas](Acts::SourceLink slink) {
  //   const auto islink = slink.get<IndexSourceLink>();
  //   const auto& meas = vec_meas[islink.index()];

  //   return std::visit(
  //       [](const auto& measurement) {
  //         auto expander = measurement.expander();
  //         Acts::BoundVector par = expander * measurement.parameters();
  //         Acts::BoundSymMatrix cov =
  //             expander * measurement.covariance() * expander.transpose();
  //         return std::make_pair(par, cov);
  //       },
  //       meas);
  // };
  // auto spBuilderConfig = Acts::SpacePointBuilderConfig();
  // spBuilderConfig.trackingGeometry = det_geometry;
  // auto spConstructor =
  //     [](const Acts::Vector3& pos, const Acts::Vector2& cov,
  //        boost::container::static_vector<Acts::SourceLink, 2> slinks)
  //     -> SimSpacePoint {
  //   return SimSpacePoint(pos, cov[0], cov[1], std::move(slinks));
  // };
  // auto m_spacePointBuilder = Acts::SpacePointBuilder<SimSpacePoint>(
  //     spBuilderConfig, spConstructor,
  //     Acts::getDefaultLogger("SpacePointBuilder", Acts::Logging::VERBOSE));

  // for(auto sl: inputSourceLinks){
  //   m_spacePointBuilder.buildSpacePoint(geoContext,{Acts::SourceLink{sl}},spOpt,std::back_inserter(spacePoints));
  // }
  // for(auto sp:spacePoints){
  //   std::cout<<"SpacePoint: "<<sp.x()<<" "<<sp.y()<<" "<<sp.z()<<std::endl;
  // }

   std::cout<<"Creating Initial Parameters"<<std::endl;
  Acts::BoundVector stddev;
    stddev[Acts::eBoundLoc0] = 50_um;
    stddev[Acts::eBoundLoc1] = 50_um;
    // stddev[Acts::eBoundTime] = 25_ns;
    stddev[Acts::eBoundPhi] = 0.1_degree;
    stddev[Acts::eBoundTheta] = 0.1_degree;
    stddev[Acts::eBoundQOverP] = 1 / 100._GeV;
    Acts::BoundSymMatrix cov = stddev.cwiseProduct(stddev).asDiagonal();
// Start parameters
// std::vector<Acts::CurvilinearTrackParameters> startParameters;
// for(size_t iseed=0;iseed<seeds.size();iseed++){
//   Acts::Vector4 mStartPos(seed_order[iseed][0], seed_order[iseed][1], seed_order[iseed][2]-20., 0_ns);
//   Acts::Vector3 mStartDir(seed_poss[iseed][0], seed_poss[iseed][1], seed_poss[iseed][2]);
//   std::cout<<seed_order[iseed][0]<<" "<<seed_order[iseed][1]<<" "<<seed_order[iseed][2]<<std::endl;
//   std::cout<<seed_poss[iseed][0]<<" "<<seed_poss[iseed][1]<<" "<<seed_poss[iseed][2]<<std::endl;
//   // Acts::Vector4 mStartPos(0.1_um, 0.1_um, -250.125_mm, 0_ns);
//   // Acts::Vector3 mStartDir(1e-5, 1e-5, 0.99999);
//   startParameters.push_back({mStartPos, mStartDir, 1_GeV, 1_e});
//   break;
// }
// global direction for reference
  // const Vector3 dir{1.,1e-5,1e-5};
  // // convert local-to-global for reference
  // const Vector2 loc(10._mm, 10._mm);
  // const Vector3 pos = surface->localToGlobal(geoContext, loc, dir);
  // global four-position as input
  // Vector4 pos4;
  // pos4.segment<3>(ePos0) = pos;
  // pos4[eTime] = 0_ns;
std::vector<Acts::CurvilinearTrackParameters> startParameters;
    Acts::Vector4 mStartPos0(-2500.775_mm, 10, -10, 1_ns);
    Acts::Vector3 mStartDir0(0.9999, 1e-5, 1e-5);
    Acts::CurvilinearTrackParameters mStartPar(mStartPos0,0_degree,90_degree,1_GeV,1_e,cov);
    auto planesur = dynamic_cast<const Acts::PlaneSurface*>(&mStartPar.referenceSurface());

        auto poscen = planesur->center(geoContext);
    std::cout<<"start surface center: "<<poscen.x()<<" "<<poscen.y()<<" "<<poscen.z()<<std::endl;
    auto normvec = planesur->normal(geoContext);
    std::cout<<"start surface normal: "<<normvec.x()<<" "<<normvec.y()<<" "<<normvec.z()<<std::endl;
    startParameters = {
        {mStartPos0, 0_degree, 90_degree, 1_GeV, 1_e, cov},
        // {mStartPos1, -1_degree, 91_degree, 1_GeV, 1_e, cov},
        // {mStartPos2, 1_degree, 89_degree, 1_GeV, -1_e, cov},
    };
  // std::cout<<"Do Track Finding"<<std::endl;
  fTrackFinder->execute(geoContext, inputSourceLinks, det_geometry, startParameters, vec_meas);
  

  // std::cout<<"Value : "<<gid.value()<<std::endl;
  // auto det_trkvolume = det_tracker->findVolume(gid);

  return 1;
}