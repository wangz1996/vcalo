#include "AlgoBase.hh"
#include "VAnaManager.hh"
#include <unordered_map>

template<class T>
class TruthBaseline : public AlgoBase<T> {
public:
    TruthBaseline(const std::string &name) : AlgoBase<T>(name){
        std::cout<<name<<" algorithm constructed"<<std::endl;
    }
    int init(TTree* tin,TTree* tout) override{
        fTree = tin->CloneTree();
        fOutTree = tout;
        fTree->SetBranchAddress("isconv",&isconv);
        fTree->SetBranchAddress("conve_inECAL",&conve_inECAL);
        fTree->SetBranchAddress("convp_inECAL",&convp_inECAL);
        fTree->SetBranchAddress("init_x",&init_x);
        fTree->SetBranchAddress("init_y",&init_y);
        fTree->SetBranchAddress("init_z",&init_z);
        fTree->SetBranchAddress("init_Px",&init_Px);
        fTree->SetBranchAddress("init_Py",&init_Py);
        fTree->SetBranchAddress("init_Pz",&init_Pz);
        fTree->SetBranchAddress("init_E",&init_E);
        fTree->SetBranchAddress("init_Ke",&init_Ke);
        fTree->SetBranchAddress("conve_kinematic",&conve_kinematic);
        fTree->SetBranchAddress("convp_kinematic",&convp_kinematic);

        //Output variables
        fOutTree->Branch("Truth_init",&Truth_init);
        fOutTree->Branch("Truth_isconv",&Truth_isconv);
        fOutTree->Branch("Truth_conve_inECAL",&Truth_conve_inECAL);
        fOutTree->Branch("Truth_convp_inECAL",&Truth_convp_inECAL);
        fOutTree->Branch("Truth_conve",&Truth_conve);
        fOutTree->Branch("Truth_convp",&Truth_convp);
        return 1;
        
    }
    int run(const int& ientry) override{
        //Reset branchs
        Truth_isconv=0;
        Truth_conve_inECAL=0;
        Truth_convp_inECAL=0;
        Truth_init.clear();
        Truth_conve.clear();
        Truth_convp.clear();
        

        fTree->GetEntry(ientry);
        Truth_conve.reserve(conve_kinematic->size());
        Truth_convp.reserve(convp_kinematic->size());
        Truth_isconv = isconv;
        Truth_conve_inECAL = conve_inECAL;
        Truth_convp_inECAL = convp_inECAL;
        Truth_init.emplace_back(init_x);
        Truth_init.emplace_back(init_y);
        Truth_init.emplace_back(init_z);
        Truth_init.emplace_back(init_Px);
        Truth_init.emplace_back(init_Py);
        Truth_init.emplace_back(init_Pz);
        Truth_init.emplace_back(init_E);
        Truth_init.emplace_back(init_Ke);

        std::copy(conve_kinematic->begin(),conve_kinematic->end(),Truth_conve.begin());
        std::copy(convp_kinematic->begin(),convp_kinematic->end(),Truth_convp.begin());
        return 1;
    }

private:
    TTree* fTree;
    TTree* fOutTree;

    //Branchs
    int eventNo;
    T init_x;
    T init_y;
    T init_z;
    T init_Px;
    T init_Py;
    T init_Pz;
    T init_E;
    T init_Ke;
    int isconv;
    int conve_inECAL;
    int convp_inECAL;
    std::vector<T>* conve_kinematic=nullptr;
    std::vector<T>* convp_kinematic=nullptr;

    //Output treee branchs
    std::vector<T> Truth_init; //x y z Px Py Pz E Ke
    int Truth_isconv;
    int Truth_conve_inECAL;
    int Truth_convp_inECAL;
    std::vector<T> Truth_conve; //x y z Px Py Pz E theta phi Ke
    std::vector<T> Truth_convp; //x y z Px Py Pz E theta phi Ke

};