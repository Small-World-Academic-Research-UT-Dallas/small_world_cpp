#pragma once

#include <cassert>
#include <tuple>
#include <type_traits>

#include <small_world_io/CsvEnrollmentDataReader.hpp>
#include <small_world_io/Section.hpp>
#include <small_world_io/Student.hpp>

namespace small_world::static_analysis {

namespace io_filters {

template<bool C>
struct Const {
  inline constexpr bool operator()(const small_world::io::Student& student) const noexcept {
    return C;
  }

  inline constexpr bool operator()(const small_world::io::Section& section) const noexcept {
    return C;
  }
};

struct Strm {
  size_t strm;

  explicit inline Strm(size_t strm) :
    strm(strm)
  {}

  inline bool operator()(const small_world::io::Student& student) const noexcept {
    return student.get_strm() == strm;
  }

  inline bool operator()(const small_world::io::Section& section) const noexcept {
    return section.get_strm() == strm;
  }
};

}

template<typename Filter>
inline constexpr void filterStudents(std::vector<bool>& ans, const io::EnrollmentDataReader& data, const Filter& filter = Filter{}) {
  const std::vector<io::Student>& students = *data.get_students();
  assert(ans.size() == students.size());
  for (size_t i = 0; i < students.size(); i++)
    ans[i] = ans[i] & filter(students[i]);
}

template<typename Filter>
inline constexpr void filterSections(std::vector<bool>& ans, const io::EnrollmentDataReader& data, const Filter& filter = Filter{}) {
  const std::vector<io::Section>& sections = *data.get_sections();
  assert(ans.size() == sections.size());
  for (size_t i = 0; i < sections.size(); i++)
    ans[i] = ans[i] & filter(sections[i]);
}

namespace index_filters {

template<typename S>
struct Set {
  const S& set;
  explicit inline Set(const S& set) :
    set(set)
  {}

  inline bool operator()(size_t i) const noexcept {
    return set.find(i) != set.end();
  }
};

}

}