#pragma once
#include "Screen.h"
#include "Widgets.h"
#include "Theme.h"
#include "HeaderBar.h"
#include "Sidebar.h"
#include "../ExamHall.h"
#include <functional>

enum class HallMode { Add, View, Search, Update, Delete };

class HallScreen : public Screen {
public:
    HallScreen(sf::Font& font, ExamHallManager& mgr, std::function<void()> onBack)
        : font(font), mgr(mgr),
          header(font, "Exam Hall Management", "Manage | Add | View | Update | Delete Halls"),
          sidebar(font, {
              {"Add Hall",    [this]{ setMode(HallMode::Add);    }},
              {"View Halls",  [this]{ setMode(HallMode::View);   }},
              {"Search Hall", [this]{ setMode(HallMode::Search); }},
              {"Update Hall", [this]{ setMode(HallMode::Update); }},
              {"Delete Hall", [this]{ setMode(HallMode::Delete); }},
          }, std::move(onBack)) {

        float fx=270.f, fy=168.f;
        idField  = std::make_unique<TextField>(font,"Hall ID :",    sf::FloatRect(fx,      fy,      370.f,38.f));
        nameField= std::make_unique<TextField>(font,"Hall Name :",  sf::FloatRect(fx+410.f,fy,      370.f,38.f));
        capField = std::make_unique<TextField>(font,"Capacity :",   sf::FloatRect(fx,      fy+90.f, 370.f,38.f));
        capField->setNumericOnly(true); capField->setMaxLength(6);
        locField = std::make_unique<TextField>(font,"Location :",   sf::FloatRect(fx+410.f,fy+90.f, 370.f,38.f));
        availField=std::make_unique<Dropdown> (font,"Availability :",sf::FloatRect(fx,     fy+180.f,370.f,38.f),
            {"Available","Not Available"});

        saveBtn  = std::make_unique<Button>(font,"Save",  sf::FloatRect(fx,      fy+260.f,170.f,44.f),Theme::primaryBtn(),Theme::primaryBtnHover());
        clearBtn2= std::make_unique<Button>(font,"Clear", sf::FloatRect(fx+190.f,fy+260.f,170.f,44.f),Theme::clearBtn(),sf::Color(100,112,128));
        saveBtn->setOnClick([this]{onSave();}); clearBtn2->setOnClick([this]{clearForm();});

        table=std::make_unique<Table>(font,sf::FloatRect(260.f,130.f,1010.f,480.f),
            std::vector<std::string>{"Hall ID","Hall Name","Capacity","Location","Availability"},
            std::vector<float>{130.f,200.f,110.f,280.f,150.f});

        statusText.setFont(font); statusText.setCharacterSize(14); statusText.setPosition(270.f,580.f);
        setMode(HallMode::Add);
    }

    void handleMouseMove(sf::Vector2f m) override { sidebar.handleMouseMove(m); saveBtn->handleMouseMove(m); clearBtn2->handleMouseMove(m); }
    void handleClick(sf::Vector2f m) override {
        if(sidebar.handleClick(m)) return;
        if(mode==HallMode::View) return;
        idField->handleClick(m); nameField->handleClick(m); capField->handleClick(m); locField->handleClick(m); availField->handleClick(m);
        if(saveBtn->handleClick(m)) return; clearBtn2->handleClick(m);
    }
    void handleTextEntered(sf::Uint32 u) override {
        if(mode==HallMode::View) return;
        idField->handleTextEntered(u); nameField->handleTextEntered(u); capField->handleTextEntered(u); locField->handleTextEntered(u);
    }
    void handleScroll(float d) override { if(mode==HallMode::View) table->handleScroll(d); }
    void onShow() override { refreshTable(); statusMsg.clear(); }

    void draw(sf::RenderWindow& win) override {
        win.clear(Theme::contentBg()); header.draw(win); sidebar.draw(win);
        sf::RectangleShape card({1010.f,570.f}); card.setPosition(258.f,88.f);
        card.setFillColor(sf::Color::White); card.setOutlineThickness(1.f); card.setOutlineColor(sf::Color(210,225,245)); win.draw(card);
        sf::RectangleShape sub({1010.f,64.f}); sub.setPosition(258.f,88.f); sub.setFillColor(sf::Color(240,246,255)); win.draw(sub);
        sf::Text t; t.setFont(font);
        t.setString(mode==HallMode::Add?"Add Hall":mode==HallMode::View?"View Halls":
                    mode==HallMode::Search?"Search Hall":mode==HallMode::Update?"Update Hall":"Delete Hall");
        t.setCharacterSize(17); t.setStyle(sf::Text::Bold); t.setFillColor(Theme::brandNavy()); t.setPosition(316.f,98.f); win.draw(t);
        if(mode==HallMode::View){ table->draw(win); }
        else { idField->draw(win); nameField->draw(win); capField->draw(win); locField->draw(win); availField->draw(win); saveBtn->draw(win); clearBtn2->draw(win); }
        if(!statusMsg.empty()){ statusText.setString(statusMsg); statusText.setFillColor(statusOk?Theme::successText():Theme::errorText()); win.draw(statusText); }
    }

private:
    sf::Font& font; ExamHallManager& mgr; HallMode mode=HallMode::Add;
    HeaderBar header; Sidebar sidebar; sf::Text statusText;
    std::unique_ptr<TextField> idField,nameField,capField,locField;
    std::unique_ptr<Dropdown>  availField;
    std::unique_ptr<Button>    saveBtn,clearBtn2;
    std::unique_ptr<Table>     table;
    std::string statusMsg; bool statusOk=false;

    void setMode(HallMode m){ mode=m; clearForm(); sidebar.setActive((int)m); if(m==HallMode::View) refreshTable(); }
    void clearForm(){ idField->clear(); nameField->clear(); capField->clear(); locField->clear(); availField->setValue("Available"); statusMsg.clear(); }
    void refreshTable(){
        std::vector<std::vector<std::string>> rows;
        for(auto& h:mgr.viewHalls()) rows.push_back({h.hallId,h.hallName,std::to_string(h.capacity),h.location,h.availability});
        table->setRows(rows);
    }
    void onSave(){
        if(idField->getValue().empty()){statusOk=false;statusMsg="Hall ID required.";return;}
        ExamHall h; h.hallId=idField->getValue(); h.hallName=nameField->getValue();
        h.capacity=capField->getValue().empty()?0:std::stoi(capField->getValue());
        h.location=locField->getValue(); h.availability=availField->getValue();
        if(mode==HallMode::Add){bool ok=mgr.addHall(h);statusOk=ok;statusMsg=ok?"Hall added.":"ID exists.";}
        else if(mode==HallMode::Update){bool ok=mgr.updateHall(h);statusOk=ok;statusMsg=ok?"Updated.":"Not found.";}
        else if(mode==HallMode::Search){
            auto* s=mgr.searchHall(idField->getValue());
            if(s){nameField->setValue(s->hallName);capField->setValue(std::to_string(s->capacity));locField->setValue(s->location);availField->setValue(s->availability);statusOk=true;statusMsg="Found.";}
            else{statusOk=false;statusMsg="Not found.";}
        } else if(mode==HallMode::Delete){bool ok=mgr.deleteHall(idField->getValue());statusOk=ok;statusMsg=ok?"Deleted.":"Not found.";if(ok)clearForm();}
    }
};
