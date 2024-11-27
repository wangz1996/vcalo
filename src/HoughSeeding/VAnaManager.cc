#include "HoughSeeding/VAnaManager.hh"

template <class T>
static inline auto fz(const T &theta, const T &rho, const T &z) -> T
{
    return -1. / tan(theta) * z + rho / sin(theta);
};

VAnaManager::~VAnaManager(){
    // fout->Close();
    // fFile->Close();
}

VAnaManager::VAnaManager()
{
    fFile = TFile::Open("test.root", "READ");
    fTree = static_cast<TTree *>(fFile->Get("vtree"));
    // fout=new TFile("hist.root","RECREATE");
    tracker_hitx = nullptr;
    tracker_hity = nullptr;
    tracker_hitz = nullptr;
    tracker_hite = nullptr;

    fTree->SetBranchAddress("tracker_hitx", &tracker_hitx);
    fTree->SetBranchAddress("tracker_hity", &tracker_hity);
    fTree->SetBranchAddress("tracker_hitz", &tracker_hitz);
    fTree->SetBranchAddress("tracker_hite", &tracker_hite);

    fTree->SetBranchAddress("init_x", &init_x);
    fTree->SetBranchAddress("init_y", &init_y);
    fTree->SetBranchAddress("init_z", &init_z);
    fTree->SetBranchAddress("init_Px", &init_Px);
    fTree->SetBranchAddress("init_Py", &init_Py);
    fTree->SetBranchAddress("init_Pz", &init_Pz);
}

int VAnaManager::run()
{
    auto fout = std::unique_ptr<TFile>(TFile::Open("vana.root", "RECREATE"));
    auto tout = std::unique_ptr<TTree>(new TTree("tout", "tout"));

    tout->Branch("init_x", &init_x);
    tout->Branch("init_y", &init_y);
    tout->Branch("init_z", &init_z);
    tout->Branch("init_Px", &init_Px);
    tout->Branch("init_Py", &init_Py);
    tout->Branch("init_Pz", &init_Pz);

    tout->Branch("reco_x", &reco_x);
    tout->Branch("reco_y", &reco_y);
    tout->Branch("reco_z", &reco_z);
    tout->Branch("reco_Px", &reco_Px);
    tout->Branch("reco_Py", &reco_Py);
    tout->Branch("reco_Pz", &reco_Pz);

    tout->Branch("seed_size", &seed_size);
    for (size_t ientry = 0; ientry < fTree->GetEntries(); ientry++)
    {
        // if(ientry>10)break;
        std::vector<float>().swap(reco_x);
        std::vector<float>().swap(reco_y);
        std::vector<float>().swap(reco_z);
        std::vector<float>().swap(reco_Px);
        std::vector<float>().swap(reco_Py);
        std::vector<float>().swap(reco_Pz);
        std::vector<int>().swap(seed_size);
        auto houghseeds = getHoughSeeds(ientry);
        auto seeds = std::get<0>(houghseeds);
        auto seedposs = std::get<1>(houghseeds);
        seed_size = std::get<2>(houghseeds);
        std::cout<<"NTracks: "<<seeds.size()<<std::endl;
        // std::cout<<"Entry: "<<ientry<<" "<<seeds.size()<<" seeds "<<seedposs.size()<<" dirs"<<std::endl;
        for(size_t iseed=0;iseed<seeds.size();iseed++){
            auto seed = seeds[iseed];
            auto seedpos = seedposs[iseed];
            reco_x.emplace_back(seed[0]);
            reco_y.emplace_back(seed[1]);
            reco_z.emplace_back(seed[2]);
            reco_Px.emplace_back(seedpos[0]);
            reco_Py.emplace_back(seedpos[1]);
            reco_Pz.emplace_back(seedpos[2]);
        }
        tout->Fill();
    }

    fout->cd();
    tout->Write();
    return 1;
}

