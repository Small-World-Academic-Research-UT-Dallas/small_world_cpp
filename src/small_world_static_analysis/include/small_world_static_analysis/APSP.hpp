#pragma once

#include <algorithm>
#include <atomic>
#include <iomanip>
#include <iostream>
#include <limits>
#include <mutex>
#include <numeric>
#include <queue>
#include <thread>

#include <small_world_static_analysis/Compression.hpp>
#include <small_world_static_analysis/Graph.hpp>
#include <small_world_static_analysis/Multithreading.hpp>

namespace small_world::static_analysis {

template<typename dist_t>
struct APSP {
  template<typename T>
  static constexpr T NIL = std::numeric_limits<T>::max();
  std::vector<std::vector<dist_t>> dist;
  std::vector<std::vector<size_t>> prev;

  inline APSP(size_t n) :
    dist(n, std::vector<dist_t>(n, NIL<dist_t>)),
    prev(n, std::vector<size_t>(n, NIL<size_t>))
  {}

  inline size_t size() {
    return dist.size();
  }

  // Op should be commutative and associative for consistent behavior.
  template<typename Op>
  inline dist_t accumulate_dist(dist_t id, const Op& op = Op{}) {
    size_t n = size();
    std::vector<dist_t> tAns(THREAD_CT, id);

    const auto loop = [&, n](size_t i, size_t t) {
      tAns[t] = std::accumulate(dist[i].begin(), dist[i].end(), tAns[t], op);
    };
    parallel_atomic_loop(n, loop);

    return std::accumulate(tAns.begin(), tAns.end(), id, op);
  }

  inline dist_t diameter() {
    return accumulate_dist(std::numeric_limits<dist_t>::min(), [](const auto& l, const auto& r) { return std::max(l, r); });
  }

  inline dist_t total_dist() {
    return accumulate_dist(0, std::plus());
  }
};

template<typename float_t>
inline APSP<size_t> apsp_unweighted(const CGraph<float_t> cg) {
  APSP<size_t> apsp(cg.size());
  std::mutex mutex;

  const auto sssp = [&](size_t i, size_t) {
    // TODO use circular queue
    std::queue<size_t> queue;
    if (i % 20 == 0) {
      mutex.lock();
      std::cout << "\rAPSP: node " << std::setw(5) << i << '/' << std::setw(5) << cg.size() << std::flush;
      mutex.unlock();
    }
    std::vector<size_t>& d = apsp.dist[i];
    std::vector<size_t>& p = apsp.prev[i];
    d[i] = 0;
    p[i] = i;
    queue.push(i);
    while (!queue.empty()) {
      size_t v = queue.front();
      queue.pop();
      size_t dw = d[v] + 1;
      for (const auto& [w, wt] : cg.get_adj()[v])
        if (p[w] == APSP<size_t>::NIL<size_t>) {
          d[w] = dw;
          p[w] = v;
          queue.push(w);
        }
    }
  };

  parallel_atomic_loop(cg.size(), sssp);

  // std::vector<std::thread> threads;
  // threads.reserve(THREAD_CT);
  // for (size_t i = 0; i < THREAD_CT; i++)
  //   threads.emplace_back(ThreadOp(cg, apsp, index, mutex));
  // for (std::thread& thread : threads)
  //   thread.join();
  std::cout << '\r' << std::string(22, ' ') << '\r' << std::flush;
  return apsp;
}

}