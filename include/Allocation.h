#pragma once
#include <string>
#include <vector>
#include "FileUtils.h"
#include "Exam.h"

struct Allocation {
    std::string allocationId;
    std::string examId;
    std::string invigilatorId;
    std::string hallId;
    std::string allocationDate; // duplicated from Exam for quick display/reporting
    std::string timeSlot;       // "startTime-endTime", duplicated from Exam
    std::string status = "Confirmed";

    std::string toCSV() const {
        return allocationId + "," + examId + "," + invigilatorId + "," + hallId + "," +
               allocationDate + "," + timeSlot + "," + status;
    }

    static Allocation fromCSV(const std::vector<std::string>& f) {
        Allocation a;
        if (f.size() < 7) return a;
        a.allocationId = f[0]; a.examId = f[1]; a.invigilatorId = f[2]; a.hallId = f[3];
        a.allocationDate = f[4]; a.timeSlot = f[5]; a.status = f[6];
        return a;
    }
};

enum class ConflictType {
    None,
    InvigilatorDoubleBooked,   // same invigilator, same date+time, different hall
    HallAlreadyAllocated       // same hall, same exam already has an invigilator
};

class AllocationManager {
public:
    AllocationManager(std::string path, ExamManager& examMgr)
        : filePath(std::move(path)), exams(examMgr) { load(); }

    // The core "smart" check: does assigning this invigilator to this exam/hall
    // clash with something they're already assigned to at an overlapping date+time?
    ConflictType checkConflict(const std::string& invigilatorId, const std::string& examId,
                                const std::string& hallId) {
        Exam* exam = exams.searchExam(examId);
        if (!exam) return ConflictType::None; // exam must exist; caller validates separately

        for (auto& a : allocations) {
            if (a.status != "Confirmed") continue;

            // Same invigilator already booked elsewhere at the same date+time?
            if (a.invigilatorId == invigilatorId) {
                Exam* other = exams.searchExam(a.examId);
                if (other && other->examDate == exam->examDate &&
                    timesOverlap(other->startTime, other->endTime, exam->startTime, exam->endTime) &&
                    a.hallId != hallId) {
                    return ConflictType::InvigilatorDoubleBooked;
                }
            }
            // Same hall, same exam, already has a different invigilator?
            if (a.hallId == hallId && a.examId == examId && a.invigilatorId != invigilatorId) {
                return ConflictType::HallAlreadyAllocated;
            }
        }
        return ConflictType::None;
    }

    // Returns {success, message}. Refuses to save on conflict - this is what
    // makes allocation "smart" rather than just a data table.
    std::pair<bool, std::string> assignInvigilator(const Allocation& a) {
        ConflictType c = checkConflict(a.invigilatorId, a.examId, a.hallId);
        if (c == ConflictType::InvigilatorDoubleBooked) {
            return { false, "Invigilator already assigned to another hall at this date/time." };
        }
        if (c == ConflictType::HallAlreadyAllocated) {
            return { false, "This hall already has an invigilator for this exam." };
        }
        allocations.push_back(a);
        save();
        return { true, "Allocated successfully." };
    }

    bool updateAllocation(const Allocation& updated) {
        for (auto& a : allocations) {
            if (a.allocationId == updated.allocationId) { a = updated; save(); return true; }
        }
        return false;
    }

    bool deleteAllocation(const std::string& id) {
        for (size_t i = 0; i < allocations.size(); i++) {
            if (allocations[i].allocationId == id) {
                allocations.erase(allocations.begin() + i);
                save();
                return true;
            }
        }
        return false;
    }

    const std::vector<Allocation>& viewAllocations() const { return allocations; }

    int count() const { return static_cast<int>(allocations.size()); }

private:
    std::string filePath;
    ExamManager& exams;
    std::vector<Allocation> allocations;

    // Very small time-overlap check assuming "H:MM AM/PM" format.
    // Converts to minutes-since-midnight and checks for overlap.
    static int toMinutes(const std::string& t) {
        // expects like "10:00 AM" or "2:30 PM"
        int hh = 0, mm = 0;
        char ampm[3] = {0,0,0};
        sscanf(t.c_str(), "%d:%d %2s", &hh, &mm, ampm);
        if ((ampm[0]=='P'||ampm[0]=='p') && hh != 12) hh += 12;
        if ((ampm[0]=='A'||ampm[0]=='a') && hh == 12) hh = 0;
        return hh * 60 + mm;
    }

    static bool timesOverlap(const std::string& s1, const std::string& e1,
                              const std::string& s2, const std::string& e2) {
        int a1 = toMinutes(s1), b1 = toMinutes(e1);
        int a2 = toMinutes(s2), b2 = toMinutes(e2);
        return a1 < b2 && a2 < b1;
    }

    void load() {
        allocations.clear();
        for (auto& row : FileUtils::readCSV(filePath, true)) allocations.push_back(Allocation::fromCSV(row));
    }

    void save() {
        std::vector<std::string> lines;
        for (auto& a : allocations) lines.push_back(a.toCSV());
        FileUtils::writeCSV(filePath, "allocationId,examId,invigilatorId,hallId,allocationDate,timeSlot,status", lines);
    }
};
