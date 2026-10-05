#pragma once
#include <string>
#include <vector>
#include "FileUtils.h"

struct Student {
    std::string rollNo;
    std::string name;
    std::string department;
    int year = 1;
    std::string hallNumber; // assigned at allocation time, may be blank
    std::string email;
    std::string phone;

    std::string toCSV() const {
        return rollNo + "," + name + "," + department + "," + std::to_string(year) +
               "," + hallNumber + "," + email + "," + phone;
    }

    static Student fromCSV(const std::vector<std::string>& f) {
        Student s;
        if (f.size() < 7) return s;
        s.rollNo = f[0]; s.name = f[1]; s.department = f[2];
        s.year = std::stoi(f[3]);
        s.hallNumber = f[4]; s.email = f[5]; s.phone = f[6];
        return s;
    }
};

class StudentManager {
public:
    explicit StudentManager(std::string path) : filePath(std::move(path)) { load(); }

    bool addStudent(const Student& s) {
        if (searchStudent(s.rollNo) != nullptr) return false; // duplicate roll no
        students.push_back(s);
        save();
        return true;
    }

    bool updateStudent(const Student& updated) {
        for (auto& s : students) {
            if (s.rollNo == updated.rollNo) { s = updated; save(); return true; }
        }
        return false;
    }

    bool deleteStudent(const std::string& rollNo) {
        for (size_t i = 0; i < students.size(); i++) {
            if (students[i].rollNo == rollNo) {
                students.erase(students.begin() + i);
                save();
                return true;
            }
        }
        return false;
    }

    Student* searchStudent(const std::string& rollNo) {
        for (auto& s : students) if (s.rollNo == rollNo) return &s;
        return nullptr;
    }

    const std::vector<Student>& viewStudents() const { return students; }

    // Bulk import from a CSV exported from Excel: rollNo,name,department,year,hallNumber,email,phone
    // Returns {imported, skipped} counts.
    std::pair<int,int> importFromCSV(const std::string& importPath) {
        auto rows = FileUtils::readCSV(importPath, true);
        int imported = 0, skipped = 0;
        for (auto& row : rows) {
            if (row.size() < 3) { skipped++; continue; } // need at least rollNo,name,department
            Student s;
            s.rollNo = row[0];
            s.name = row[1];
            s.department = row[2];
            s.year = row.size() > 3 && !row[3].empty() ? std::stoi(row[3]) : 1;
            s.hallNumber = row.size() > 4 ? row[4] : "";
            s.email = row.size() > 5 ? row[5] : "";
            s.phone = row.size() > 6 ? row[6] : "";
            if (addStudent(s)) imported++; else skipped++;
        }
        return { imported, skipped };
    }

    int count() const { return static_cast<int>(students.size()); }

private:
    std::string filePath;
    std::vector<Student> students;

    void load() {
        students.clear();
        for (auto& row : FileUtils::readCSV(filePath, true)) {
            students.push_back(Student::fromCSV(row));
        }
    }

    void save() {
        std::vector<std::string> lines;
        for (auto& s : students) lines.push_back(s.toCSV());
        FileUtils::writeCSV(filePath, "rollNo,name,department,year,hallNumber,email,phone", lines);
    }
};
