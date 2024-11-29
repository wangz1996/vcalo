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
  measurementCovariance(0, 0) = (1_mm) * (1_mm);
  // measurementCovariance(0, 1) = (0.5_mm) * (0.5_mm);
  measurementCovariance(1, 1) = (1_mm) * (1_mm);
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
    auto gid = sgids.at(i);
    
    // std::cout<<"surface x: "<<det_geometry->findSurface(gid)->center(geoContext).x()<<std::endl;
    // return 1;
    // double zinit = det_geometry->findSurface(sgids.at(0))->center(geoContext).z();
    double x = det_geometry->findSurface(gid)->center(geoContext).x();
    //Generate random x y positions in a plane;
    for (int ipp = 0; ipp < 1; ipp++){
        double z = random.Uniform(0.1,0.2);
        double y = random.Uniform(0.1,0.2);
        z = 10._mm;
        y = 10._mm;
        // sps.emplace_back(std::array<double,3>{x,y,z});
        //Add Hough points
        // for(int ibin=0;ibin<1000;ibin++){
        //   double theta = 3.1415926/1000.*ibin;
        //   double rho = z*cos(theta) + x*sin(theta);
        //   // std::cout<<theta<<" "<<rho<<std::endl;
        //   int jbin = int(rho+500.);
        //   h_HT(ibin,jbin).first++;
        // }

        fAddMeas(z,y,gid);
    }
  }
  auto fz = [](const double& theta,const double& rho,const double& z){
          return -1./tan(theta)*z+rho/sin(theta);
        };
  
  // std::unordered_set<std::array<double,3>> seeds;
  // std::vector<std::array<double,3>> seed_order;
  // std::vector<std::array<double,3>> seed_poss;
  // for(int ibin=0;ibin<1000;ibin++){
  //   for(int jbin=0;jbin<1000;jbin++){
  //     if(h_HT(ibin,jbin).first>5){
  //       double theta = 3.1415926/1000.*ibin;
  //       double rho = jbin-500.;
  //       // std::cout<<"SEED par: "<<theta<<" "<<rho<<std::endl
  //       std::array<double,3> seed_pos{0.,0.,0.};
  //       double minz=1000.;
  //       std::vector<double> dis;
  //       std::vector<std::array<double,3>> online_sps;
  //       for(auto sp:sps){
  //         double disx = abs(fz(theta,rho,sp[2])-sp[0]);
  //         if(disx<1.){
  //           online_sps.emplace_back(sp);
  //         }
  //       }
  //       std::sort(online_sps.begin(),online_sps.end(),[&](const std::array<double,3>& a,const std::array<double,3>& b){
  //         return a[2]<b[2];
  //       });
  //       if(seeds.count(online_sps[0])==0){
  //         size_t nsp = online_sps.size();
  //         std::array<double,3> dir{online_sps[nsp-1][0]-online_sps[0][0],online_sps[nsp-1][1]-online_sps[0][1],online_sps[nsp-1][2]-online_sps[0][2]};
  //         double mag = sqrt(dir[0]*dir[0]+dir[1]*dir[1]+dir[2]*dir[2]);
  //         dir[0] = dir[0]/mag;
  //         dir[1] = dir[1]/mag;
  //         dir[2] = dir[2]/mag;
  //         dir[0] = dir[0]==0. ? 1e-3 : dir[0];
  //         dir[1] = dir[1]==0. ? 1e-3 : dir[1];

  //         seed_poss.emplace_back(dir);
  //         seeds.insert(online_sps[0]);
  //         seed_order.emplace_back(online_sps[0]);
  //       }
  //     }
  //   }
  // }
  // std::cout<<"Number of seeds: "<<seeds.size()<<std::endl;
  // for(auto seed:seeds){
  //   std::cout<<seed[0]<<" "<<seed[1]<<" "<<seed[2]<<std::endl;
  // }

   std::cout<<"Creating Initial Parameters"<<std::endl;
  Acts::BoundVector stddev;
    stddev[Acts::eBoundLoc0] = 1_mm;
    stddev[Acts::eBoundLoc1] = 1_mm;
    // stddev[Acts::eBoundTime] = 25_ns;
    stddev[Acts::eBoundPhi] = 1_degree;
    stddev[Acts::eBoundTheta] = 1_degree;
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
std::vector<Acts::CurvilinearTrackParameters> startParameters;
    Acts::Vector4 mStartPos0(-250.125_mm, 10._mm, 10._mm, 0_ns);
    Acts::Vector3 mStartDir0(0.9999, 1e-5, 1e-5);
    startParameters = {
        {mStartPos0, mStartDir0, 1_GeV, 1_e, cov},
    };
  // std::cout<<"Do Track Finding"<<std::endl;
  fTrackFinder->execute(geoContext, inputSourceLinks, det_geometry, startParameters, vec_meas);
  

  // std::cout<<"Value : "<<gid.value()<<std::endl;
  // auto det_trkvolume = det_tracker->findVolume(gid);

  return 1;
}