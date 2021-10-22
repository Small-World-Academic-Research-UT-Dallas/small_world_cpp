#include <exception>
#include <fstream>
#include <iostream>
#include <vector>

#include <small_world_io/CsvEnrollmentDataReader.hpp>
#include <small_world_io/Student.hpp>
#include <small_world_io/Section.hpp>

#include <small_world_static_analysis/Graph.hpp>
#include <small_world_static_analysis/IO.hpp>

int main() {
  try {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    small_world::io::CsvEnrollmentDataReader reader(std::cin);
    small_world::static_analysis::Graph<double> graph = small_world::static_analysis::student_to_student_weighted<double>(reader);
    std::cout << "Graph constructed" << std::endl;
    small_world::static_analysis::GraphMetrics<double> metrics = graph.compute_metrics();
    std::cout << metrics.full_node_count << std::endl;
    return 0;
  }
  catch (std::exception& e) {
    std::cerr << "ERROR" << std::endl << e.what() << std::endl;
    return -1;
  }
}
