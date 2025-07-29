#include "AlgoBase.hh"
#include "VAnaManager.hh"
#include "KDTree/ORTCalculator.hh"
#include <unordered_map>
#include "global.hh"
#include <utility>
#include <cmath>
#include <unordered_map>

template <class T>
class ParticleNet : public AlgoBase<T>
{
public:
    ParticleNet(const std::string &name) : AlgoBase<T>(name)
    {
        std::cout << name << " algorithm constructed" << std::endl;
    }
    int init(TTree *tin, TTree *tout) override
    {
        fTree = tin->CloneTree();
        fOutTree = tout;
        fTree->SetBranchAddress("conv_e", &conv_e);
        fTree->SetBranchAddress("ecal_cellid", &ecal_cellid);
        fTree->SetBranchAddress("ecal_celle", &ecal_celle);
        fTree->SetBranchAddress("tracker_hitx", &tracker_hitx);
        fTree->SetBranchAddress("tracker_hity", &tracker_hity);
        fTree->SetBranchAddress("tracker_hitz", &tracker_hitz);
        fTree->SetBranchAddress("tracker_hite", &tracker_hite);

        //Output variables
        fOutTree->Branch("GNN_isconv",&GNN_isconv);

        //ORT Calculator
        ortc = new ORTCalculator();
        return 1;
    }
    int run(const int &ientry) override
    {
        // Reset branchs
        fTree->GetEntry(ientry);
        ortc->reset();
        ortc->addPoint(0.,0.,-251.8,conv_e);
        for(size_t i=0;i<tracker_hitx->size();i++)
        {
            float x = tracker_hitx->at(i);
            float y = tracker_hity->at(i);
            float z = tracker_hitz->at(i);
            float e = 0.1;
            ortc->addPoint(x,y,z,e);
        }
        for(size_t i=0;i<ecal_celle->size();i++)
        {
            float e = ecal_celle->at(i);
            int id = ecal_cellid->at(i);
            float x = ecal_id_xy[id].first;
            float y = ecal_id_xy[id].second;
            ortc->addPoint(x,y,0.1,e);
        }
        GNN_isconv = ortc->getScore();
        return 1;
    }

private:
    TTree *fTree;
    TTree *fOutTree;
    ORTCalculator *ortc;
    std::unordered_map<int, int> umap_centerId;
    // Branchs
    std::vector<T> *ecal_celle = nullptr;
    std::vector<int> *ecal_cellid = nullptr;
    std::vector<T> *tracker_hitx = nullptr;
    std::vector<T> *tracker_hity = nullptr;
    std::vector<T> *tracker_hitz = nullptr;
    std::vector<T> *tracker_hite = nullptr;
    T conv_e;

    // Output treee branchs
    T GNN_isconv = 0.;
};