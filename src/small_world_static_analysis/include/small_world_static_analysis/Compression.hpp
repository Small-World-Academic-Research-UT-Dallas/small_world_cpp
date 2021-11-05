#pragma once

#include <algorithm>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <small_world_static_analysis/Graph.hpp>

namespace small_world::static_analysis {

template<typename float_t>
class CGraph {
public:
  inline CGraph() = default;
  inline CGraph(size_t n) :
    adj(n)
  {}

  inline std::vector<std::vector<std::pair<size_t, float_t>>>& get_adj() { return adj; }
  inline const std::vector<std::vector<std::pair<size_t, float_t>>>& get_adj() const { return adj; }

  inline size_t size() const { return adj.size(); }

  inline explicit operator Graph<float_t>() const;

private:
  // Adjacency lists
  std::vector<std::vector<std::pair<size_t, float_t>>> adj;
};

// Converts a Graph into a CGraph, which has contiguous indices and is represented with vectors instead of hash tables
template<typename float_t>
inline CGraph<float_t> index_compress(const Graph<float_t>& g, std::vector<size_t>& toOriginal, std::unordered_map<size_t, size_t>& toCompressed) {
  toOriginal.clear();
  toCompressed.clear();
  toOriginal.reserve(g.size());
  for (const auto& [u, uAdj] : g.get_adj()) {
    toCompressed[u] = toOriginal.size();
    toOriginal.push_back(u);
  }

  CGraph<float_t> cg(g.size());
  for (size_t i = 0; i < g.size(); i++) {
    size_t u = toOriginal[i];
    const auto& uAdj = g.get_adj().find(u)->second;
    auto& iAdj = cg.get_adj()[i];
    iAdj.reserve(uAdj.size());
    for (const auto& [v, wt] : uAdj)
      iAdj.emplace_back(toCompressed[v], wt);
    std::sort(iAdj.begin(), iAdj.end());
  }

  return cg;
}

}
