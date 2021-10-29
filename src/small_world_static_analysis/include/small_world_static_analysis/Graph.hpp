#pragma once

#include <tuple>
#include <unordered_map>
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

  template<typename F>
  inline Graph subgraph(const F& filter = F{}) const;

private:
  // Adjacency lists
  std::unordered_map<size_t, std::unordered_map<size_t, float_t>> adj;
};

template<typename float_t>
template<typename F>
inline Graph<float_t> Graph<float_t>::subgraph(const F& filter) const {
  Graph g;
  // Add unfiltered vertices to the new graph
  for (const auto& [u, uAdj] : adj)
    if (filter(u))
      g.adj[u];
  // Add edges connecting unfiltered vertices
  for (const auto& [u, uAdj] : adj)
    if (g.adj.find(u) != g.adj.end())
      for (const auto& [v, weight] : uAdj)
        if (g.adj.find(v) != g.adj.end())
          g.adj[u][v] = weight;
  return g;
}

}
