#include <exception>
#include <fstream>
#include <iostream>
#include <vector>

#include <small_world_io/CsvEnrollmentDataReader.hpp>
#include <small_world_io/Student.hpp>
#include <small_world_io/Section.hpp>

#include <small_world_static_analysis/Graph.hpp>
#include <small_world_static_analysis/GraphMetrics.hpp>
#include <small_world_static_analysis/IO.hpp>

int main() {
  using namespace small_world;

  std::ios_base::sync_with_stdio(false);
  std::cin.tie(nullptr);

  io::CsvEnrollmentDataReader csvReader(std::cin);
  const io::EnrollmentDataReader& data = csvReader;
  //std::cout << data.get_students()->size() << std::endl;
  //std::cout << data.get_sections()->size() << std::endl;
  //std::cout << std::endl;
  //std::unordered_map<std::size_t, std::size_t> studentCts, sectionCts;
  //for (const io::Student& student : *data.get_students())
  //  studentCts[student.get_strm()]++;
  //for (const io::Section& section : *data.get_sections())
  //  sectionCts[section.get_strm()]++;
  //for (std::pair<std::size_t, std::size_t> p : studentCts)
  //  std::cout << p.first << ' ' << p.second << std::endl;
  //std::cout << std::endl;
  //for (std::pair<std::size_t, std::size_t> p : sectionCts)
  //  std::cout << p.first << ' ' << p.second << std::endl;

  const std::unordered_set<io::Section::Component> forbiddenComps{
    io::Section::Component::EXM,
    io::Section::Component::INT,
    io::Section::Component::RXM
  };

  const auto stFilter = [&](const io::Student& student) -> bool {
    return student.get_strm() == 2198;
  };

  const auto seFilter = [&](const io::Section& section) -> bool {
    return section.get_strm() == 2198 &&
           forbiddenComps.find(section.get_component()) == forbiddenComps.end() &&
           section.get_mode() == io::Section::Mode::FACE_TO_FACE;
  };

  static_analysis::Graph<double> g = static_analysis::student_to_student_weighted<double, decltype(stFilter), decltype(seFilter)>(data, stFilter, seFilter);

  static_analysis::GraphMetrics<double> metrics = compute_metrics(g);
  std::cout << "full_node_count: " << metrics.full_node_count << std::endl;
  std::cout << "full_edge_count: " << metrics.full_edge_count << std::endl;
  std::cout << "comp_node_count: " << metrics.comp_node_count << std::endl;
  std::cout << "comp_edge_count: " << metrics.comp_edge_count << std::endl;
  std::cout << "bcmp_node_count: " << metrics.bcmp_node_count << std::endl;
  std::cout << "bcmp_edge_count: " << metrics.bcmp_edge_count << std::endl;
  std::cout << "average_degree: " << metrics.average_degree << std::endl;
  std::cout << "c_node_percent: " << metrics.c_node_percent << std::endl;
  std::cout << "average_weight: " << metrics.average_weight << std::endl;
}
