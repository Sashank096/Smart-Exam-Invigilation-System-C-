#pragma once
#include <string>
#include <vector>
#include "FileUtils.h"

struct ExamHall {
    std::string hallId;
    std::string hallNumber;
    std::string block;    // e.g. "RTB Bhavan" - admin picks from configured blocks or types a new one
    int capacity = 0;
    bool isAvailable = true;

    std::string toCSV() const {
        return hallId + "," + hallNumber + "," + block + "," + std::to_string(capacity) +
               "," + (isAvailable ? "1" : "0");
    }

    static ExamHall fromCSV(const std::vector<std::string>& f) {
        ExamHall h;
        if (f.size() < 5) return h;
        h.hallId = f[0]; h.hallNumber = f[1]; h.block = f[2];
        h.capacity = std::stoi(f[3]); h.isAvailable = (f[4] == "1");
        return h;
    }
};

class ExamHallManager {
public:
    explicit ExamHallManager(std::string path) : filePath(std::move(path)) { load(); }

    bool addHall(const ExamHall& h) {
        if (searchHall(h.hallId) != nullptr) return false;
        halls.push_back(h);
        save();
        return true;
    }

    bool updateHall(const ExamHall& updated) {
        for (auto& h : halls) {
            if (h.hallId == updated.hallId) { h = updated; save(); return true; }
        }
        return false;
    }

    bool deleteHall(const std::string& hallId) {
        for (size_t i = 0; i < halls.size(); i++) {
            if (halls[i].hallId == hallId) { halls.erase(halls.begin() + i); save(); return true; }
        }
        return false;
    }

    ExamHall* searchHall(const std::string& hallId) {
        for (auto& h : halls) if (h.hallId == hallId) return &h;
        return nullptr;
    }

    const std::vector<ExamHall>& viewHalls() const { return halls; }

    bool checkAvailability(const std::string& hallId) {
        auto* h = searchHall(hallId);
        return h != nullptr && h->isAvailable;
    }

    int count() const { return static_cast<int>(halls.size()); }

private:
    std::string filePath;
    std::vector<ExamHall> halls;

    void load() {
        halls.clear();
        for (auto& row : FileUtils::readCSV(filePath, true)) halls.push_back(ExamHall::fromCSV(row));
    }

    void save() {
        std::vector<std::string> lines;
        for (auto& h : halls) lines.push_back(h.toCSV());
        FileUtils::writeCSV(filePath, "hallId,hallNumber,block,capacity,isAvailable", lines);
    }
};
