// Temporary console harness to verify the C++ core (models + conflict logic)
// works correctly, before the SFML GUI is layered on top.
#include <iostream>
#include "../include/Admin.h"
#include "../include/Student.h"
#include "../include/Invigilator.h"
#include "../include/ExamHall.h"
#include "../include/Exam.h"
#include "../include/Allocation.h"
#include "../include/Report.h"

int main() {
    Admin admin("data/admin.csv");
    std::cout << "Login Admin/Admin123: " << (admin.login("Admin", "Admin123") ? "OK" : "FAIL") << "\n";
    std::cout << "Login wrong pass: " << (!admin.login("Admin", "wrong") ? "OK (rejected)" : "FAIL") << "\n";

    StudentManager students("data/students.csv");
    InvigilatorManager invigilators("data/invigilators.csv");
    ExamHallManager halls("data/halls.csv");
    ExamManager exams("data/exams.csv");
    AllocationManager allocations("data/allocations.csv", exams);
    ReportManager report(students, invigilators, halls, exams, allocations);

    // --- Students ---
    students.addStudent({"S101", "Rohan Sharma", "AIML", 2, "H-101", "rohan@test.com", "9876543210"});
    students.addStudent({"S102", "Priya Verma", "CSE", 3, "H-102", "priya@test.com", "9876543211"});
    std::cout << "Student count: " << students.count() << " (expect 2)\n";
    std::cout << "Duplicate roll no rejected: "
              << (!students.addStudent({"S101","dupe","CSE",1,"","",""}) ? "OK" : "FAIL") << "\n";

    // --- Invigilators ---
    invigilators.addInvigilator({"I201", "Amit Kumar", "AIML", "9876500000", "amit@test.com", true});
    invigilators.addInvigilator({"I202", "Sunita Rao", "CSE", "9876500001", "sunita@test.com", true});
    std::cout << "Invigilator count: " << invigilators.count() << " (expect 2)\n";

    // --- Halls (admin-defined, using the real block names) ---
    halls.addHall({"H1", "H-101", "RTB Bhavan", 60, true});
    halls.addHall({"H2", "H-102", "BILLGATES Bhavan", 40, true});
    std::cout << "Hall count: " << halls.count() << " (expect 2)\n";

    // --- Exams (subject/time fully admin-typed, not fixed slots) ---
    exams.addExam({"E301", "Data Structures", "AIML", "25-05-2026", "10:00 AM", "01:00 PM"});
    exams.addExam({"E302", "Operating Systems", "CSE", "25-05-2026", "10:30 AM", "01:30 PM"}); // overlaps E301
    std::cout << "Exam count: " << exams.count() << " (expect 2)\n";

    // --- Allocation + conflict logic ---
    auto r1 = allocations.assignInvigilator({"A401", "E301", "I201", "H1", "25-05-2026", "10:00 AM-01:00 PM"});
    std::cout << "Allocate I201 to E301/H1: " << r1.second << " (expect success)\n";

    // Same invigilator, overlapping time, different hall -> should be rejected
    auto r2 = allocations.assignInvigilator({"A402", "E302", "I201", "H2", "25-05-2026", "10:30 AM-01:30 PM"});
    std::cout << "Allocate I201 to overlapping E302/H2: " << r2.second
               << (!r2.first ? "  [OK - correctly rejected]\n" : "  [FAIL - should have been rejected]\n");

    // Different invigilator, same overlapping slot, different hall -> should succeed
    auto r3 = allocations.assignInvigilator({"A403", "E302", "I202", "H2", "25-05-2026", "10:30 AM-01:30 PM"});
    std::cout << "Allocate I202 to E302/H2: " << r3.second
               << (r3.first ? "  [OK]\n" : "  [FAIL]\n");

    // Same hall + same exam, different invigilator -> should be rejected
    auto r4 = allocations.assignInvigilator({"A404", "E301", "I202", "H1", "25-05-2026", "10:00 AM-01:00 PM"});
    std::cout << "Allocate I202 to E301/H1 (already has I201): " << r4.second
               << (!r4.first ? "  [OK - correctly rejected]\n" : "  [FAIL]\n");

    std::cout << "Allocation count: " << allocations.count() << " (expect 2, since 2 were rejected)\n";

    // --- Report / Dashboard ---
    auto summary = report.showSummary();
    std::cout << "\nDashboard summary -> students:" << summary.totalStudents
              << " invigilators:" << summary.totalInvigilators
              << " halls:" << summary.totalHalls
              << " exams:" << summary.totalExams
              << " allocations:" << summary.totalAllocations << "\n";

    std::string outPath = report.exportReport("data/report.txt");
    std::cout << "Report exported to: " << outPath << "\n";

    // --- CSV import test ---
    std::ofstream importFile("data/import_test.csv");
    importFile << "rollNo,name,department,year,hallNumber,email,phone\n";
    importFile << "S103,Aman Singh,IT,3,H-102,aman@test.com,9876543212\n";
    importFile << "S104,Neha Gupta,ECE,2,H-103,neha@test.com,9876543213\n";
    importFile.close();
    auto [imported, skipped] = students.importFromCSV("data/import_test.csv");
    std::cout << "\nCSV import -> imported:" << imported << " skipped:" << skipped
               << " (expect 2, 0)\n";
    std::cout << "Student count after import: " << students.count() << " (expect 4)\n";

    std::cout << "\nAll core checks completed.\n";
    return 0;
}
