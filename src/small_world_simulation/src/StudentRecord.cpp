#include "small_world_io/Student.hpp"
#include "small_world_simulation/StudentRecord.hpp"

using small_world::simulation::StudentRecord;

StudentRecord::StudentRecord() {

}

StudentRecord::StudentRecord(const small_world::io::Student & student) {
  (void) student; // student is unused, at least for now
}

StudentRecord::~StudentRecord() {

}
