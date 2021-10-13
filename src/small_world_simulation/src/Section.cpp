#include "small_world_simulation/Section.hpp"
#include "small_world_simulation/Simulation.hpp"
#include "small_world_io/Section.hpp"

using namespace small_world::simulation;

Section::Section(const small_world::io::Section & section) {
  this->students = section.get_students();
}

Section::~Section() {}

void Section::simulate_section(Simulation & simulation) {

}
