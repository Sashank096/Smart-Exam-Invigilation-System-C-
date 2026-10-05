#pragma once
#include "Screen.h"
#include "Widgets.h"
#include "Theme.h"
#include "HeaderBar.h"
#include "Sidebar.h"
#include "../Student.h"
#include <functional>

enum class StudentMode { Add, View, Search, Update, Delete };

class StudentScreen : public Screen {
public:
    StudentScreen(sf::Font& font, StudentManager& mgr, std::function<void()> onBack)
        : font(font), mgr(mgr),
          header(font, "Student Management", "Manage | Add | View | Update | Delete Students"),
          sidebar(font, {
              {"Add Student",    [this]{ setMode(StudentMode::Add);    }},
              {"View Students",  [this]{ setMode(StudentMode::View);   }},
              {"Search Student", [this]{ setMode(StudentMode::Search); }},
              {"Update Student", [this]{ setMode(StudentMode::Update); }},
              {"Delete Student", [this]{ setMode(StudentMode::Delete); }},
          }, std::move(onBack)) {

        // Form fields — 2 column layout matching PPT
        float fx = 270.f, fy = 168.f;
        idField   = std::make_unique<TextField>(font, "Student ID *",  sf::FloatRect(fx,       fy,       370.f, 38.f));
        hallField = std::make_unique<TextField>(font, "Hall Number *", sf::FloatRect(fx+410.f, fy,       370.f, 38.f));
        nameField = std::make_unique<TextField>(font, "Name *",        sf::FloatRect(fx,       fy+90.f,  370.f, 38.f));
        emailField= std::make_unique<TextField>(font, "Email *",       sf::FloatRect(fx+410.f, fy+90.f,  370.f, 38.f));
        deptField = std::make_unique<Dropdown> (font, "Department *",  sf::FloatRect(fx,       fy+180.f, 370.f, 38.f),
            {"AIML","CSE","ECE","MECH","DS","CIVIL","EEE","IT"});
        phoneField= std::make_unique<TextField>(font, "Phone *",       sf::FloatRect(fx+410.f, fy+180.f, 370.f, 38.f));
        phoneField->setNumericOnly(true); phoneField->setMaxLength(10);
        yearField = std::make_unique<Dropdown> (font, "Year *",        sf::FloatRect(fx,       fy+270.f, 370.f, 38.f),
            {"1","2","3","4"});

        saveBtn  = std::make_unique<Button>(font, "Save",  sf::FloatRect(fx,       fy+340.f, 170.f, 44.f), Theme::saveBtn(),  Theme::saveBtnHover());
        clearBtn2= std::make_unique<Button>(font, "Clear", sf::FloatRect(fx+190.f, fy+340.f, 170.f, 44.f), Theme::clearBtn(), sf::Color(100,112,128));
        saveBtn->setOnClick([this]  { onSave(); });
        clearBtn2->setOnClick([this]{ clearForm(); });

        // Table for View mode
        table = std::make_unique<Table>(font, sf::FloatRect(260.f, 130.f, 1010.f, 480.f),
            std::vector<std::string>{"Roll No","Name","Department","Year","Hall","Email","Phone"},
            std::vector<float>{110.f,160.f,120.f,55.f,100.f,210.f,120.f});

        statusText.setFont(font); statusText.setCharacterSize(14);
        statusText.setPosition(270.f, 620.f);

        setMode(StudentMode::Add);
    }

    void handleMouseMove(sf::Vector2f m) override {
        sidebar.handleMouseMove(m);
        saveBtn->handleMouseMove(m); clearBtn2->handleMouseMove(m);
    }
    void handleClick(sf::Vector2f m) override {
        if (sidebar.handleClick(m)) return;
        if (mode == StudentMode::View) { table->handleScroll(0); return; }
        idField->handleClick(m); nameField->handleClick(m);
        hallField->handleClick(m); emailField->handleClick(m); phoneField->handleClick(m);
        deptField->handleClick(m); yearField->handleClick(m);
        if (saveBtn->handleClick(m)) return;
        clearBtn2->handleClick(m);
    }
    void handleTextEntered(sf::Uint32 u) override {
        if (mode == StudentMode::View) return;
        idField->handleTextEntered(u); nameField->handleTextEntered(u);
        hallField->handleTextEntered(u); emailField->handleTextEntered(u);
        phoneField->handleTextEntered(u);
    }
    void handleScroll(float d) override { if (mode==StudentMode::View) table->handleScroll(d); }
    void onShow() override { refreshTable(); statusMsg.clear(); }

