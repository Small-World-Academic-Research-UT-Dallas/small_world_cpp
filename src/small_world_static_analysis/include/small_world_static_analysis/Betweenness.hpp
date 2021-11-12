#pragma once

#include <cassert>
#include <iostream>
#include <iomanip>
#include <limits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <small_world_static_analysis/Compression.hpp>
#include <small_world_static_analysis/Multithreading.hpp>

namespace small_world::static_analysis {

template<typename float_t>
std::vector<float_t> brandes_weighted(const CGraph<float_t>& cg, const std::vector<size_t> mult) {
  constexpr bool PROGRESS = true;

  size_t n = cg.size();
  std::vector<float_t> ans(n, 0);
  std::mutex mutex;

  size_t m = 0;
  // Paths between structurally equivalent vertices
  for (size_t i = 0; i < n; i++) {
    assert(mult[i]);
    if (mult[i] == 1)
      continue;
    m += cg.get_adj()[i].size();
    size_t m = mult[i] * (mult[i] - 1);
    size_t d = 0;
    float_t minwt = std::numeric_limits<float_t>::infinity();
    for (const auto& [j, wt] : cg.get_adj()[i]) {
      if (wt < minwt) {
        d = mult[j];
        minwt = wt;
      }
      else if (wt == minwt)
        d += mult[j];
    }
    float_t delta = (float_t)(mult[i] * (mult[i] - 1)) / d;
    for (const auto& [j, wt] : cg.jet_adj()[i])
      if (wt == minwt)
        ans[j] += delta;
  }
  m /= 2;
  if (PROGRESS)
    std::cout << "Paths between structurally equivalent vertices done" << std::endl;

  std::vector<std::vector<size_t>> stacks(THREAD_CT);
  for (std::vector<size_t> stack : stacks)
    stack.reserve(n);
  
  std::vector<std::vector<size_t>> queues;
  for (std::vector<size_t> queue : queues)
    queue.reserve(n);
  
  std::vector<std::vector<std::vector<size_t>>> prevs(THREAD_CT, std::vector<size_t>(n));
  for (size_t i = 0; i < THREAD_CT; i++)
    for (size_t j = 0; j < n; j++)
      prevs[i][j].reserve(cg.get_adj()[j].size());
  
  std::vector<std::vector<size_t>> pathCts(THREAD_CT, std::vector<float_t>(n, 0));
  std::vector<std::vector<float_t>> dists(THREAD_CT, std::vector<float_t>(n, 0));
  std::vector<std::vector<float_t>> deps(THREAD_CT, std::vector<float_t>(n, 0));

  // Other paths
  const auto perThread = [&, n, m](size_t i, size_t t) {
    if (PROGRESS && i % 20 == 0) {
      mutex.lock();
      std::cout << "\rBrandes: " << std::setw(5) << i << '/' << n << std::flush;
      mutex.unlock();
    }

    std::vector<size_t>& stack = stacks[t];
    std::vector<size_t>& queue = queues[t];
    std::vector<std::vector<size_t>>& prev = prevs[t];
    std::vector<size_t>& pathCt = pathCts[t];
    std::vector<float_t>& dist = dists[t];
    std::vector<float_t>& dep = deps[t];

    assert(stack.empty());
    assert(queue.empty());
    for (std::vector<size_t>& p : prev)
      p.clear();
    std::fill(pathCt.begin(), pathCt.end(), 0);
    std::fill(dist.begin(), dist.end(), 0);
    std::fill(dep.begin(), dep.end(), 0);
    pathCt[s] = mult[s];
    pathCt[s]
  }

  if (PROGRESS)
    std::cout << "\rPaths between structurally inequivalent vertices done" << std::endl;
}

// TODO add block-cut tree decomposition optimization
template<typename float_t>
std::unoredred_map<size_t, float_t> betweenness_weighted(const Graph<float_t>& g) {
  std::unordered_map<size_t, std::unordered_set<size_t>> sToOriginal;
  std::unordered_map<size_t, size_t> sToCompressed;
  std::vector<size_t>& iToOriginal;
  std::unordered_map<size_t, size_t>& iToCompressed;
  Graph<float_t> cg = index_compress(structural_compress(g, sToOriginal, sToCompressed), iToOriginal, iToCompressed);
  std::vector<size_t> mult(cg.size());
  for (size_t i = 0; i < cg.size(); i++)
    mult[i] = sToOriginal[iToOriginal[i]].size();
  std::vector<size_t> brandes = brandes_weighted(cg, mult);
  std::unordered_map<size_t, float_t> ans;
  for (const auto& [v, vAdj] : g.get_adj())
    ans[v] = brandes[iToCompressed[sToCompressed[v]]];
  return ans;
}

}
