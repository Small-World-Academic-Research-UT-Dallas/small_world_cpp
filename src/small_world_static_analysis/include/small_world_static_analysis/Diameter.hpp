#pragma once

#include <algorithm>
#include <atomic>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

#include <small_world_static_analysis/Compression.hpp>
#include <small_world_static_analysis/Constants.hpp>
#include <small_world_static_analysis/Graph.hpp>

namespace small_world::static_analysis {

template<typename float_t>
inline size_t diameter_unweighted(const CGraph<float_t> cg) {
  struct ThreadOp {
    std::atomic_size_t& index;
    size_t& diam;
    std::mutex& mutex;
    const CGraph<float_t>& cg;

    inline ThreadOp(std::atomic_size_t& index, size_t& diam, std::mutex& mutex, const CGraph<float_t>& cg) :
      index(index),
      diam(diam),
      mutex(mutex),
      cg(cg)
    {}

    inline void operator()() {
      size_t i;
      size_t ans = 0;
      std::queue<size_t> que;
      std::vector<size_t> d(cg.size());
      while ((i = index++) < cg.size()) {
        if (i % 10 == 0)
        {
          mutex.lock();
          std::cout << "\rDiameter: node" << std::setw(6) << i << '/' << std::setw(5) << cg.size() << std::flush;
          mutex.unlock();
        }
        std::fill(d.begin(), d.end(), 0);
        d[i] = 1;
        que.push(i);
        while (!que.empty()) {
          size_t v = que.front();
          que.pop();
          size_t dw = d[v] + 1;
          for (const auto& [w, wt] : cg.get_adj()[v])
            if (!d[w]) {
              d[w] = dw;
              que.push(w);
            }
        }
        ans = std::max(ans, *std::max_element(d.begin(), d.end()));
      }
      ans--;
      mutex.lock();
      diam = std::max(ans, diam);
      mutex.unlock();
    }
  };

  std::atomic_size_t index = 0;
  size_t diam = 0;
  std::mutex mutex;

  std::vector<std::thread> threads;
  threads.reserve(THREAD_CT);
  for (size_t i = 0; i < THREAD_CT; i++)
    threads.emplace_back(ThreadOp(index, diam, mutex, cg));
  for (std::thread& thread : threads)
    thread.join();
  std::cout << '\r' << std::string(26, ' ') << '\r' << std::flush;
  return diam;
}

}