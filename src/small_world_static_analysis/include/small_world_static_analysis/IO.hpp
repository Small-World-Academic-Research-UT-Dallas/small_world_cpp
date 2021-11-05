#pragma once

// This file provides methods for converting the data from the small_world_io directory into a format used by static_analysis.
#include <ctime>
#include <iostream>
#include <iomanip>
#include <unordered_set>

#include <small_world_io/CsvEnrollmentDataReader.hpp>
#include <small_world_io/Section.hpp>
#include <small_world_io/Student.hpp>

#include <small_world_static_analysis/Filters.hpp>
#include <small_world_static_analysis/Graph.hpp>

namespace small_world::static_analysis {

template<typename float_t, typename StF = io_filters::Const<true>, typename SeF = io_filters::Const<true>>
Graph<float_t> student_to_student_weighted(const io::EnrollmentDataReader& data, const StF& stFilter = StF{}, const SeF& seFilter = SeF{}) {
  Graph<float_t> graph;

  const std::vector<io::Student>& students = *data.get_students();
  const std::vector<io::Section>& sections = *data.get_sections();

  std::vector<bool> stMask(students.size(), true);
  std::vector<bool> seMask(sections.size(), true);
  filterStudents(stMask, data, stFilter);
  filterSections(seMask, data, seFilter);

  for (size_t i = 0; i < students.size(); i++)
    if (stMask[i])
      graph.get_adj()[i];

  for (size_t i = 0; i < sections.size(); i++) {
    if (!seMask[i])
      continue;
    const io::Section& section = sections[i];

    size_t days = 0;
    days += section.get_monday();
    days += section.get_tuesday();
    days += section.get_wednesday();
    days += section.get_thursday();
    days += section.get_friday();
    days += section.get_saturday();
    days += section.get_sunday();
    if (days == 0)
      continue;

    // TODO compute total days over semester using start and end date fields
    time_t seconds = section.get_end_time() - section.get_start_time();
    double contact = seconds * days * (1. / 3600);

    for (size_t u : *section.get_students()) {
      if (!stMask[u])
        continue;
      for (size_t v : *section.get_students()) {
        if (u == v || !stMask[v])
          continue;
        graph.get_adj()[u][v] += contact;
      }
    }
  }

  for (auto& [u, adj] : graph.get_adj())
    for (auto& [v, wt] : adj)
      wt = 1 / wt;

  return graph;
}

}
