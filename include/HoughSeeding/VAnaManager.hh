#ifndef VANAMANAGER_HH
#define VANAMANAGER_HH

#include <boost/container/small_vector.hpp>
#include <string>
#include <cmath>
#include <iostream>
#include <unordered_set>

#include "HoughVector.hh"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TH2D.h"


using SpacePoint = boost::container::small_vector<float,3>;
using SpacePoints = std::vector<SpacePoint>;
using HoughHist = vector2D<std::pair<int, std::unordered_set<unsigned>>>;


class VAnaManager{
public:
  VAnaManager();
  virtual ~VAnaManager();

  int run();

private:
  SpacePoints getHoughSeeds(const int& entry);
  

private:
  TFile *fFile;
  TTree *fTree;

  //Branch
  std::vector<float>* tracker_hitx;
  std::vector<float>* tracker_hity;
  std::vector<float>* tracker_hitz;
  std::vector<float>* tracker_hite;

};

namespace std {
    template <>
    struct hash<SpacePoint> {
        size_t operator()(const SpacePoint& arr) const {
            size_t h1 = std::hash<float>{}(arr[0]);
            size_t h2 = std::hash<float>{}(arr[1]);
            size_t h3 = std::hash<float>{}(arr[2]);
            // 使用简单的合并策略，结合哈希值
            return h1 ^ (h2 << 1) ^ (h3 << 2); 
        }
    };
}

#endif