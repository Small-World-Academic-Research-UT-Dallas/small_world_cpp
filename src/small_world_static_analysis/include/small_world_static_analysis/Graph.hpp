#pragma once

#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <small_world_io/CsvEnrollmentDataReader.hpp>
#include <small_world_io/Section.hpp>
#include <small_world_io/Student.hpp>

namespace small_world::static_analysis {

template<typename float_t>
class Graph {
public:
  inline Graph() = default;

  inline std::unordered_map<size_t, std::unordered_map<size_t, float_t>>& get_adj() { return adj; }
  inline const std::unordered_map<size_t, std::unordered_map<size_t, float_t>>& get_adj() const { return adj; }

  inline size_t size() const { return adj.size(); }

  inline Graph subgraph(const std::unordered_set<size_t>& verts) const;
  template<typename F>
  inline Graph subgraph(const F& filter = F{}) const;

private:
  // Adjacency lists
  std::unordered_map<size_t, std::unordered_map<size_t, float_t>> adj;
};

template<typename float_t>
inline Graph<float_t> Graph<float_t>::subgraph(const std::unordered_set<size_t>& verts) const {
  Graph g;
  // Add unfiltered vertices to the new graph
  if (verts.size() < size())
    for (size_t v : verts) {
      if (adj.find(v) != adj.end())
        g.adj[v];
    }
  else
    for (const auto& [v, vAdj] : adj) {
      if (verts.find(v) != verts.end())
        g.adj[v];
    }
  // Add unfiltered edges to the new graph
  for (auto& [u, uAdj] : g.adj)
    for (const auto& [v, wt] : adj.find(u)->second)
      if (g.adj.find(v) != g.adj.end())
        uAdj[v] = wt;
  return g;
}

template<typename float_t>
template<typename F>
inline Graph<float_t> Graph<float_t>::subgraph(const F& filter) const {
  Graph g;
  // Add unfiltered vertices to the new graph
  for (const auto& [u, uAdj] : adj)
    if (filter(u))
      g.adj[u];
  // Add edges connecting unfiltered vertices
  for (auto& [u, uAdj] : g.adj)
    for (const auto& [v, wt] : adj.find(u)->second)
      if (g.adj.find(v) != g.adj.end())
        uAdj[v] = wt;
  return g;
}

}
