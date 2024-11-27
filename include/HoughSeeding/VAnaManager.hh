#ifndef VANAMANAGER_HH
#define VANAMANAGER_HH

#include <boost/container/small_vector.hpp>
#include <string>
#include <cmath>
#include <iostream>
#include <unordered_set>
#include <tuple>

#include "HoughVector.hh"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TH2D.h"
#include "TH3D.h"
#include "TLorentzVector.h"


using SpacePoint = boost::container::small_vector<float,3>;
using SpacePoints = std::vector<SpacePoint>;
using HoughHist = vector2D<std::pair<int, std::unordered_set<unsigned>>>;

using HoughSeeds = std::tuple<SpacePoints,SpacePoints,std::vector<int>>;


class VAnaManager{
public:
  VAnaManager();
  virtual ~VAnaManager();

  int run();

private:
  HoughSeeds getHoughSeeds(const int& entry);
  

private:
  TFile *fFile;
  TTree *fTree;

  //Branch
  std::vector<float>* tracker_hitx;
  std::vector<float>* tracker_hity;
  std::vector<float>* tracker_hitz;
  std::vector<float>* tracker_hite;
  float init_x;
  float init_y;
  float init_z;
  float init_Px;
  float init_Py;
  float init_Pz;

  //Output branches
  std::vector<float> reco_x;
  std::vector<float> reco_y;
  std::vector<float> reco_z;
  std::vector<float> reco_Px;
  std::vector<float> reco_Py;
  std::vector<float> reco_Pz;
  std::vector<int> seed_size;

  //Hough
  static constexpr int nXbin = 7000;
  static constexpr double Xmin = 0.;
  static constexpr double Xmax = M_PI;
  static constexpr double Xgap = (Xmax-Xmin)/nXbin;
  static constexpr int nYbin = 200;
  static constexpr double Ymin = -300.;
  static constexpr double Ymax = 300.;
  static constexpr double Ygap = (Ymax-Ymin)/nYbin;

  static constexpr std::array<float, 6> TrackerPosZ = {
    	-218.425, -213.125, -187.425,
    	-182.125, -156.425, -151.125
	};
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