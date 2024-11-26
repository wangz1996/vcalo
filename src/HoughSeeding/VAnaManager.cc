#include "HoughSeeding/VAnaManager.hh"

template<class T>
static inline auto fz(const T& theta,const T& rho,const T& z)->T{
          return -1./tan(theta)*z+rho/sin(theta);
        };

VAnaManager::VAnaManager(){
    fFile = TFile::Open("test.root","READ");
    fTree = static_cast<TTree*>(fFile->Get("vtree"));

    tracker_hitx = nullptr;
    tracker_hity = nullptr;
    tracker_hitz = nullptr;
    tracker_hite = nullptr;

    fTree->SetBranchAddress("tracker_hitx",&tracker_hitx);
    fTree->SetBranchAddress("tracker_hity",&tracker_hity);
    fTree->SetBranchAddress("tracker_hitz",&tracker_hitz);
    fTree->SetBranchAddress("tracker_hite",&tracker_hite);
}

VAnaManager::~VAnaManager(){
    fFile->Close();
}

int VAnaManager::run(){
    for(size_t ientry=0;ientry<fTree->GetEntries();ientry++){
        auto seeds = getHoughSeeds(ientry);
        std::cout<<"Number of seeds: "<<seeds.size()<<std::endl;
    }
    return 1;
}

SpacePoints VAnaManager::getHoughSeeds(const int& entry){
    std::cout<<"Enter"<<std::endl;
    fTree->GetEntry(entry);
    HoughHist h_HT(1000,1000);
    SpacePoints sps;//initial space points
    std::unordered_set<SpacePoint> seed_set;
    SpacePoints seedposs; //Create empty container for output seed positions
    SpacePoints seeds;//Create empty container for output seeds
    for(size_t ihit=0;ihit<tracker_hitx->size();ihit++){
        float x = tracker_hitx->at(ihit);
        float y = tracker_hity->at(ihit);
        float z = tracker_hitz->at(ihit);
        sps.emplace_back(SpacePoint{x,y,z});
        for(size_t ibin=0;ibin<1000;ibin++){
            float theta = M_PI/1000.*ibin;
            float rho = z*cos(theta) + x*sin(theta);
            int jbin = int(rho+500.);
            h_HT(ibin,jbin).first++;
        }
    }
    std::cout<<"End of adding points starting to find seeds"<<std::endl;
    //Find seeds
    for(size_t ibin=0;ibin<1000;ibin++){
        for(size_t jbin=0;jbin<1000;jbin++){
            if(h_HT(ibin,jbin).first>10){
                std::cout<<h_HT(ibin,jbin).first<<std::endl;
                double theta = M_PI/1000.*ibin;
                double rho = jbin-500.;
                SpacePoints online_sps;
                for(auto &sp:sps){
                    double distance = abs(fz<float>(theta,rho,sp[2])-sp[0]);
                    std::cout<<distance<<std::endl;
                    if(distance<20.){
                        online_sps.emplace_back(sp);
                    }
                }
                std::sort(online_sps.begin(),online_sps.end(),[&](const SpacePoint& a,const SpacePoint& b){return a[2]<b[2];});
                if(seed_set.count(online_sps[0])==0){
                    seed_set.insert(online_sps[0]);
                    seeds.emplace_back(online_sps[0]);
                    size_t nsp = online_sps.size();
                    SpacePoint dir{online_sps[nsp-1][0]-online_sps[0][0],online_sps[nsp-1][1]-online_sps[0][1],online_sps[nsp-1][2]-online_sps[0][2]};
                    float mag = sqrt(dir[0]*dir[0]+dir[1]*dir[1]+dir[2]*dir[2]);
                    dir[0]/=mag;
                    dir[1]/=mag;
                    dir[2]/=mag;
                    seedposs.emplace_back(dir);
                }
            }
        }
    } //End of seed finding
    std::cout<<"End of finding seeds"<<std::endl;

    return seeds;
}