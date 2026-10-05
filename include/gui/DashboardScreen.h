#pragma once
#include "Screen.h"
#include "Widgets.h"
#include "Theme.h"
#include "../Report.h"
#include <functional>
#include <vector>

enum class ScreenId { Login, Dashboard, Student, Invigilator, Hall, Exam, Allocation, Reports };

class DashboardScreen : public Screen {
public:
    DashboardScreen(sf::Font& font, ReportManager& report,
                    std::function<void(ScreenId)> navigate, std::function<void()> logout)
        : font(font), report(report), navigate(std::move(navigate)), logout(std::move(logout)) {

        logoLoaded = logoTex.loadFromFile("assets/logo_small.png");
        if(logoLoaded){
            logoSpr.setTexture(logoTex);
            auto sz=logoTex.getSize(); float scale=56.f/sz.y;
            logoSpr.setScale(scale,scale); logoSpr.setPosition(16.f,10.f);
        }

        adityaT.setFont(font); adityaT.setString("ADITYA"); adityaT.setCharacterSize(14);
        adityaT.setStyle(sf::Text::Bold); adityaT.setFillColor(Theme::brandOrange());

        uniT.setFont(font); uniT.setString("UNIVERSITY"); uniT.setCharacterSize(12);
        uniT.setStyle(sf::Text::Bold); uniT.setFillColor(Theme::brandNavy());

        welcomeT.setFont(font); welcomeT.setString("Welcome, Admin");
        welcomeT.setCharacterSize(14); welcomeT.setFillColor(Theme::textMid());

        // Nav card buttons — 3x2 grid
        struct CardDef{ std::string title; std::string sub; ScreenId id; };
        std::vector<CardDef> defs={
            {"Student Management",    "Manage | Add | View | Update | Delete", ScreenId::Student},
            {"Invigilator Management","Manage invigilator details",             ScreenId::Invigilator},
            {"Exam Hall Management",  "Manage | Add | View | Update | Delete", ScreenId::Hall},
            {"Exam Schedule",         "Manage exam dates and subjects",         ScreenId::Exam},
            {"Invigilator Allocation","Assign invigilators to halls",           ScreenId::Allocation},
            {"Reports & Dashboard",   "Summary and export",                     ScreenId::Reports},
        };

        float cW=380.f,cH=120.f,gX=30.f,gY=20.f,sX=290.f,sY=120.f;
        for(size_t i=0;i<defs.size();i++){
            int col=i%3,row=i/3;
            sf::FloatRect bounds(sX+col*(cW+gX),sY+row*(cH+gY),cW,cH);
            auto btn=std::make_unique<Button>(font,defs[i].title,bounds,
                sf::Color(13,71,161),sf::Color(21,101,192));
            ScreenId tid=defs[i].id; auto nav=this->navigate;
            btn->setOnClick([nav,tid](){ if(nav) nav(tid); });
            cards.push_back(std::move(btn));
            cardSubs.push_back(defs[i].sub);
            cardBounds.push_back(bounds);
        }

        logoutBtn=std::make_unique<Button>(font,"Logout",sf::FloatRect(1140.f,24.f,110.f,36.f),
            sf::Color(192,57,43),sf::Color(150,40,30));
        logoutBtn->setOnClick([this](){ if(this->logout) this->logout(); });

        summaryT.setFont(font); summaryT.setCharacterSize(13);
        summaryT.setFillColor(Theme::textMid()); summaryT.setPosition(290.f,400.f);
    }

    void handleMouseMove(sf::Vector2f m) override {
        for(auto& c:cards) c->handleMouseMove(m);
        logoutBtn->handleMouseMove(m);
    }
    void handleClick(sf::Vector2f m) override {
        for(auto& c:cards) if(c->handleClick(m)) return;
        logoutBtn->handleClick(m);
    }
    void onShow() override {
        auto s=report.showSummary();
        summaryT.setString(
            "Students: "+std::to_string(s.totalStudents)+
            "   |   Invigilators: "+std::to_string(s.totalInvigilators)+
            "   |   Halls: "+std::to_string(s.totalHalls)+
            "   |   Exams: "+std::to_string(s.totalExams)+
            "   |   Allocations: "+std::to_string(s.totalAllocations));
    }

    void draw(sf::RenderWindow& win) override {
        win.clear(sf::Color(232,240,253));

        // White header
        sf::RectangleShape hdr({1280.f,76.f}); hdr.setPosition(0.f,0.f);
        hdr.setFillColor(sf::Color::White); win.draw(hdr);
        sf::RectangleShape hdrBorder({1280.f,1.5f}); hdrBorder.setPosition(0.f,76.f);
        hdrBorder.setFillColor(sf::Color(210,225,245)); win.draw(hdrBorder);

        // Logo
        if(logoLoaded){ win.draw(logoSpr); float lx=logoSpr.getPosition().x+logoTex.getSize().x*logoSpr.getScale().x+8.f;
            adityaT.setPosition(lx,14.f); uniT.setPosition(lx,36.f); win.draw(adityaT); win.draw(uniT); }

        // "Welcome, Admin" center
        auto wb=welcomeT.getLocalBounds(); welcomeT.setPosition(640.f-wb.width/2.f,30.f); win.draw(welcomeT);

        logoutBtn->draw(win);

        // Cards
        for(size_t i=0;i<cards.size();i++){
            cards[i]->draw(win);
            // Sub text on card
            sf::Text sub; sub.setFont(font); sub.setString(cardSubs[i]);
            sub.setCharacterSize(11); sub.setFillColor(sf::Color(180,210,255));
            sub.setPosition(cardBounds[i].left+16.f, cardBounds[i].top+70.f);
            win.draw(sub);
        }

        win.draw(summaryT);
    }

private:
    sf::Font& font; ReportManager& report;
    std::function<void(ScreenId)> navigate; std::function<void()> logout;
    sf::Texture logoTex; sf::Sprite logoSpr; bool logoLoaded=false;
    sf::Text adityaT,uniT,welcomeT,summaryT;
    std::vector<std::unique_ptr<Button>> cards;
    std::vector<std::string> cardSubs;
    std::vector<sf::FloatRect> cardBounds;
    std::unique_ptr<Button> logoutBtn;
};
