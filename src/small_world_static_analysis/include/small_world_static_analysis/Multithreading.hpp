#pragma once

#include <atomic>
#include <thread>
#include <vector>

namespace small_world::static_analysis {

const size_t THREAD_CT = std::max(1u, std::thread::hardware_concurrency());

template<typename Op>
inline void parallel_atomic_loop(size_t lim, const Op& op = Op{}, size_t threadCt = 0) {
  if (threadCt == 0)
    threadCt = THREAD_CT;

  std::atomic_size_t index = 0;

  struct ThreadOp {
    size_t lim;
    const Op& op;
    std::atomic_size_t& index;
    size_t thread;
    inline ThreadOp(size_t lim, const Op& op, std::atomic_size_t& index, size_t thread) :
      lim(lim),
      op(op),
      index(index),
      thread(thread)
    {}
    inline void operator()() const noexcept {
      for (size_t i = index++; i < lim; i = index++)
        op(i, thread);
    }
  };

  std::vector<std::thread> threads(threadCt);
  for (size_t i = 0; i < threadCt; i++)
    threads[i] = std::thread(ThreadOp(lim, op, index, i));
  
  for (std::thread& thread : threads)
    thread.join();
}

}