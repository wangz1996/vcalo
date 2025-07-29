#include "AlgoBase.hh"
#include "VAnaManager.hh"
#include <unordered_map>

template<class T>
class TrackerBaseline : public AlgoBase<T> {
public:
    TrackerBaseline(const std::string &name) : AlgoBase<T>(name){
        std::cout<<name<<" algorithm constructed"<<std::endl;
    }
    int init(TTree* tin,TTree* tout) override{
        fTree = tin->CloneTree();
        fOutTree = tout;
        fTree->SetBranchAddress("tracker_hitx",&tracker_hitx);
        fTree->SetBranchAddress("tracker_hity",&tracker_hity);
        fTree->SetBranchAddress("tracker_hitz",&tracker_hitz);
        fTree->SetBranchAddress("tracker_hite",&tracker_hite);

        //Output variables
        fOutTree->Branch("Tracker_hitx",&Tracker_hitx);
        fOutTree->Branch("Tracker_hity",&Tracker_hity);
        fOutTree->Branch("Tracker_hitz",&Tracker_hitz);
        fOutTree->Branch("Tracker_hite",&Tracker_hite);
        return 1;
    }
    int run(const int& ientry) override{
        //Reset branchs
        Tracker_hitx.clear();
        Tracker_hity.clear();
        Tracker_hitz.clear();
        Tracker_hite.clear();

        fTree->GetEntry(ientry);
        Tracker_hitx.resize(tracker_hitx->size());
        Tracker_hity.resize(tracker_hity->size());
        Tracker_hitz.resize(tracker_hitz->size());
        Tracker_hite.resize(tracker_hite->size());

        std::copy(tracker_hitx->begin(),tracker_hitx->end(),Tracker_hitx.begin());
        std::copy(tracker_hity->begin(),tracker_hity->end(),Tracker_hity.begin());
        std::copy(tracker_hitz->begin(),tracker_hitz->end(),Tracker_hitz.begin());
        std::copy(tracker_hite->begin(),tracker_hite->end(),Tracker_hite.begin());
        return 1;
    }

private:
    TTree* fTree;
    TTree* fOutTree;

    //Branchs
    std::vector<T> *tracker_hitx=nullptr;
    std::vector<T> *tracker_hity=nullptr;
    std::vector<T> *tracker_hitz=nullptr;
    std::vector<T> *tracker_hite=nullptr;

    //Output treee branchs
    std::vector<T> Tracker_hitx;
    std::vector<T> Tracker_hity;
    std::vector<T> Tracker_hitz;
    std::vector<T> Tracker_hite;
};