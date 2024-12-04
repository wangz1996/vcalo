#include "AlgoBase.hh"
#include "VAnaManager.hh"
#include <unordered_map>

template<class T>
class ECALBaseline : public AlgoBase<T> {
public:
    ECALBaseline(const std::string &name) : AlgoBase<T>(name){
        std::cout<<name<<" algorithm constructed"<<std::endl;
    }
    int init(TTree* tin,TTree* tout) override{
        fTree = tin;
        fOutTree = tout;
        fTree->SetBranchAddress("ecal_cellid",&ecal_cellid);
        fTree->SetBranchAddress("ecal_celle",&ecal_celle);

        //Output variables
        ECAL_maxid=-1; ECAL_recoE=0.;
        fOutTree->Branch("ECAL_maxid",&ECAL_maxid);
        fOutTree->Branch("ECAL_recoE", &ECAL_recoE);

        return 1;
    }
    int run(const int& ientry) override{
        //Reset branchs
        ECAL_recoE = 0.;
        fTree->GetEntry(ientry);
        //Find max cell
        int maxid=-1;
        T maxe = 0.;
        std::unordered_map<int,T> umap_celle;
        for(size_t i=0;i<ecal_cellid->size();i++){
            umap_celle[ecal_cellid->at(i)] = ecal_celle->at(i);
            if(ecal_celle->at(i)>maxe){
                maxe=ecal_celle->at(i);
                maxid=ecal_cellid->at(i);
            }
        }
        ECAL_maxid = maxid;

        //Reconstruct energy
        ECAL_recoE += (umap_celle[maxid-6] + umap_celle[maxid-5] + umap_celle[maxid-4]
                    + umap_celle[maxid-1] + umap_celle[maxid] + umap_celle[maxid+1]
                    + umap_celle[maxid+4] + umap_celle[maxid+5] + umap_celle[maxid+6]);
        return 1;
    }

private:
    TTree* fTree;
    TTree* fOutTree;
    std::unordered_map<int,int> umap_centerId;

    //Branchs
    std::vector<T> *ecal_celle=nullptr;
    std::vector<int> *ecal_cellid=nullptr;

    //Output treee branchs
    int ECAL_maxid;
    T ECAL_recoE;
};