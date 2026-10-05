#pragma once
#include <string>
#include <vector>
#include <fstream>
#include "Student.h"
#include "Invigilator.h"
#include "ExamHall.h"
#include "Exam.h"
#include "Allocation.h"

// Pulls together counts/lists from every manager for the Dashboard screen
// and for exporting a plain-text report - this is the Report/Dashboard
// pair from the class diagram, kept as one helper since both are read-only
// views over the other five managers.
class ReportManager {
public:
    ReportManager(StudentManager& s, InvigilatorManager& i, ExamHallManager& h,
                   ExamManager& e, AllocationManager& a)
        : students(s), invigilators(i), halls(h), exams(e), allocations(a) {}

    struct Summary {
        int totalStudents;
        int totalInvigilators;
        int totalHalls;
        int totalExams;
        int totalAllocations;
    };

    Summary showSummary() const {
        return { students.count(), invigilators.count(), halls.count(),
                 exams.count(), allocations.count() };
    }

    // Writes a plain text report to disk (exportReport). Returns the path.
    std::string exportReport(const std::string& outPath) const {
        std::ofstream f(outPath, std::ios::trunc);
        f << "SMART EXAM INVIGILATION SYSTEM - REPORT\n";
        f << "=========================================\n\n";

        f << "Exam Schedule (" << exams.count() << ")\n";
        for (auto& e : exams.viewExams()) {
            f << "  " << e.examId << " | " << e.subject << " | " << e.department
              << " | " << e.examDate << " | " << e.startTime << "-" << e.endTime << "\n";
        }

        f << "\nAllocations (" << allocations.count() << ")\n";
        for (auto& a : allocations.viewAllocations()) {
            f << "  " << a.allocationId << " | Exam:" << a.examId << " | Hall:" << a.hallId
              << " | Invigilator:" << a.invigilatorId << " | " << a.status << "\n";
        }

        f << "\nTotals: " << students.count() << " students, "
          << invigilators.count() << " invigilators, "
          << halls.count() << " halls\n";
        return outPath;
    }

private:
    StudentManager& students;
    InvigilatorManager& invigilators;
    ExamHallManager& halls;
    ExamManager& exams;
    AllocationManager& allocations;
};
