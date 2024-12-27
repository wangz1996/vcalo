#ifndef KNNCALCULATOR_HH
#define KNNCALCULATOR_HH
#include "KDTree/PointCloud.hh"

template<class T>
class KNNCalculator {
public:
  KNNCalculator(){
    cloud = new PointCloud();
  }
  ~KNNCalculator(){
    if(cloud != nullptr){
      delete cloud;
      cloud = nullptr;
    }
  }
  void reset(){
    cloud->points.clear();
  }
  void addPoint(const T& x, const T& y){
    cloud->points.emplace_back(std::vector<float>{x,y});
  }
  void run(const int& k){
    //Clear edge_index
    edge_index.clear();
    KDTree kdtree(2, *cloud, nanoflann::KDTreeSingleIndexAdaptorParams(10));
    kdtree.buildIndex();
    std::vector<int64_t> edge_col1,edge_col2;
        for(int i=0;i<cloud->points.size();i++){
            float query_pt[2] = {cloud->points[i][0], cloud->points[i][1]};
            std::vector<unsigned> ret_indexes(k);
            std::vector<float> out_dist_sqr(k);
            kdtree.knnSearch(&query_pt[0], k, &ret_indexes[0], &out_dist_sqr[0]);
            for (size_t j = 0; j < ret_indexes.size(); j++) {
            if (i != ret_indexes[j]) {
                edge_col1.emplace_back(i);
                edge_col2.emplace_back(ret_indexes[j]);
            }
            }
        }
        edge_index.insert(edge_index.end(), edge_col1.begin(), edge_col1.end());
        edge_index.insert(edge_index.end(), edge_col2.begin(), edge_col2.end());
  }
  std::vector<int64_t> getEdgeIndex(){return edge_index;}
  
private:
  void calculateKNN(const int& k);

  std::vector<int64_t> edge_index;
  PointCloud* cloud;
};

#endif