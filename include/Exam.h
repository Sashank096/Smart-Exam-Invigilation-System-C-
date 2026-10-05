#pragma once
#include <string>
#include <vector>
#include "FileUtils.h"

struct Exam {
    std::string examId;
    std::string subject;      // free text - admin decides
    std::string department;
    std::string examDate;     // "DD-MM-YYYY"
    std::string startTime;    // "HH:MM AM/PM" - admin decides, not fixed slots
    std::string endTime;

    std::string toCSV() const {
        return examId + "," + subject + "," + department + "," + examDate + "," +
               startTime + "," + endTime;
    }

    static Exam fromCSV(const std::vector<std::string>& f) {
        Exam e;
        if (f.size() < 6) return e;
        e.examId = f[0]; e.subject = f[1]; e.department = f[2];
        e.examDate = f[3]; e.startTime = f[4]; e.endTime = f[5];
        return e;
    }
};

class ExamManager {
public:
    explicit ExamManager(std::string path) : filePath(std::move(path)) { load(); }

    bool addExam(const Exam& e) {
        if (searchExam(e.examId) != nullptr) return false;
        exams.push_back(e);
        save();
        return true;
    }

    bool updateExam(const Exam& updated) {
        for (auto& e : exams) {
            if (e.examId == updated.examId) { e = updated; save(); return true; }
        }
        return false;
    }

    bool deleteExam(const std::string& examId) {
        for (size_t i = 0; i < exams.size(); i++) {
            if (exams[i].examId == examId) { exams.erase(exams.begin() + i); save(); return true; }
        }
        return false;
    }

    Exam* searchExam(const std::string& examId) {
        for (auto& e : exams) if (e.examId == examId) return &e;
        return nullptr;
    }

    const std::vector<Exam>& viewExams() const { return exams; }

    int count() const { return static_cast<int>(exams.size()); }

private:
    std::string filePath;
    std::vector<Exam> exams;

    void load() {
        exams.clear();
        for (auto& row : FileUtils::readCSV(filePath, true)) exams.push_back(Exam::fromCSV(row));
    }

    void save() {
        std::vector<std::string> lines;
        for (auto& e : exams) lines.push_back(e.toCSV());
        FileUtils::writeCSV(filePath, "examId,subject,department,examDate,startTime,endTime", lines);
    }
};
