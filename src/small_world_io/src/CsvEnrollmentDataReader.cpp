#include <istream>
#include <fstream>
#include <memory>
#include <vector>
#include <iostream>
#include <map>

#include "small_world_io/CsvEnrollmentDataReader.hpp"
#include "small_world_io/Section.hpp"
#include "small_world_io/Student.hpp"
#include "fstream"
#include "algorithm"
#include "string.h"
#define len 2048 // 48
#define stuid 0
#define crsnbr 13 

using small_world::io::CsvEnrollmentDataReader;

CsvEnrollmentDataReader::CsvEnrollmentDataReader(std::ifstream &input) {
  std::cout << "Reading input file..." << std::endl;
  
  std::vector<int> numsr = std::vector<int>();
  std::vector<Student> mstudents = std::vector<Student>(); // each student's sections
  std::vector<Section> msections = std::vector<Section>(); // each section's students
  std::map<int, std::vector<int>> dictKnownSections = std::map<int, std::vector<int>> (); // (crse nbr, list of students)
  std::shared_ptr<const std::vector<Student>> x = std::make_shared<std::vector<small_world::io::Student>>();
  this->students = std::make_shared<std::vector<small_world::io::Student>>();
  this->sections = std::make_shared<std::vector<small_world::io::Section>>();

  numsr.push_back(0);
  
  const std::shared_ptr<std::vector<std::size_t>> stu_sec = std::make_shared<std::vector<std::size_t>>(); 
  stu_sec->push_back(0);
  Student stu = Student(stu_sec);
  mstudents.push_back(stu);
  // this->students->push_back(stu); // does not work?

  std::shared_ptr<std::vector<std::size_t>> sec_stu = std::make_shared<std::vector<std::size_t>>();
  sec_stu->push_back(0);
  Section sec = Section(sec_stu);
  msections.push_back(sec);

  std::string line;
  int i = 0;
  int student_id = 0;
  const std::shared_ptr<std::vector<std::size_t>> currStuSec = std::make_shared<std::vector<std::size_t>>();
  if (input.is_open()) {
    while (std::getline(input, line)) {
      // process line
      char *chrstr = const_cast<char*>(line.c_str());
      if (i <= 0) {
        i++;
        continue;
      }
  
      char *arr[len];
      int n = 0;
      char *curr = strtok(chrstr, "\t");
      arr[0] = curr;

      while (curr != NULL) {
        curr = strtok(NULL, "\t");
        arr[++n] = curr;
      }
      
      // process arr
      // std::cout << arr[stuid] << " in " << arr[5] << " in course " << arr[crsnbr] << "\n"; // "\r";
      // add each student
      int cur_id = atoi(arr[stuid]);
      if (student_id != cur_id) {
        // save student
        const std::shared_ptr<std::vector<std::size_t>> secs = currStuSec;
        Student stude = Student(secs);
        mstudents.push_back(stude);
        // new student
        currStuSec->clear();
      }

      currStuSec->push_back(atoi(arr[crsnbr])); // add P - class nbr
      student_id = cur_id;

      // add each section
      if (dictKnownSections.find(atoi(arr[crsnbr])) != dictKnownSections.end()) {
        // section exists
        std::vector<int> secti = dictKnownSections.at(atoi(arr[crsnbr]));
        secti.push_back(atoi(arr[0])); // add A - student ID
      } else {
        // new section
        std::vector<int> secti = std::vector<int>();
        secti.push_back(atoi(arr[stuid]));
        dictKnownSections.emplace(atoi(arr[crsnbr]), secti);
      }
      

      printf("%.2f%\r", (i/620403.0)*100);
      i++;
    }
    // ??
    for (auto const& kv : dictKnownSections) {
      int course = kv.first;
      std::vector<int> sts = kv.second;
      std::shared_ptr<std::vector<std::size_t>> currSect = std::make_shared<std::vector<std::size_t>>();
      for (int k : sts) currSect->push_back(k);
      Section secti = Section(currSect);
      msections.push_back(secti);
    }
    
    std::cout << "\nDone" << std::endl;
    input.close();
  }
}

CsvEnrollmentDataReader::~CsvEnrollmentDataReader() {}

std::shared_ptr<const std::vector<small_world::io::Student>> CsvEnrollmentDataReader::get_students() const {
  return this->students;
}

std::shared_ptr<const std::vector<small_world::io::Section>> CsvEnrollmentDataReader::get_sections() const {
  return this->sections;
}
