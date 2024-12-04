#ifndef VANAMANAGER_HH
#define VANAMANAGER_HH

#include <boost/container/small_vector.hpp>
#include <string>
#include <cmath>
#include <iostream>
#include <vector>
#include <unordered_set>

#include "TFile.h"
#include "TTree.h"
#include "AlgoBase.hh"
#include "ECALBaseline.hh"
#include "TruthBaseline.hh"
#include "TrackerBaseline.hh"

template<class T=float>
class VAnaManager{
public:
  static VAnaManager& getInstance(){
    static VAnaManager instance;
    return instance;
  }

  int Initialize(){
    // TODO cannot open file and tree with smart pointer
    auto fFile = TFile::Open("test.root","READ");
    fTree = (static_cast<TTree*>(fFile->Get("vtree")));

    fOutFile = new TFile("vana.root","RECREATE");
    fOutTree = new TTree("vtree","vtree");

    //Register algorithm
    RegisterAlgorithm<ECALBaseline<float>>(new ECALBaseline<float>("ECALBaseline"));
    RegisterAlgorithm<TruthBaseline<float>>(new TruthBaseline<float>("TruthBaseline"));

    for(auto &alg: fAlgorithms){
        if(!alg->init(fTree,fOutTree)){
            std::cout<<"Error: Algorithm " <<alg->getName()<<" initialization failed!"<<std::endl;
        }
    }
    std::cout<<"Analysis initialized!"<<std::endl;
    return 1;
}
  
  int Execute(){
    for (size_t i = 0; i < fTree->GetEntries(); i++)
    {
      fTree->GetEntry(i);
      for (auto &alg : fAlgorithms)
      {
        if (!alg->run(i))
        {
          std::cout << "Error: Algorithm " << alg->getName() << " execution failed!" << std::endl;
        }
      }
      fOutTree->Fill();
    }
    std::cout<<"Analysis finished!"<<std::endl;

    return 1;
}

  void Finalize(){
    fOutFile->cd();
    fOutTree->Write();
    fOutFile->Close();
  }

private:
  VAnaManager() = default;
  ~VAnaManager(){}
  VAnaManager(const VAnaManager&) = delete;
  VAnaManager& operator=(const VAnaManager&) = delete;

  template<class TAlg>
  void RegisterAlgorithm(TAlg* alg){
    static_assert(std::is_base_of<AlgoBase<float>,TAlg>::value, "TAlg must inherit from AlgoBase");
    fAlgorithms.emplace_back(alg);
  }

private:
  TTree* fTree;
  TFile* fOutFile;
  TTree* fOutTree;

  std::vector<AlgoBase<float>*> fAlgorithms;
  
};

#endif