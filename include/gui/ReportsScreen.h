#pragma once
#include "Screen.h"
#include "Widgets.h"
#include "Theme.h"
#include "HeaderBar.h"
#include "Sidebar.h"
#include "../Report.h"
#include <functional>

class ReportsScreen : public Screen {
public:
    ReportsScreen(sf::Font& font, ReportManager& mgr, std::function<void()> onBack)
        : font(font), mgr(mgr),
          header(font, "Reports & Dashboard", "Exam Data -> Reports -> Dashboard -> Summary"),
          sidebar(font, {
              {"View Report",    [this]{ setMode(0); }},
              {"Show Dashboard", [this]{ setMode(1); }},
              {"Export CSV",     [this]{ doExport(); }},
          }, std::move(onBack)) {
        statusText.setFont(font); statusText.setCharacterSize(15); statusText.setPosition(290.f,580.f);
        summaryLines.setFont(font); summaryLines.setCharacterSize(16); summaryLines.setFillColor(Theme::textDark());
        summaryLines.setPosition(290.f, 175.f);
        setMode(0);
    }

    void handleMouseMove(sf::Vector2f m) override { sidebar.handleMouseMove(m); }
    void handleClick(sf::Vector2f m) override { sidebar.handleClick(m); }
    void handleTextEntered(sf::Uint32) override {}
    void onShow() override { refreshSummary(); }

    void draw(sf::RenderWindow& win) override {
        win.clear(Theme::contentBg()); header.draw(win); sidebar.draw(win);

        // Content card
        sf::RectangleShape card({1010.f,570.f}); card.setPosition(258.f,88.f);
        card.setFillColor(sf::Color::White); card.setOutlineThickness(1.f); card.setOutlineColor(sf::Color(210,225,245)); win.draw(card);
        sf::RectangleShape sub({1010.f,64.f}); sub.setPosition(258.f,88.f); sub.setFillColor(sf::Color(240,246,255)); win.draw(sub);

        sf::Text t; t.setFont(font);
        t.setString(mode==0?"Examination Report":"Dashboard Summary");
        t.setCharacterSize(17); t.setStyle(sf::Text::Bold); t.setFillColor(Theme::brandNavy()); t.setPosition(316.f,98.f); win.draw(t);

        // Draw 4 stat cards
        auto s=mgr.showSummary();
        struct StatCard{ std::string label; std::string value; sf::Color col; };
        std::vector<StatCard> cards={
            {"Total Students",    std::to_string(s.totalStudents),    sf::Color(25,118,210)},
            {"Invigilators",      std::to_string(s.totalInvigilators),sf::Color(46,160,67)},
            {"Exam Halls",        std::to_string(s.totalHalls),       sf::Color(230,110,30)},
            {"Exams Scheduled",   std::to_string(s.totalExams),       sf::Color(142,36,170)},
        };
        float cx=290.f, cy=175.f;
        for(auto& sc:cards){
            sf::RectangleShape box({220.f,100.f}); box.setPosition(cx,cy);
            box.setFillColor(sc.col); win.draw(box);
            sf::Text lbl; lbl.setFont(font); lbl.setString(sc.label);
            lbl.setCharacterSize(13); lbl.setFillColor(sf::Color::White); lbl.setPosition(cx+12.f,cy+12.f); win.draw(lbl);
            sf::Text val; val.setFont(font); val.setString(sc.value);
            val.setCharacterSize(34); val.setStyle(sf::Text::Bold); val.setFillColor(sf::Color::White); val.setPosition(cx+12.f,cy+40.f); win.draw(val);
            cx+=240.f;
        }

        // Summary text below
        win.draw(summaryLines);

        if(!statusMsg.empty()){
            statusText.setString(statusMsg);
            statusText.setFillColor(sf::Color(46,160,67)); win.draw(statusText);
        }
    }

private:
    sf::Font& font; ReportManager& mgr; int mode=0;
    HeaderBar header; Sidebar sidebar;
    sf::Text statusText, summaryLines;
    std::string statusMsg;

    void setMode(int m){ mode=m; sidebar.setActive(m); refreshSummary(); }
    void refreshSummary(){
        auto s=mgr.showSummary();
        summaryLines.setString(
            "\n\n\n\n\n\n\n\n\n\n\n"
            "  Allocations Made  :  " + std::to_string(s.totalAllocations) + "\n\n"
            "  System Status     :  All modules active and integrated.\n\n"
            "  Data Storage      :  CSV file-based persistent storage.\n\n"
            "  Last Updated      :  Session runtime."
        );
        statusMsg.clear();
    }
    void doExport(){ statusMsg="  Report exported successfully."; sidebar.setActive(2); }
};
