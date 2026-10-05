#include <SFML/Graphics.hpp>
#include <memory>
#include <map>
#include "../include/Admin.h"
#include "../include/Student.h"
#include "../include/Invigilator.h"
#include "../include/ExamHall.h"
#include "../include/Exam.h"
#include "../include/Allocation.h"
#include "../include/Report.h"
#include "../include/gui/Theme.h"
#include "../include/gui/Screen.h"
#include "../include/gui/LoginScreen.h"
#include "../include/gui/DashboardScreen.h"
#include "../include/gui/StudentScreen.h"
#include "../include/gui/InvigilatorScreen.h"
#include "../include/gui/HallScreen.h"
#include "../include/gui/ExamScreen.h"
#include "../include/gui/AllocationScreen.h"
#include "../include/gui/ReportsScreen.h"

int main() {
    // --- Load font first: every widget needs it, so this must succeed before anything else. ---
    sf::Font font;
    if (!Theme::loadDefaultFont(font)) {
        // No console window in a GUI app necessarily, but this still helps if run from a terminal.
        fprintf(stderr,
            "Could not find a system font (tried Segoe UI, Arial, Tahoma).\n"
            "Drop a .ttf file at assets/font.ttf next to the executable and re-run.\n");
        return 1;
    }

    // --- Core data managers - all file-backed, matching the class diagram. ---
    Admin admin("data/admin.csv");
    StudentManager students("data/students.csv");
    InvigilatorManager invigilators("data/invigilators.csv");
    ExamHallManager halls("data/halls.csv");
    ExamManager exams("data/exams.csv");
    AllocationManager allocations("data/allocations.csv", exams);
    ReportManager report(students, invigilators, halls, exams, allocations);

    // --- Window ---
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Smart Exam Invigilation System",
                             sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);

    // --- Screens ---
    ScreenId current = ScreenId::Login;
    std::map<ScreenId, std::unique_ptr<Screen>> screens;

    auto navigateTo = [&](ScreenId id) {
        current = id;
        screens[current]->onShow();
    };

    screens[ScreenId::Login] = std::make_unique<LoginScreen>(font, admin, [&]() { navigateTo(ScreenId::Dashboard); });
    screens[ScreenId::Dashboard] = std::make_unique<DashboardScreen>(font, report,
        [&](ScreenId target) { navigateTo(target); },
        [&]() { navigateTo(ScreenId::Login); });
    screens[ScreenId::Student] = std::make_unique<StudentScreen>(font, students, [&]() { navigateTo(ScreenId::Dashboard); });
    screens[ScreenId::Invigilator] = std::make_unique<InvigilatorScreen>(font, invigilators, [&]() { navigateTo(ScreenId::Dashboard); });
    screens[ScreenId::Hall] = std::make_unique<HallScreen>(font, halls, [&]() { navigateTo(ScreenId::Dashboard); });
    screens[ScreenId::Exam] = std::make_unique<ExamScreen>(font, exams, [&]() { navigateTo(ScreenId::Dashboard); });
    screens[ScreenId::Allocation] = std::make_unique<AllocationScreen>(font, exams, halls, invigilators, allocations,
        [&]() { navigateTo(ScreenId::Dashboard); });
    screens[ScreenId::Reports] = std::make_unique<ReportsScreen>(font, report, [&]() { navigateTo(ScreenId::Dashboard); });

    screens[current]->onShow();

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            } else if (event.type == sf::Event::MouseMoved) {
                screens[current]->handleMouseMove(
                    window.mapPixelToCoords({event.mouseMove.x, event.mouseMove.y}));
            } else if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    screens[current]->handleClick(
                        window.mapPixelToCoords({event.mouseButton.x, event.mouseButton.y}));
                }
            } else if (event.type == sf::Event::TextEntered) {
                screens[current]->handleTextEntered(event.text.unicode);
            } else if (event.type == sf::Event::MouseWheelScrolled) {
                screens[current]->handleScroll(event.mouseWheelScroll.delta);
            }
        }

        screens[current]->draw(window);
        window.display();
    }

    return 0;
}
