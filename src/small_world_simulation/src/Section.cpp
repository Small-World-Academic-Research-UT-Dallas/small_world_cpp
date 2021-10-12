#include "small_world_simulation/Section.hpp"
#include "small_world_simulation/SectionRecord.hpp"
#include "small_world_simulation/Simulation.hpp"

using namespace small_world::simulation;

Section::Section(const SectionRecord & section) {
  this->students = section.get_students();
}

Section::~Section() {}

void Section::simulate_section(Simulation & simulation) {
  
}
