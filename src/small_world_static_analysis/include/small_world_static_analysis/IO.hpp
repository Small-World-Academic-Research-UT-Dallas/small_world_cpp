// This file provides methods for converting the data from the small_world_io directory into a format used by static_analysis.
#include <iostream>
#include <iomanip>

#include <small_world_io/CsvEnrollmentDataReader.hpp>
#include <small_world_io/Section.hpp>
#include <small_world_io/Student.hpp>

#include <small_world_static_analysis/Graph.hpp>

namespace small_world::static_analysis {

template<typename float_t>
Graph<float_t> student_to_student_weighted(const io::CsvEnrollmentDataReader& data) {
	Graph<float_t> graph;

	int sectionIdx = 0;
	for (const small_world::io::Section& section : *data.get_sections()) {
		if (++sectionIdx % 1 == 0)
			std::cout << "\rsection " << sectionIdx << ", size " << section.get_students()->size() << std::flush;
		for (size_t u : *section.get_students())
			for (size_t v : *section.get_students()) {
				if (u == v) continue;
				graph.get_adj()[u][v] += section.get_contact();
			}
	}

	return graph;
}

}
