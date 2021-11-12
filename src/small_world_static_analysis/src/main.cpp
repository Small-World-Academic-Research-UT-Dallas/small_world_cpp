#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

#include <small_world_io/CsvEnrollmentDataReader.hpp>
#include <small_world_io/Student.hpp>
#include <small_world_io/Section.hpp>

#include <small_world_static_analysis/Filters.hpp>
#include <small_world_static_analysis/Graph.hpp>
#include <small_world_static_analysis/GraphMetrics.hpp>
#include <small_world_static_analysis/IO.hpp>
#include <small_world_static_analysis/Timer.hpp>

int main() {

  using namespace small_world;

  // static_analysis::Graph<double> triangle;
  // for (size_t i = 0; i < 3; i++)
  //   for (size_t j = 0; j < 3; j++)
  //     if (i != j)
  //       triangle.get_adj()[i].emplace(j, 1);
  // std::vector<size_t> toO;
  // std::unordered_map<size_t, size_t> toC;
  // static_analysis::CGraph<double> ctri = static_analysis::index_compress(triangle, toO, toC);
  // std::cout << "triangle: " << std::endl;
  // for (size_t t : static_analysis::triangles(ctri))
  //   std::cout << t << ' ';
  // std::cout << std::endl;
  // for (size_t t : static_analysis::triads(ctri))
  //   std::cout << t << ' ';
  // std::cout << std::endl;
  // static_analysis::ClusteringData cDat(ctri);
  // std::cout << cDat.local << ' ' << cDat.global << std::endl;

  static_analysis::Timer timer;

  std::ios_base::sync_with_stdio(false);
  std::cin.tie(nullptr);

  io::CsvEnrollmentDataReader data(std::cin);

  std::cout << "Data parsed in " << timer << std::endl;
  timer.clear();

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

  // const auto stFilter = [&](const io::Student& student) -> bool {
  //   return student.get_strm() == 2198;
  // };
  const static_analysis::io_filters::Const<true> stFilter;

  const auto seFilter = [&](const io::Section& section) -> bool {
    return forbiddenComps.find(section.get_component()) == forbiddenComps.end() &&
           section.get_mode() == io::Section::Mode::FACE_TO_FACE;
  };

  //static_analysis::Graph<double> g = static_analysis::student_to_student_weighted<double, decltype(stFilter), decltype(seFilter)>(data, stFilter, seFilter);

  //std::cout << "Graph generated in " << timer << std::endl;

  const auto ptMetrics = [&](const static_analysis::Graph<double>& g) {
    timer.clear();

    static_analysis::GraphMetrics<double> metrics = compute_metrics(g);

    std::cout << "Metrics computed in " << timer << std::endl;

    std::cout << "full_node_count: " << metrics.full_node_count << std::endl;
    std::cout << "full_edge_count: " << metrics.full_edge_count << std::endl;
    std::cout << "comp_node_count: " << metrics.comp_node_count << std::endl;
    std::cout << "comp_edge_count: " << metrics.comp_edge_count << std::endl;
    std::cout << "bcmp_node_count: " << metrics.bcmp_node_count << std::endl;
    std::cout << "bcmp_edge_count: " << metrics.bcmp_edge_count << std::endl;
    std::cout << "average_degree : " << metrics.average_degree << std::endl;
    std::cout << "c_node_percent : " << metrics.c_node_percent << std::endl;
    std::cout << "avg_inv_weight : " << metrics.avg_inv_weight << std::endl;
    std::cout << "comp_diam_unwtd: " << metrics.comp_diam_unwtd << std::endl;
    std::cout << "c_avg_geo_dist : " << metrics.c_avg_geo_dist << std::endl;
    std::cout << "c_local_c_coef : " << metrics.c_local_c_coef << std::endl;
    std::cout << "c_global_coeff : " << metrics.c_global_coeff << std::endl;
    std::cout << "c_ntwk_density : " << metrics.c_ntwk_density << std::endl;
  };

  // std::cout << "University" << std::endl;
  // ptMetrics(g);

  const auto stFilterGrad = [&](const io::Student& student) -> bool {
    return student.get_career() == io::Student::Career::GRAD;
  };
  std::cout << "Graduates" << std::endl;
  ptMetrics(static_analysis::student_to_student_weighted<double, decltype(stFilterGrad), decltype(seFilter)>(data, stFilterGrad, seFilter));
  std::cout << std::endl;

  const auto seFilter1 = [&](const io::Section& section) -> bool {
    return seFilter(section) && section.get_catalog_number().front() == '1';
  };
  std::cout << "1xxx" << std::endl;
  ptMetrics(static_analysis::student_to_student_weighted<double, decltype(stFilter), decltype(seFilter1)>(data, stFilter, seFilter1));
  std::cout << std::endl;
  const auto seFilter2 = [&](const io::Section& section) -> bool {
    return seFilter(section) && section.get_catalog_number().front() == '2';
  };
  std::cout << "2xxx" << std::endl;
  ptMetrics(static_analysis::student_to_student_weighted<double, decltype(stFilter), decltype(seFilter2)>(data, stFilter, seFilter2));
  std::cout << std::endl;
  const auto seFilter3 = [&](const io::Section& section) -> bool {
    return seFilter(section) && section.get_catalog_number().front() == '3';
  };
  std::cout << "3xxx" << std::endl;
  ptMetrics(static_analysis::student_to_student_weighted<double, decltype(stFilter), decltype(seFilter3)>(data, stFilter, seFilter3));
  std::cout << std::endl;
  const auto seFilter4 = [&](const io::Section& section) -> bool {
    return seFilter(section) && section.get_catalog_number().front() == '4';
  };
  std::cout << "4xxx" << std::endl;
  ptMetrics(static_analysis::student_to_student_weighted<double, decltype(stFilter), decltype(seFilter4)>(data, stFilter, seFilter4));
  std::cout << std::endl;
  const auto seFilter56 = [&](const io::Section& section) -> bool {
    return seFilter(section) && (section.get_catalog_number().front() == '5' || section.get_catalog_number().front() == '6');
  };
  std::cout << "5xxx 6xxx" << std::endl;
  ptMetrics(static_analysis::student_to_student_weighted<double, decltype(stFilter), decltype(seFilter56)>(data, stFilter, seFilter56));
  std::cout << std::endl;
  const auto seFilter7 = [&](const io::Section& section) -> bool {
    return seFilter(section) && section.get_catalog_number().front() == '7';
  };
  std::cout << "7xxx" << std::endl;
  ptMetrics(static_analysis::student_to_student_weighted<double, decltype(stFilter), decltype(seFilter7)>(data, stFilter, seFilter7));
  std::cout << std::endl;

  // std::cout << "Undergraduates" << std::endl;
  // ptMetrics(g.subgraph([&](size_t i) { return (*data.get_students())[i].get_career() == io::Student::Career::UGRD; }));
}
