#pragma once

#include <tuple>
#include <unordered_map>
#include <vector>

#include <small_world_io/CsvEnrollmentDataReader.hpp>
#include <small_world_io/Section.hpp>
#include <small_world_io/Student.hpp>

namespace small_world::static_analysis {

template<typename float_t>
struct GraphMetrics {
	size_t full_node_count;
	size_t full_edge_count;
	size_t comp_node_count;
	size_t comp_edge_count;
	float_t mean_degree;
};

template<typename float_t>
class Graph {
public:
	inline Graph() = default;

	inline GraphMetrics<float_t> compute_metrics() const;

	inline std::unordered_map<size_t, std::unordered_map<size_t, float_t>>& get_adj() { return adj; }
	inline const std::unordered_map<size_t, std::unordered_map<size_t, float_t>>& get_adj() const { return adj; }

private:
	// Adjacency lists
	std::unordered_map<size_t, std::unordered_map<size_t, float_t>> adj;
};

template<typename float_t>
GraphMetrics<float_t> Graph<float_t>::compute_metrics() const {
	return GraphMetrics<float_t>{ adj.size() };
}

}
