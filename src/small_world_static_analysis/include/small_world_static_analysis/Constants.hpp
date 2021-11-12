#pragma once

#include <thread>

namespace small_world::static_analysis {

//const unsigned int THREAD_CT = std::min(64u, std::thread::hardware_concurrency() == 0 ? 12u : std::thread::hardware_concurrency());
const unsigned int THREAD_CT = std::thread::hardware_concurrency();

}