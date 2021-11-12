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

template<typename float_t>
inline Graph<float_t> structural_compress(const Graph<float_t>& g, std::unordered_map<size_t, std::unordered_set<size_t>>& toOriginal, std::unordered_map<size_t, size_t>& toCompressed) {
  struct Hasher {
      const std::hash<size_t> idxHash;
      const std::hash<float_t> wtHash;
    inline size_t operator()(const std::unordered_map<size_t, float_t> map) const noexcept {
      std::vector<std::pair<size_t, float_t>> vec(map.begin(), map.end());
      std::sort(vec.begin(), vec.end());
      // The adjacency list is now in a unique order and has unique elements, so hashes are consistent.
      size_t hash = 0;
      for (const auto& [idx, wt] : vec)
        hash = 31 * (31 * hash + idxHash(idx)) + wtHash(wt);
      return hash;
    }
  };

  std::unordered_map<std::unordered_map<size_t, float_t>, size_t, Hasher> adjToClass;

  toOriginal.clear();
  toCompressed.clear();

  for (const auto& [v, vAdj] : g.get_adj()) {
    // Add a new equivalence class represented by v iff vAdj is new.
    auto ins = adjToClass.try_emplace(vAdj, v);
    // Equivalence class representative
    size_t u = ins.first->second;
    toOriginal[u].insert(v);
    toCompressed[v] = u;
  }

  // Return quotient graph containing only representative vertices
  return g.subgraph([&](size_t v) { return toCompressed[v] == v; });
}

}
