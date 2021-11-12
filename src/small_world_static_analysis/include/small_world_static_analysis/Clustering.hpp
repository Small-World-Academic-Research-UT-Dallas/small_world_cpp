#pragma once

#include <thread>

#include <small_world_static_analysis/Compression.hpp>
#include <small_world_static_analysis/Multithreading.hpp>

namespace small_world::static_analysis {

  // Counts triangles containing each vertex (summing triple-counts the triangles in the graph).
  template<typename float_t>
  inline std::vector<size_t> triangles(const CGraph<float_t>& cg) {
    size_t n = cg.size();
    std::vector<size_t> ans(n, 0);
    std::vector<std::vector<uint8_t>> iSets(THREAD_CT, std::vector<uint8_t>(n, false));

    const auto perThread = [&, n](size_t i, size_t t) {
      std::vector<uint8_t>& iSet = iSets[t];
      const std::vector<std::pair<size_t, float_t>>& iAdj = cg.get_adj()[i];
      for (const auto& [j, wt] : iAdj)
        iSet[j] = true;
      for (const auto& [j, wt] : iAdj)
        for (const auto& [k, wt2] : cg.get_adj()[j])
          if (j < k && iSet[k])
            ans[i]++;
      for (const auto& [j, wt] : iAdj)
        iSet[j] = false;
    };

    parallel_atomic_loop(n, perThread);

    return ans;
  }

  // Number of triads centered at each vertex. Note that triangles are triple-counted, as in the formula for the global clustering coefficient.
  template<typename float_t>
  inline std::vector<size_t> triads(const CGraph<float_t>& cg) {
    std::vector<size_t> ans(cg.size());
    for (size_t i = 0; i < cg.size(); i++) {
      size_t d = cg.get_adj()[i].size();
      ans[i] = d * (d - 1) / 2;
    }
    return ans;
  }

  template<typename float_t>
  struct ClusteringData {
    float_t local = 0, global = 0;
    inline ClusteringData() = default;
    inline ClusteringData(const CGraph<float_t>& cg) {
      size_t n = cg.size();
      std::vector<size_t> triangleCts = triangles(cg), triadCts = triads(cg);
      for (size_t i = 0; i < n; i++) {
        size_t d = cg.get_adj()[i].size();
        if (d > 1)
          local += (float_t)triangleCts[i] / (d * (d - 1) / 2);
      }
      local /= n;
      global = (float_t)(std::accumulate(triangleCts.begin(), triangleCts.end(), 0ull)) / std::accumulate(triadCts.begin(), triadCts.end(), 0ull);
    }
  };

}