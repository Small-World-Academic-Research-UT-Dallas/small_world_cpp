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
  
  std::vector<Student> mstudents = std::vector<Student>(); // each student's sections
  std::vector<Section> msections = std::vector<Section>(); // each section's students
  std::map<int, std::vector<int>> dictKnownSections = std::map<int, std::vector<int>> (); // (crse nbr, list of students)
  
  this->students = std::make_shared<std::vector<small_world::io::Student>>();
  this->sections = std::make_shared<std::vector<small_world::io::Section>>();

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
        secti.push_back(atoi(arr[stuid])); // add A - student ID
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
    for (auto const &kv : dictKnownSections) {
      int course = kv.first;
      std::vector<int> sts = kv.second;
      std::shared_ptr<std::vector<std::size_t>> currSect = std::make_shared<std::vector<std::size_t>>();
      for (int k : sts) currSect->push_back(k);
      Section secti = Section(currSect);
      msections.push_back(secti);
    }

    const std::vector<Student> mstudents = mstudents;
    const std::vector<Section> msections = msections;
    this->sections = std::make_shared<const std::vector<Section>>(msections);
    this->students = std::make_shared<const std::vector<Student>>(mstudents);
    
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
