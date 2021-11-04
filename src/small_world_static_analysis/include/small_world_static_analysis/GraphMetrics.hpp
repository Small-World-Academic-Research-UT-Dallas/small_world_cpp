#pragma once

#include <algorithm>
#include <unordered_map>
#include <vector>

#include <small_world_static_analysis/Biconnectivity.hpp>
#include <small_world_static_analysis/Connectivity.hpp>
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
  float_t average_weight = 0;
};

template<typename float_t>
GraphMetrics<float_t> compute_metrics(const Graph<float_t>& g) {
  size_t n = g.get_adj().size();

  GraphMetrics<float_t> metrics{};

  metrics.full_node_count = n;

  for (const auto& [u, uAdj] : g.get_adj())
    metrics.full_edge_count += uAdj.size();
  metrics.full_edge_count /= 2;
  
  const auto cmpSize = [&](const std::unordered_set<size_t>& l, const std::unordered_set<size_t>& r) {
    return l.size() < r.size();
  };

  struct SetFilter {
    const std::unordered_set<size_t>& S;
    inline bool operator()(size_t v) const noexcept {
      return S.contains(v);
    }
  };

  Components comps = components(g);
  size_t maxCompIdx = std::max_element(comps.compToVerts.begin(), comps.compToVerts.end(), cmpSize) - comps.compToVerts.begin();
  Graph c = g.subgraph(SetFilter{comps.compToVerts[maxCompIdx]});

  metrics.comp_node_count = c.get_adj().size();

  for (const auto& [u, uAdj] : c.get_adj())
    metrics.comp_edge_count += uAdj.size();
  metrics.comp_edge_count /= 2;

  Bicomponents bicomps = bicomponents(c);
  size_t maxBicompIdx = std::max_element(bicomps.bicompToVerts.begin(), bicomps.bicompToVerts.end(), cmpSize) - bicomps.bicompToVerts.begin();
  Graph bc = g.subgraph(SetFilter{bicomps.bicompToVerts[maxBicompIdx]});

  metrics.bcmp_node_count = bc.get_adj().size();

  for (const auto& [u, uAdj] : bc.get_adj())
    metrics.bcmp_edge_count += uAdj.size();
  metrics.bcmp_edge_count /= 2;

  metrics.average_degree = (float_t)2 * metrics.comp_edge_count / metrics.comp_node_count;

  metrics.c_node_percent = (float_t)100 * metrics.comp_node_count / metrics.full_node_count;

  for (const auto& [u, uAdj] : c.get_adj())
    for (const auto& [v, weight] : uAdj)
      metrics.average_weight += weight;
  metrics.average_weight /= 2 * metrics.comp_edge_count;

  return metrics;
}

}
