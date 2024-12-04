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

template<class T=float>
class VAnaManager{
public:
  static VAnaManager& getInstance(){
    static VAnaManager instance;
    return instance;
  }

  template<class TAlg>
  void RegisterAlgorithm(const TAlg* alg){
    static_assert(std::is_base_of<AlgoBase<float>,TAlg>::value, "TAlg must inherit from AlgoBase");
    fAlgorithms.emplace_back(alg);
  }

  int Initialize(){
    for(const auto &alg: fAlgorithms){
        if(!alg->init()){
            std::cout<<"Error: Algorithm " <<alg->getName()<<" initialization failed!"<<std::endl;
        }
    }
    std::cout<<"Analysis initialized!"<<std::endl;
    return 1;
}
  
  int Execute(){
    for(const auto &alg: fAlgorithms){
        if(!alg->run()){
            std::cout<<"Error: Algorithm " <<alg->getName()<<" execution failed!"<<std::endl;
        }
    }
    std::cout<<"Analysis finished!"<<std::endl;
    return 1;
}

  void Finalize(){}

private:
  VAnaManager() = default;
  ~VAnaManager() = default;
  VAnaManager(const VAnaManager&) = delete;
  VAnaManager& operator=(const VAnaManager&) = delete;

private:
  std::unique_ptr<TFile> fFile;
  std::unique_ptr<TTree> fTree;
  std::vector<AlgoBase<float>*> fAlgorithms;
  
};

#endif