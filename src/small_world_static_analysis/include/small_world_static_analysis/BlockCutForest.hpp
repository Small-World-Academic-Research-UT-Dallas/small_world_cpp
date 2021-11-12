#pragma once

#include <vector>

#include <small_world_static_analysis/Biconnectivity.hpp>
#include <small_world_static_analysis/Connectivity.hpp>
#include <small_world_static_analysis/Compression.hpp>
#include <small_world_static_analysis/Graph.hpp>

namespace small_world::static_analysis {

template<typename float_t>
class BlockCutForest {
  Components<float_t> comps;
  Bicomponents<float_t> bicomps;

  inline BlockCutForest(const Graph<float_t>& G) {
    
  }
};

}