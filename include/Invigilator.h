#pragma once
#include <string>
#include <vector>
#include "FileUtils.h"

struct Invigilator {
    std::string invigilatorId;
    std::string name;
    std::string department;
    std::string phone;
    std::string email;
    bool isAvailable = true;

    std::string toCSV() const {
        return invigilatorId + "," + name + "," + department + "," + phone + "," +
               email + "," + (isAvailable ? "1" : "0");
    }

    static Invigilator fromCSV(const std::vector<std::string>& f) {
        Invigilator inv;
        if (f.size() < 6) return inv;
        inv.invigilatorId = f[0]; inv.name = f[1]; inv.department = f[2];
        inv.phone = f[3]; inv.email = f[4]; inv.isAvailable = (f[5] == "1");
        return inv;
    }
};

class InvigilatorManager {
public:
    explicit InvigilatorManager(std::string path) : filePath(std::move(path)) { load(); }

    bool addInvigilator(const Invigilator& i) {
        if (searchInvigilator(i.invigilatorId) != nullptr) return false;
        invigilators.push_back(i);
        save();
        return true;
    }

    bool updateInvigilator(const Invigilator& updated) {
        for (auto& i : invigilators) {
            if (i.invigilatorId == updated.invigilatorId) { i = updated; save(); return true; }
        }
        return false;
    }

    bool deleteInvigilator(const std::string& id) {
        for (size_t idx = 0; idx < invigilators.size(); idx++) {
            if (invigilators[idx].invigilatorId == id) {
                invigilators.erase(invigilators.begin() + idx);
                save();
                return true;
            }
        }
        return false;
    }

    Invigilator* searchInvigilator(const std::string& id) {
        for (auto& i : invigilators) if (i.invigilatorId == id) return &i;
        return nullptr;
    }

    const std::vector<Invigilator>& viewInvigilators() const { return invigilators; }

    // Manual availability flag the admin can toggle (sick leave etc.)
    // Real double-booking conflict checking happens in AllocationManager,
    // which looks at actual assigned slots rather than this flag alone.
    bool checkAvailability(const std::string& id) {
        auto* inv = searchInvigilator(id);
        return inv != nullptr && inv->isAvailable;
    }

    int count() const { return static_cast<int>(invigilators.size()); }

private:
    std::string filePath;
    std::vector<Invigilator> invigilators;

    void load() {
        invigilators.clear();
        for (auto& row : FileUtils::readCSV(filePath, true)) {
            invigilators.push_back(Invigilator::fromCSV(row));
        }
    }

    void save() {
        std::vector<std::string> lines;
        for (auto& i : invigilators) lines.push_back(i.toCSV());
        FileUtils::writeCSV(filePath, "invigilatorId,name,department,phone,email,isAvailable", lines);
    }
};