HoughSeeds VAnaManager::getHoughSeeds(const int &entry)
{
    
    TString hname = TString("h")+TString(std::to_string(entry));
    TFile *fout=new TFile(hname+TString(".root"),"RECREATE");
    TH2D *h2=new TH2D(hname,"h2",nXbin,Xmin,Xmax,nYbin,Ymin,Ymax);
    TH3D *h3=new TH3D(hname+TString("3d"),"h3",100,-150,150,100,-150,150,100,-220.,-150);
    TH2D *hhit = new TH2D(hname+TString("hit"),"hit",100,-150,150,100,-150,150);
    fTree->GetEntry(entry);
    std::cout<<"Start processing entry: "<<entry<<" with "<<tracker_hitx->size()<<" hits"<<std::endl;
    //  if(init_Pz<0.7)return SpacePoints();
    HoughHist h_HT(nXbin, nYbin);
    SpacePoints sps; // initial space points
    std::unordered_set<SpacePoint> seed_set;
    std::vector<int> seed_size;
    SpacePoints seedposs; // Create empty container for output seed positions
    SpacePoints seeds;    // Create empty container for output seeds
    for (size_t ihit = 0; ihit < tracker_hitx->size(); ihit++)
    {
        float x = tracker_hitx->at(ihit);
        float y = tracker_hity->at(ihit);
        float z = tracker_hitz->at(ihit);
        hhit->Fill(x,y);
        h3->Fill(x,y,z);
        sps.emplace_back(SpacePoint{x, y, z});
        for (size_t ibin = 0; ibin < nXbin; ibin++)
        {
            float theta = Xgap * ibin;
            float rho = z * cos(theta) + x * sin(theta);
            // std::cout<<rho<<std::endl;
            int jbin = int((rho - Ymin) / Ygap);
            h_HT(ibin, jbin).first++;
        }
    }
    // std::cout<<"End of adding points starting to find seeds"<<std::endl;
    // Find seeds
    for (size_t ibin = 0; ibin < nXbin; ibin++)
    {
        for (size_t jbin = 0; jbin < nYbin; jbin++)
        {
            h2->SetBinContent(ibin+1,jbin+1,h_HT(ibin,jbin).first);
            // std::cout<<h_HT(ibin,jbin).first<<std::endl;
            if (h_HT(ibin, jbin).first > 60.)
            {
                // std::cout<<h_HT(ibin,jbin).first<<std::endl;
                double theta = Xgap * ibin;
                double rho = jbin * Ygap + Ymin;
                SpacePoints online_sps;
                for (auto &sp : sps)
                {
                    // std::cout<<sp[0]<<" "<<sp[1]<<" "<<sp[2]<<std::endl;
                    double distance = abs(fz<double>(theta, rho, sp[2]) - sp[0]);
                    // std::cout<<distance<<std::endl;
                    if (distance < 1.)
                    {
                        online_sps.emplace_back(sp);
                    }
                }
                auto sps_size = online_sps.size();

                if (sps_size < 5)
                    continue;
                // std::cout<<"Seed size: "<<sps_size<<std::endl;

                std::sort(online_sps.begin(), online_sps.end(), [&](const SpacePoint &a, const SpacePoint &b)
                          { return a[2] < b[2]; });
                if (online_sps[0][2] > TrackerPosZ[0] || online_sps[sps_size - 1][2] < TrackerPosZ[5]){
                    continue;
                    if(online_sps[0][2] > TrackerPosZ[0]){
                        std::cout<<"Seed start at "<<online_sps[0][2]<<" which is larger than "<<TrackerPosZ[0]<<std::endl;
                    }
                    else{
                        std::cout<<"Seed end at "<<online_sps[sps_size - 1][2]<<" which is smaller than "<<TrackerPosZ[5]<<std::endl;
                    }
                    continue;
                }
                if (seed_set.count(online_sps[0]) == 0)
                {
                    seed_size.emplace_back(sps_size);
                    seed_set.insert(online_sps[0]);
                    seeds.emplace_back(online_sps[0]);
                    size_t nsp = online_sps.size();
                    SpacePoint dir{online_sps[nsp - 1][0] - online_sps[0][0], online_sps[nsp - 1][1] - online_sps[0][1], online_sps[nsp - 1][2] - online_sps[0][2]};
                    float mag = sqrt(dir[0] * dir[0] + dir[1] * dir[1] + dir[2] * dir[2]);
                    dir[0] /= mag;
                    dir[1] /= mag;
                    dir[2] /= mag;
                    seedposs.emplace_back(dir);
                    // std::cout << online_sps[0][0] << " " << online_sps[0][1] << " " << online_sps[0][2] << std::endl;
                }
            }
        }
    } // End of seed finding
    // std::cout<<"End of finding seeds "<<seeds.size()<<std::endl;
    fout->cd();
    h2->Write();
    hhit->Write();
    h3->Write();
    fout->Close();

    // Select the seed with most points
    //  int max_size = 0;
    //  int tindex=-1;
    //  for(size_t i=0;i<seed_size.size();i++){
    //      if(seed_size[i]>max_size){
    //          max_size = seed_size[i];
    //          tindex = i;
    //      }
    //  }

    return std::make_tuple(seeds, seedposs, seed_size);
}