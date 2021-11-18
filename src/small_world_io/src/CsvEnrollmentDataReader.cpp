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
#define crsnbr 15 

using small_world::io::CsvEnrollmentDataReader;

CsvEnrollmentDataReader::CsvEnrollmentDataReader(std::istream & input) {
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
  if (input) {
    while (std::getline(input, line)) {
      // process line
      // char *chrstr = const_cast<char*>(line.c_str());
      if (i <= 0) {
        i++;
        continue;
      }
  
      char *arr[len];
      int n = 0;
      char *curr;
      
      char *string = strdup(line.c_str());
      while ((curr = strsep(&string, "\t")) != NULL) {
        arr[n++] = curr;
      }
      
      
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
      
      // navigate
      int cur_idx = mstudents.size()-1;
      
      // process arr
      // std::cout << "student " << arr[stuid] << "idx" << cur_idx << " in course " << arr[crsnbr] << "\n"; // "\r";
      
      int key = atoi(arr[crsnbr]);
      auto found = dictKnownSections.find(key);
      
      // add each section
      if (found != dictKnownSections.end()) {
        // section exists
        std::vector<int> secti = (*found).second; // dictKnownSections.at(atoi(arr[crsnbr]));
        
        if (std::find(secti.begin(), secti.end(), cur_id) == secti.end()) { // todo use set instead.
          secti.push_back(cur_idx); // add cur_id student ID
          dictKnownSections[key] = secti;
        }
      } else {
        // new section
        std::vector<int> secti = std::vector<int>();
        secti.push_back(cur_idx); // add cur_id
        dictKnownSections.emplace(key, secti);
      }
      
      // std::cout << "{\n"; // sanity check
      // for(const auto &elem : dictKnownSections){
      //   std::cout << "\t" << elem.first << ", " << elem.second.size() << "\n";
      // }
      // std::cout << "}\n"; // --

      student_id = cur_id;
      printf("%.2f%\r", (i/620403.0)*100);
      i++;
    }
    // ??
    for (auto const &kv : dictKnownSections) {
      int course = kv.first;
      std::vector<int> stdnts(kv.second);
      std::vector<std::size_t> sts(stdnts.begin(), stdnts.end());
      std::shared_ptr<std::vector<std::size_t>> currSect = std::make_shared<std::vector<std::size_t>>(sts);
      Section secti = Section(currSect);
      msections.push_back(secti);
    }

    std::cout << mstudents.size() << " students, " << msections.size() << " sections." << std::endl;
    this->sections = std::make_shared<const std::vector<Section>>(msections);
    this->students = std::make_shared<const std::vector<Student>>(mstudents);
    
    std::cout << "Done" << std::endl;
    // input.close();
  }
}

CsvEnrollmentDataReader::~CsvEnrollmentDataReader() {}

std::shared_ptr<const std::vector<small_world::io::Student>> CsvEnrollmentDataReader::get_students() const {
  return this->students;
}

std::shared_ptr<const std::vector<small_world::io::Section>> CsvEnrollmentDataReader::get_sections() const {
  return this->sections;
}
