#include "small_world_io/Section.hpp"

using small_world::io::Section;

Section::Section(std::shared_ptr<const std::vector<std::size_t>> students,
                 std::size_t number,
                 std::size_t course_id,
                 Session session,
                 time_t session_begin,
                 Subject subject,
                 std::string catalog_number,
                 std::string section_name,
                 Group group,
                 Organization organization,
                 std::size_t units,
                 Component component,
                 time_t start_date,
                 time_t end_date,
                 Facility facility,
                 std::size_t room_capacity,
                 time_t start_time,
                 time_t end_time,
                 bool monday,
                 bool tuesday,
                 bool wednesday,
                 bool thursday,
                 bool friday,
                 bool saturday,
                 bool sunday,
                 Mode mode) :
  students(students),
  number(number),
  course_id(course_id),
  session(session),
  session_begin(session_begin),
  subject(subject),
  catalog_number(catalog_number),
  section_name(section_name),
  group(group),
  organization(organization),
  units(units),
  component(component),
  start_date(start_date),
  end_date(end_date),
  facility(facility),
  room_capacity(room_capacity),
  start_time(start_time),
  end_time(end_time),
  monday(monday),
  tuesday(tuesday),
  wednesday(wednesday),
  thursday(thursday),
  friday(friday),
  saturday(saturday),
  sunday(sunday),
  mode(mode)
{}
          

Section::~Section() {}

std::shared_ptr<const std::vector<std::size_t>> Section::get_students() const {
  return this->students;
}

std::size_t Section::get_number() const {
  return this->number;
}

std::size_t Section::get_course_id() const {
  return this->course_id;
}

Section::Session Section::get_session() const {
  return this->session;
}

time_t Section::get_session_begin() const {
  return this->session_begin;
}

Section::Subject Section::get_subject() const {
  return this->subject;
}

std::string Section::get_catalog_number() const {
  return this->catalog_number;
}

std::string Section::get_section_name() const {
  return this->section_name;
}

Section::Group Section::get_group() const {
  return this->group;
}

Section::Organization Section::get_organization() const {
  return this->organization;
}

std::size_t Section::get_units() const {
  return this->units;
}

Section::Component Section::get_component() const {
  return this->component;
}

time_t Section::get_start_date() const {
  return this->start_date;
}

time_t Section::get_end_date() const {
  return this->end_date;
}

Section::Facility Section::get_facility() const {
  return this->facility;
}

std::size_t Section::get_room_capacity() const {
  return this->room_capacity;
}

time_t Section::get_start_time() const {
  return this->start_time;
}

time_t Section::get_end_time() const {
  return this->end_time;
}

bool Section::get_monday() const {
  return this->monday;
}

bool Section::get_tuesday() const {
  return this->tuesday;
}

bool Section::get_wednesday() const {
  return this->wednesday;
}

bool Section::get_thursday() const {
  return this->thursday;
}

bool Section::get_friday() const {
  return this->friday;
}

bool Section::get_saturday() const {
  return this->saturday;
}

bool Section::get_sunday() const {
  return this->sunday;
}

Section::Mode Section::get_mode() const {
  return this->mode;
}

bool operator==(const small_world::io::Section& l, const small_world::io::Section& r) {
  return *l.get_students() == *r.get_students() &&
         l.get_number() == r.get_number() &&
         l.get_course_id() == r.get_course_id() &&
         l.get_session() == r.get_session() &&
         l.get_session_begin() == r.get_session_begin() &&
         l.get_subject() == r.get_subject() &&
         l.get_catalog_number() == r.get_catalog_number() &&
         l.get_section_name() == r.get_section_name() &&
         l.get_group() == r.get_group() &&
         l.get_organization() == r.get_organization() &&
         l.get_units() == r.get_units() &&
         l.get_component() == r.get_component() &&
         l.get_start_date() == r.get_start_date() &&
         l.get_end_date() == r.get_end_date() &&
         l.get_facility() == r.get_facility() &&
         l.get_room_capacity() == r.get_room_capacity() &&
         l.get_start_time() == r.get_start_time() &&
         l.get_end_time() == r.get_end_time() &&
         l.get_monday() == r.get_monday() &&
         l.get_tuesday() == r.get_tuesday() &&
         l.get_wednesday() == r.get_wednesday() &&
         l.get_thursday() == r.get_thursday() &&
         l.get_friday() == r.get_friday() &&
         l.get_saturday() == r.get_saturday() &&
         l.get_sunday() == r.get_sunday() &&
         l.get_mode() == r.get_mode();
}

bool operator!=(const small_world::io::Section& l, const small_world::io::Section& r) {
  return !(l == r);
}
