#include "AlgoBase.hh"
#include "VAnaManager.hh"
#include <unordered_map>

template<class T>
class ConvBaseline : public AlgoBase<T> {
public:
    ConvBaseline(const std::string &name) : AlgoBase<T>(name){
        std::cout<<name<<" algorithm constructed"<<std::endl;
    }
    int init(TTree* tin,TTree* tout) override{
        fTree = tin;
        fOutTree = tout;
        fTree->SetBranchAddress("conv_e",&conv_e);

        //Output variables
        fOutTree->Branch("Conv_E",&Conv_E);
        return 1;
        
    }
    int run(const int& ientry) override{
        //Reset branchs
        Conv_E = 0.;

        fTree->GetEntry(ientry);
        Conv_E = conv_e;
        
        return 1;
    }

private:
    TTree* fTree;
    TTree* fOutTree;

    //Branchs
    T conv_e;

    //Output treee branchs
    T Conv_E;

};