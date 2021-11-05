#pragma once

#include <algorithm>
#include <cassert>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <small_world_static_analysis/Graph.hpp>

namespace small_world::static_analysis {

template<typename float_t>
struct Bicomponents {
  std::unordered_map<size_t, std::unordered_set<size_t>> vertToBicomps;
  std::vector<std::unordered_set<size_t>> bicompToVerts;
  std::vector<Graph<float_t>> bicomps;
};

// Tarjan's Biconnected Component Algorithm
template<typename float_t>
inline Bicomponents<float_t> bicomponents(const Graph<float_t>& g) {
  const std::unordered_map<size_t, std::unordered_map<size_t, float_t>>& adj = g.get_adj();

  struct Helper {
    const Graph<float_t>& g;

    std::unordered_map<size_t, std::unordered_set<size_t>> vertToBicomps;
    std::vector<std::unordered_set<size_t>> bicompToVerts;
    std::vector<Graph<float_t>> bicomps;

    std::vector<std::pair<size_t, size_t>> edges;
    std::unordered_map<size_t, size_t> num, lowpt;
    size_t i = 0;

    inline Helper(const Graph<float_t>& g) :
      g(g)
    {
      num.reserve(g.size());
      lowpt.reserve(g.size());
    }
    
    void biconnect(size_t u, size_t v) {
      lowpt[v] = num[v] = i++;
      for (auto [w, wt] : g.get_adj().find(v)->second) {
        auto wIt = num.find(w);
        if (wIt == num.end()) {
          edges.emplace_back(v, w);
          biconnect(v, w);
          lowpt[v] = std::min(lowpt[v], lowpt[w]);
          if (lowpt[w] >= num[v]) {
            std::unordered_set<size_t> bicomp;
            size_t u1, u2;
            while (std::tie(u1, u2) = edges.back(), num[u1] > num[v]) {
              edges.pop_back();
              bicomp.insert(u2);
              vertToBicomps[u2].insert(bicompToVerts.size());
            }
            edges.pop_back();
            bicomp.insert(v);
            bicomp.insert(w);
            vertToBicomps[v].insert(bicompToVerts.size());
            vertToBicomps[w].insert(bicompToVerts.size());
            bicomps.push_back(g.subgraph(bicomp));
            bicompToVerts.push_back(std::move(bicomp));
          }
        }
        else if (w != u && wIt->second < num[v]) {
          edges.emplace_back(v, w);
          lowpt[v] = std::min(lowpt[v], num[w]);
        }
      }
    }
  } helper(g);

  for (const auto& [v, vAdj] : adj)
    if (vAdj.empty()) {
      helper.vertToBicomps[v].insert(helper.bicompToVerts.size());
      helper.bicompToVerts.push_back({v});
    }
    else if (helper.num.find(v) == helper.num.end())
      helper.biconnect(0, v);

  return Bicomponents<float_t>{ std::move(helper.vertToBicomps), std::move(helper.bicompToVerts), std::move(helper.bicomps) };
}

}