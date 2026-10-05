#pragma once
#include <string>
#include <fstream>

// Single admin account, stored in its own small file so the password
// isn't hardcoded in the binary and can be changed without recompiling.
class Admin {
public:
    explicit Admin(std::string path) : filePath(std::move(path)) { load(); }

    bool login(const std::string& user, const std::string& pass) const {
        return user == username && pass == password;
    }

    bool resetPassword(const std::string& oldPass, const std::string& newPass) {
        if (oldPass != password) return false;
        password = newPass;
        save();
        return true;
    }

    std::string getUsername() const { return username; }

private:
    std::string filePath;
    std::string username = "Admin";
    std::string password = "Admin123";

    void load() {
        std::ifstream file(filePath);
        if (!file.is_open()) { save(); return; } // first run: create with defaults
        std::getline(file, username);
        std::getline(file, password);
        if (username.empty()) username = "Admin";
        if (password.empty()) password = "Admin123";
    }

    void save() const {
        std::ofstream file(filePath, std::ios::trunc);
        file << username << "\n" << password << "\n";
    }
};
