#pragma once

#include <algorithm>
#include <unordered_map>
#include <vector>

#include <small_world_static_analysis/Connectivity.hpp>
#include <small_world_static_analysis/Graph.hpp>

namespace small_world::static_analysis {

template<typename float_t>
struct GraphMetrics {
  size_t full_node_count = 0;
  size_t full_edge_count = 0;
  size_t comp_node_count = 0;
  size_t comp_edge_count = 0;
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


  Components comps = components(g);
  size_t maxCompIdx = std::max_element(comps.compToVerts.begin(), comps.compToVerts.end(), cmpSize) - comps.compToVerts.begin();
  Graph c = g.subgraph([&](size_t v) { return comps.vertToComp[v] == maxCompIdx; });

  metrics.comp_node_count = c.get_adj().size();

  for (const auto& [u, uAdj] : c.get_adj())
    metrics.comp_edge_count += uAdj.size();
  metrics.comp_edge_count /= 2;

  metrics.average_degree = (float_t)2 * metrics.comp_edge_count / metrics.comp_node_count;

  metrics.c_node_percent = (float_t)100 * metrics.comp_node_count / metrics.full_node_count;

  for (const auto& [u, uAdj] : c.get_adj())
    for (const auto& [v, weight] : uAdj)
      metrics.average_weight += weight;
  metrics.average_weight /= 2 * metrics.comp_edge_count;

  return metrics;
}

}
