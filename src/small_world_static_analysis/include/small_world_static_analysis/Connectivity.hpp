#pragma once

#include <cassert>
#include <limits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <small_world_static_analysis/Graph.hpp>

namespace small_world::static_analysis {

struct Components {
  std::unordered_map<size_t, size_t> vertToComp;
  std::vector<std::unordered_set<size_t>> compToVerts;
};

template<typename float_t>
inline Components components(const Graph<float_t>& g) {
  const std::unordered_map<size_t, std::unordered_map<size_t, float_t>>& adj = g.get_adj();

  std::unordered_map<size_t, size_t> vertToComp;
  std::vector<std::unordered_set<size_t>> compToVerts;
  
  std::unordered_set<size_t> curComp;
  std::vector<size_t> stack;
  for (const auto& [i, iAdj] : adj) {
    if (vertToComp.find(i) != vertToComp.end())
      continue;
    // Find all elements in the same component as i using DFS
    curComp.clear();
    curComp.insert(i);
    stack.push_back(i);
    while (!stack.empty()) {
      size_t v = stack.back();
      assert(adj.find(v) != adj.end());
      stack.pop_back();
      for (const auto& [w, weight] : adj.find(v)->second)
        if (curComp.insert(w).second)
          stack.push_back(w);
    }
    for (size_t v : curComp)
      vertToComp[v] = compToVerts.size();
    compToVerts.push_back(std::move(curComp));
  }

  return {std::move(vertToComp), std::move(compToVerts)};
}

}