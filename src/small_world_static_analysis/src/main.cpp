#include <exception>
#include <fstream>
#include <iostream>
#include <vector>

#include <small_world_io/CsvEnrollmentDataReader.hpp>
#include <small_world_io/Student.hpp>
#include <small_world_io/Section.hpp>

//#include <small_world_static_analysis/Graph.hpp>
//#include <small_world_static_analysis/IO.hpp>

int main() {
  try {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    small_world::io::CsvEnrollmentDataReader reader(std::cin);
    std::cout << reader.get_students()->size() << std::endl;
    std::cout << reader.get_sections()->size() << std::endl;
    std::cout << std::endl;
    std::unordered_map<std::size_t, std::size_t> studentCts, sectionCts;
    for (const small_world::io::Student& student : *reader.get_students())
      studentCts[student.get_strm()]++;
    for (const small_world::io::Section& section : *reader.get_sections())
      sectionCts[section.get_strm()]++;
    for (std::pair<std::size_t, std::size_t> p : studentCts)
      std::cout << p.first << ' ' << p.second << std::endl;
    std::cout << std::endl;
    for (std::pair<std::size_t, std::size_t> p : sectionCts)
      std::cout << p.first << ' ' << p.second << std::endl;

    //small_world::static_analysis::Graph<double> graph = small_world::static_analysis::student_to_student_weighted<double>(reader);
    //std::cout << "Graph constructed" << std::endl;
    //small_world::static_analysis::GraphMetrics<double> metrics = graph.compute_metrics();
    //std::cout << metrics.full_node_count << std::endl;
    return 0;
  }
  catch (std::exception& e) {
    std::cerr << "ERROR" << std::endl << e.what() << std::endl;
    return -1;
  }
}
