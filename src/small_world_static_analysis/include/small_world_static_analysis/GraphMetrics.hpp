#pragma once

#include <algorithm>
#include <unordered_map>
#include <vector>

#include <small_world_static_analysis/Biconnectivity.hpp>
#include <small_world_static_analysis/Compression.hpp>
#include <small_world_static_analysis/Connectivity.hpp>
#include <small_world_static_analysis/Diameter.hpp>
#include <small_world_static_analysis/Filters.hpp>
#include <small_world_static_analysis/Graph.hpp>

namespace small_world::static_analysis {

template<typename float_t>
struct GraphMetrics {
  size_t full_node_count = 0;
  size_t full_edge_count = 0;
  size_t comp_node_count = 0;
  size_t comp_edge_count = 0;
  size_t bcmp_node_count = 0;
  size_t bcmp_edge_count = 0;
  float_t average_degree = 0;
  float_t c_node_percent = 0;
  float_t avg_inv_weight = 0;
  size_t comp_diam_unwtd = 0;
};

template<typename float_t>
GraphMetrics<float_t> compute_metrics(const Graph<float_t>& g) {
  size_t n = g.get_adj().size();

  GraphMetrics<float_t> metrics;

  metrics.full_node_count = n;
  std::cout << "full_node_count computed" << std::endl;

  for (const auto& [u, uAdj] : g.get_adj())
    metrics.full_edge_count += uAdj.size();
  metrics.full_edge_count /= 2;
  std::cout << "full_edge_count computed" << std::endl;
  
  const auto cmpSize = [&](const std::unordered_set<size_t>& l, const std::unordered_set<size_t>& r) {
    return l.size() < r.size();
  };

  Components<float_t> comps = components(g);
  size_t maxCompIdx = std::max_element(comps.compToVerts.begin(), comps.compToVerts.end(), cmpSize) - comps.compToVerts.begin();
  const Graph<float_t>& c = comps.comps[maxCompIdx];

  metrics.comp_node_count = c.get_adj().size();
  std::cout << "comp_node_count computed" << std::endl;

  for (const auto& [u, uAdj] : c.get_adj())
    metrics.comp_edge_count += uAdj.size();
  metrics.comp_edge_count /= 2;
  std::cout << "comp_edge_count computed" << std::endl;

  Bicomponents<float_t> bicomps = bicomponents(c);
  size_t maxBicompIdx = std::max_element(bicomps.bicompToVerts.begin(), bicomps.bicompToVerts.end(), cmpSize) - bicomps.bicompToVerts.begin();
  const Graph<float_t>& bc = bicomps.bicomps[maxBicompIdx];

  metrics.bcmp_node_count = bc.get_adj().size();
  std::cout << "bcmp_node_count computed" << std::endl;

  for (const auto& [u, uAdj] : bc.get_adj())
    metrics.bcmp_edge_count += uAdj.size();
  metrics.bcmp_edge_count /= 2;
  std::cout << "bcmp_edge_count computed" << std::endl;

  metrics.average_degree = (float_t)2 * metrics.comp_edge_count / metrics.comp_node_count;
  std::cout << "average_degree computed" << std::endl;

  metrics.c_node_percent = (float_t)100 * metrics.comp_node_count / metrics.full_node_count;
  std::cout << "c_node_percent computed" << std::endl;

  for (const auto& [u, uAdj] : c.get_adj())
    for (const auto& [v, weight] : uAdj)
      metrics.avg_inv_weight += 1 / weight;
  metrics.avg_inv_weight /= 2 * metrics.comp_edge_count;
  std::cout << "avg_inv_weight computed" << std::endl;

  std::vector<size_t> toOriginal;
  std::unordered_map<size_t, size_t> toCompressed;
  CGraph<float_t> compressed = index_compress(c, toOriginal, toCompressed);
  std::cout << "Compression done" << std::endl;

  metrics.comp_diam_unwtd = diameter_unweighted(compressed);
  std::cout << "comp_diam_unwtd computed" << std::endl;

  return metrics;
}

}
