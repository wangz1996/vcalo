#ifndef POINTCLOUD_HH
#define POINTCLOUD_HH
#include <vector>
#include "KDTree/nanoflann.hh"

struct PointCloud {
    std::vector<std::vector<float>> points;

    // 必须实现的函数：返回点的数量
    inline size_t kdtree_get_point_count() const { return points.size(); }

    // 必须实现的函数：返回两个点之间的距离平方
    inline float kdtree_distance(const float* p1, const size_t idx_p2, size_t) const {
        const std::vector<float>& p2 = points[idx_p2];
        float dist = 0.0;
        for (size_t i = 0; i < p2.size(); i++) {
            dist += (p1[i] - p2[i]) * (p1[i] - p2[i]);
        }
        return dist;
    }

    // 必须实现的函数：返回点的第 dim 个维度值
    inline float kdtree_get_pt(const size_t idx, int dim) const {
        return points[idx][dim];
    }

    // 必须实现的函数：是否提供 AABB 包围盒（一般返回 false）
    template <class BBOX>
    bool kdtree_get_bbox(BBOX&) const { return false; }
};

using KDTree = nanoflann::KDTreeSingleIndexAdaptor<
        nanoflann::L2_Simple_Adaptor<float, PointCloud>,
        PointCloud, 2 /* 维度 */
    >;


#endif