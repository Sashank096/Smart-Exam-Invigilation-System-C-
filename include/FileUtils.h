#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

// Small CSV helper used by every manager class for persistence.
// Kept deliberately simple (no quoted-comma support) since all our
// fields are plain names/ids/numbers.
namespace FileUtils {

    inline std::vector<std::string> splitCSVLine(const std::string& line) {
        std::vector<std::string> fields;
        std::stringstream ss(line);
        std::string field;
        while (std::getline(ss, field, ',')) {
            fields.push_back(field);
        }
        return fields;
    }

    inline std::vector<std::vector<std::string>> readCSV(const std::string& path, bool hasHeader = true) {
        std::vector<std::vector<std::string>> rows;
        std::ifstream file(path);
        if (!file.is_open()) return rows; // file may not exist yet - that's fine

        std::string line;
        bool first = true;
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            if (first && hasHeader) { first = false; continue; }
            first = false;
            rows.push_back(splitCSVLine(line));
        }
        return rows;
    }

    inline void writeCSV(const std::string& path, const std::string& header,
                          const std::vector<std::string>& lines) {
        std::ofstream file(path, std::ios::trunc);
        file << header << "\n";
        for (const auto& l : lines) file << l << "\n";
    }

    inline void appendLine(const std::string& path, const std::string& header,
                            const std::string& line) {
        std::ifstream check(path);
        bool needsHeader = !check.good();
        check.close();
        std::ofstream file(path, std::ios::app);
        if (needsHeader) file << header << "\n";
        file << line << "\n";
    }
}
