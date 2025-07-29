#ifndef POINTCLOUD_HH
#define POINTCLOUD_HH
#include <iostream>
#include <unordered_map>
#include <vector>
#include <cmath>
#include <limits>
#include "nanoflann.hpp"
class PointCloud;
struct Point{
    int index;
    float x,y,z;
};
using KDTree = nanoflann::KDTreeSingleIndexAdaptor<
        nanoflann::L2_Simple_Adaptor<float, PointCloud>,
        PointCloud, 3 /* 维度 */
    >;
class PointCloud{
public:
    PointCloud()=default;
    ~PointCloud()=default;
    inline size_t kdtree_get_point_count() const { return points.size(); }
    inline float kdtree_get_pt(const size_t idx, int dim) const {
        if (dim == 0) return points[idx].x;
        if (dim == 1) return points[idx].y;
        return points[idx].z;
    }
    template <class BBOX>
    bool kdtree_get_bbox(BBOX&) const { return false; }
    Point thetaPhiToPoint(int index, float theta, float phi) {
    float x = std::sin(theta) * std::cos(phi);
    float y = std::sin(theta) * std::sin(phi);
    float z = std::cos(theta);
    return {index, x, y, z};
    }
    void addPoint(Point p){
        points.emplace_back(p);
    }
    void addPoint(const int& index, const float& theta, const float& phi){
        points.emplace_back(thetaPhiToPoint(index, theta, phi));
    }
    void buildTree(){
        tree = new KDTree(3, *this, nanoflann::KDTreeSingleIndexAdaptorParams(10));
        tree->buildIndex();
    }
    int query(const float& theta, const float& phi){
        nanoflann::KNNResultSet<float> resultSet(1);
        resultSet.init(&ret_index, &out_dist_sqr);
        float query_pt[3] = {std::sin(theta) * std::cos(phi), std::sin(theta) * std::sin(phi), std::cos(theta)};
        tree->findNeighbors(resultSet, query_pt, nanoflann::SearchParameters(10));
        return points[ret_index].index;
    }
private:
    std::vector<Point> points;
    KDTree *tree;
    size_t ret_index;
    float out_dist_sqr;
};

#endif