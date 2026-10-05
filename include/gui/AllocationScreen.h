#pragma once
#include "Screen.h"
#include "Widgets.h"
#include "Theme.h"
#include "HeaderBar.h"
#include "Sidebar.h"
#include "../Allocation.h"
#include <functional>

enum class AllocMode { Add, View, Check, Remove };

class AllocationScreen : public Screen {
public:
    AllocationScreen(sf::Font& font, AllocationManager& mgr, std::function<void()> onBack)
        : font(font), mgr(mgr),
          header(font, "Invigilator Allocation", "Exam -> Hall -> Invigilator -> Allocation -> Conflict Check -> Assignment"),
          sidebar(font, {
              {"Allocate",        [this]{ setMode(AllocMode::Add);    }},
              {"View Allocations",[this]{ setMode(AllocMode::View);   }},
              {"Check Conflict",  [this]{ setMode(AllocMode::Check);  }},
              {"Remove Allocation",[this]{ setMode(AllocMode::Remove);}},
          }, std::move(onBack)) {

        float fx=270.f,fy=168.f;
        examField  =std::make_unique<TextField>(font,"Exam ID :",         sf::FloatRect(fx,      fy,      370.f,38.f));
        hallField  =std::make_unique<TextField>(font,"Hall ID :",         sf::FloatRect(fx+410.f,fy,      370.f,38.f));
        invField   =std::make_unique<TextField>(font,"Invigilator ID :",  sf::FloatRect(fx,      fy+90.f, 370.f,38.f));
        allocIdField=std::make_unique<TextField>(font,"Allocation ID :",  sf::FloatRect(fx+410.f,fy+90.f, 370.f,38.f));

        saveBtn  =std::make_unique<Button>(font,"Save",    sf::FloatRect(fx,      fy+170.f,170.f,44.f),Theme::primaryBtn(),Theme::primaryBtnHover());
        clearBtn2=std::make_unique<Button>(font,"Clear",   sf::FloatRect(fx+190.f,fy+170.f,170.f,44.f),Theme::clearBtn(),sf::Color(100,112,128));
        saveBtn->setOnClick([this]{onSave();}); clearBtn2->setOnClick([this]{clearForm();});

        table=std::make_unique<Table>(font,sf::FloatRect(260.f,130.f,1010.f,480.f),
            std::vector<std::string>{"Alloc ID","Exam ID","Hall ID","Invigilator ID","Status"},
            std::vector<float>{130.f,150.f,130.f,200.f,200.f});

        statusText.setFont(font); statusText.setCharacterSize(14); statusText.setPosition(270.f,520.f);
        setMode(AllocMode::Add);
    }

    void handleMouseMove(sf::Vector2f m) override { sidebar.handleMouseMove(m); saveBtn->handleMouseMove(m); clearBtn2->handleMouseMove(m); }
    void handleClick(sf::Vector2f m) override {
        if(sidebar.handleClick(m)) return;
        if(mode==AllocMode::View) return;
        examField->handleClick(m); hallField->handleClick(m); invField->handleClick(m); allocIdField->handleClick(m);
        if(saveBtn->handleClick(m)) return; clearBtn2->handleClick(m);
    }
    void handleTextEntered(sf::Uint32 u) override {
        if(mode==AllocMode::View) return;
        examField->handleTextEntered(u); hallField->handleTextEntered(u);
        invField->handleTextEntered(u); allocIdField->handleTextEntered(u);
    }
    void handleScroll(float d) override { if(mode==AllocMode::View) table->handleScroll(d); }
    void onShow() override { refreshTable(); statusMsg.clear(); }

    void draw(sf::RenderWindow& win) override {
        win.clear(Theme::contentBg()); header.draw(win); sidebar.draw(win);
        sf::RectangleShape card({1010.f,570.f}); card.setPosition(258.f,88.f);
        card.setFillColor(sf::Color::White); card.setOutlineThickness(1.f); card.setOutlineColor(sf::Color(210,225,245)); win.draw(card);
        sf::RectangleShape sub({1010.f,64.f}); sub.setPosition(258.f,88.f); sub.setFillColor(sf::Color(240,246,255)); win.draw(sub);
        sf::Text t; t.setFont(font);
        t.setString(mode==AllocMode::Add?"Allocate Invigilator":mode==AllocMode::View?"View Allocations":
                    mode==AllocMode::Check?"Check Conflict":"Remove Allocation");
        t.setCharacterSize(17); t.setStyle(sf::Text::Bold); t.setFillColor(Theme::brandNavy()); t.setPosition(316.f,98.f); win.draw(t);
        if(mode==AllocMode::View){ table->draw(win); }
        else { examField->draw(win); hallField->draw(win); invField->draw(win); allocIdField->draw(win); saveBtn->draw(win); clearBtn2->draw(win); }
        if(!statusMsg.empty()){ statusText.setString(statusMsg); statusText.setFillColor(statusOk?Theme::successText():Theme::errorText()); win.draw(statusText); }
    }

private:
    sf::Font& font; AllocationManager& mgr; AllocMode mode=AllocMode::Add;
    HeaderBar header; Sidebar sidebar; sf::Text statusText;
    std::unique_ptr<TextField> examField,hallField,invField,allocIdField;
    std::unique_ptr<Button>    saveBtn,clearBtn2;
    std::unique_ptr<Table>     table;
    std::string statusMsg; bool statusOk=false;

    void setMode(AllocMode m){ mode=m; clearForm(); sidebar.setActive((int)m); if(m==AllocMode::View) refreshTable(); }
    void clearForm(){ examField->clear(); hallField->clear(); invField->clear(); allocIdField->clear(); statusMsg.clear(); }
    void refreshTable(){
        std::vector<std::vector<std::string>> rows;
        for(auto& a:mgr.viewAllocations()) rows.push_back({a.allocId,a.examId,a.hallId,a.invigilatorId,a.status});
        table->setRows(rows);
    }
    void onSave(){
        if(mode==AllocMode::Add){
            if(examField->getValue().empty()||invField->getValue().empty()){statusOk=false;statusMsg="Exam ID and Invigilator ID required.";return;}
            Allocation a; a.examId=examField->getValue(); a.hallId=hallField->getValue();
            a.invigilatorId=invField->getValue(); a.status="Assigned";
            bool ok=mgr.allocateInvigilator(a); statusOk=ok; statusMsg=ok?"Allocated successfully.":"Conflict detected or ID exists.";
        } else if(mode==AllocMode::Check){
            bool conflict=mgr.checkConflict(invField->getValue());
            statusOk=!conflict; statusMsg=conflict?"Conflict detected for this invigilator.":"No conflict found.";
        } else if(mode==AllocMode::Remove){
            if(allocIdField->getValue().empty()){statusOk=false;statusMsg="Allocation ID required.";return;}
            bool ok=mgr.removeAllocation(allocIdField->getValue()); statusOk=ok; statusMsg=ok?"Removed.":"Not found."; if(ok)clearForm();
        }
    }
};