    void draw(sf::RenderWindow& win) override {
        win.clear(Theme::contentBg());
        header.draw(win);
        sidebar.draw(win);

        // Content card
        sf::RectangleShape card({1010.f, 570.f});
        card.setPosition(258.f, 88.f);
        card.setFillColor(sf::Color::White);
        card.setOutlineThickness(1.f);
        card.setOutlineColor(sf::Color(210,225,245));
        win.draw(card);

        // Card sub-header
        sf::RectangleShape subhdr({1010.f, 64.f});
        subhdr.setPosition(258.f, 88.f);
        subhdr.setFillColor(sf::Color(240,246,255));
        win.draw(subhdr);

        sf::Text cardTitle; cardTitle.setFont(font);
        std::string modeStr = (mode==StudentMode::Add?"Add Student":
                               mode==StudentMode::View?"View Students":
                               mode==StudentMode::Search?"Search Student":
                               mode==StudentMode::Update?"Update Student":"Delete Student");
        cardTitle.setString(modeStr);
        cardTitle.setCharacterSize(17); cardTitle.setStyle(sf::Text::Bold);
        cardTitle.setFillColor(Theme::brandNavy());
        cardTitle.setPosition(316.f, 98.f);
        win.draw(cardTitle);

        sf::Text cardSub; cardSub.setFont(font);
        cardSub.setString(mode==StudentMode::Add ? "Enter student details to register a new student" :
                          mode==StudentMode::View ? "All registered students" : "Enter student ID to proceed");
        cardSub.setCharacterSize(12); cardSub.setFillColor(Theme::textMid());
        cardSub.setPosition(316.f, 122.f);
        win.draw(cardSub);

        if (mode == StudentMode::View) {
            table->draw(win);
        } else {
            idField->draw(win); hallField->draw(win);
            nameField->draw(win); emailField->draw(win);
            deptField->draw(win); phoneField->draw(win);
            yearField->draw(win);
            saveBtn->draw(win); clearBtn2->draw(win);
        }

        if (!statusMsg.empty()) {
            statusText.setString(statusMsg);
            statusText.setFillColor(statusOk ? Theme::successText() : Theme::errorText());
            win.draw(statusText);
        }
    }

private:
    sf::Font& font;
    StudentManager& mgr;
    StudentMode mode = StudentMode::Add;
    HeaderBar header;
    Sidebar sidebar;

    sf::Text statusText;
    std::unique_ptr<TextField> idField, nameField, hallField, emailField, phoneField;
    std::unique_ptr<Dropdown>  deptField, yearField;
    std::unique_ptr<Button>    saveBtn, clearBtn2;
    std::unique_ptr<Table>     table;
    std::string statusMsg; bool statusOk = false;

    void setMode(StudentMode m) {
        mode = m; clearForm();
        sidebar.setActive((int)m);
        if (m==StudentMode::View) refreshTable();
    }
    void clearForm() {
        idField->clear(); nameField->clear(); hallField->clear();
        emailField->clear(); phoneField->clear();
        deptField->setValue("AIML"); yearField->setValue("1");
        statusMsg.clear();
    }
    void refreshTable() {
        std::vector<std::vector<std::string>> rows;
        for (auto& s : mgr.viewStudents())
            rows.push_back({s.rollNo,s.name,s.department,std::to_string(s.year),s.hallNumber,s.email,s.phone});
        table->setRows(rows);
    }
    Student formToStudent() const {
        Student s;
        s.rollNo=idField->getValue(); s.name=nameField->getValue();
        s.department=deptField->getValue();
        s.year=yearField->getValue().empty()?1:std::stoi(yearField->getValue());
        s.hallNumber=hallField->getValue(); s.email=emailField->getValue(); s.phone=phoneField->getValue();
        return s;
    }
    void onSave() {
        if (idField->getValue().empty()) { statusOk=false; statusMsg="Student ID is required."; return; }
        if (mode==StudentMode::Add) {
            bool ok=mgr.addStudent(formToStudent());
            statusOk=ok; statusMsg=ok?"Student added successfully.":"Student ID already exists.";
        } else if (mode==StudentMode::Update) {
            bool ok=mgr.updateStudent(formToStudent());
            statusOk=ok; statusMsg=ok?"Student updated.":"Student ID not found.";
        } else if (mode==StudentMode::Search) {
            auto* s=mgr.searchStudent(idField->getValue());
            if(s){ nameField->setValue(s->name); deptField->setValue(s->department);
                   yearField->setValue(std::to_string(s->year)); hallField->setValue(s->hallNumber);
                   emailField->setValue(s->email); phoneField->setValue(s->phone);
                   statusOk=true; statusMsg="Student found.";
            } else { statusOk=false; statusMsg="No student found."; }
        } else if (mode==StudentMode::Delete) {
            bool ok=mgr.deleteStudent(idField->getValue());
            statusOk=ok; statusMsg=ok?"Student deleted.":"Student ID not found.";
            if(ok) clearForm();
        }
    }
};
