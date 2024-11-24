#ifndef CLUSTER_HPP
#define CLUSTER_HPP

#include "ActsFatras/Digitization/Channelizer.hpp"

#include <vector>

struct Cluster {
  using Cell = ActsFatras::Channelizer::ChannelSegment;
  size_t sizeLoc0 = 0;
  size_t sizeLoc1 = 0;
  std::vector<Cell> channels;
};

/// Clusters have a one-to-one relation with measurements
using ClusterContainer = std::vector<Cluster>;

#endif