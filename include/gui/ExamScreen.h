#pragma once
#include "Screen.h"
#include "Widgets.h"
#include "Theme.h"
#include "HeaderBar.h"
#include "Sidebar.h"
#include "../Exam.h"
#include <functional>

enum class ExamMode { Add, View, Search, Update, Delete };

class ExamScreen : public Screen {
public:
    ExamScreen(sf::Font& font, ExamManager& mgr, std::function<void()> onBack)
        : font(font), mgr(mgr),
          header(font, "Exam Schedule", "Manage exam schedule and subject information"),
          sidebar(font, {
              {"Add Exam",    [this]{ setMode(ExamMode::Add);    }},
              {"View Exams",  [this]{ setMode(ExamMode::View);   }},
              {"Search Exam", [this]{ setMode(ExamMode::Search); }},
              {"Update Exam", [this]{ setMode(ExamMode::Update); }},
              {"Delete Exam", [this]{ setMode(ExamMode::Delete); }},
          }, std::move(onBack)) {

        float fx=270.f,fy=168.f;
        idField   =std::make_unique<TextField>(font,"Exam ID :",   sf::FloatRect(fx,      fy,      370.f,38.f));
        subjField =std::make_unique<TextField>(font,"Subject :",   sf::FloatRect(fx+410.f,fy,      370.f,38.f));
        dateField =std::make_unique<TextField>(font,"Date :",      sf::FloatRect(fx,      fy+90.f, 370.f,38.f));
        timeField =std::make_unique<TextField>(font,"Time :",      sf::FloatRect(fx+410.f,fy+90.f, 370.f,38.f));
        hallField =std::make_unique<TextField>(font,"Hall ID :",   sf::FloatRect(fx,      fy+180.f,370.f,38.f));
        deptField =std::make_unique<Dropdown> (font,"Department :",sf::FloatRect(fx+410.f,fy+180.f,370.f,38.f),
            {"AIML","CSE","ECE","MECH","DS","CIVIL","EEE","IT"});

        saveBtn  =std::make_unique<Button>(font,"Save",  sf::FloatRect(fx,      fy+260.f,170.f,44.f),Theme::primaryBtn(),Theme::primaryBtnHover());
        clearBtn2=std::make_unique<Button>(font,"Clear", sf::FloatRect(fx+190.f,fy+260.f,170.f,44.f),Theme::clearBtn(),sf::Color(100,112,128));
        saveBtn->setOnClick([this]{onSave();}); clearBtn2->setOnClick([this]{clearForm();});

        table=std::make_unique<Table>(font,sf::FloatRect(260.f,130.f,1010.f,480.f),
            std::vector<std::string>{"Exam ID","Subject","Date","Time","Hall ID","Department"},
            std::vector<float>{120.f,200.f,130.f,110.f,110.f,160.f});

        statusText.setFont(font); statusText.setCharacterSize(14); statusText.setPosition(270.f,580.f);
        setMode(ExamMode::Add);
    }

    void handleMouseMove(sf::Vector2f m) override { sidebar.handleMouseMove(m); saveBtn->handleMouseMove(m); clearBtn2->handleMouseMove(m); }
    void handleClick(sf::Vector2f m) override {
        if(sidebar.handleClick(m)) return;
        if(mode==ExamMode::View) return;
        idField->handleClick(m); subjField->handleClick(m); dateField->handleClick(m);
        timeField->handleClick(m); hallField->handleClick(m); deptField->handleClick(m);
        if(saveBtn->handleClick(m)) return; clearBtn2->handleClick(m);
    }
    void handleTextEntered(sf::Uint32 u) override {
        if(mode==ExamMode::View) return;
        idField->handleTextEntered(u); subjField->handleTextEntered(u);
        dateField->handleTextEntered(u); timeField->handleTextEntered(u); hallField->handleTextEntered(u);
    }
    void handleScroll(float d) override { if(mode==ExamMode::View) table->handleScroll(d); }
    void onShow() override { refreshTable(); statusMsg.clear(); }

    void draw(sf::RenderWindow& win) override {
        win.clear(Theme::contentBg()); header.draw(win); sidebar.draw(win);
        sf::RectangleShape card({1010.f,570.f}); card.setPosition(258.f,88.f);
        card.setFillColor(sf::Color::White); card.setOutlineThickness(1.f); card.setOutlineColor(sf::Color(210,225,245)); win.draw(card);
        sf::RectangleShape sub({1010.f,64.f}); sub.setPosition(258.f,88.f); sub.setFillColor(sf::Color(240,246,255)); win.draw(sub);
        sf::Text t; t.setFont(font);
        t.setString(mode==ExamMode::Add?"Add Exam":mode==ExamMode::View?"View Exams":
                    mode==ExamMode::Search?"Search Exam":mode==ExamMode::Update?"Update Exam":"Delete Exam");
        t.setCharacterSize(17); t.setStyle(sf::Text::Bold); t.setFillColor(Theme::brandNavy()); t.setPosition(316.f,98.f); win.draw(t);
        if(mode==ExamMode::View){ table->draw(win); }
        else { idField->draw(win); subjField->draw(win); dateField->draw(win); timeField->draw(win); hallField->draw(win); deptField->draw(win); saveBtn->draw(win); clearBtn2->draw(win); }
        if(!statusMsg.empty()){ statusText.setString(statusMsg); statusText.setFillColor(statusOk?Theme::successText():Theme::errorText()); win.draw(statusText); }
    }

private:
    sf::Font& font; ExamManager& mgr; ExamMode mode=ExamMode::Add;
    HeaderBar header; Sidebar sidebar; sf::Text statusText;
    std::unique_ptr<TextField> idField,subjField,dateField,timeField,hallField;
    std::unique_ptr<Dropdown>  deptField;
    std::unique_ptr<Button>    saveBtn,clearBtn2;
    std::unique_ptr<Table>     table;
    std::string statusMsg; bool statusOk=false;

    void setMode(ExamMode m){ mode=m; clearForm(); sidebar.setActive((int)m); if(m==ExamMode::View) refreshTable(); }
    void clearForm(){ idField->clear(); subjField->clear(); dateField->clear(); timeField->clear(); hallField->clear(); deptField->setValue("AIML"); statusMsg.clear(); }
    void refreshTable(){
        std::vector<std::vector<std::string>> rows;
        for(auto& e:mgr.viewExams()) rows.push_back({e.examId,e.subject,e.date,e.time,e.hallId,e.department});
        table->setRows(rows);
    }
    void onSave(){
        if(idField->getValue().empty()){statusOk=false;statusMsg="Exam ID required.";return;}
        Exam e; e.examId=idField->getValue(); e.subject=subjField->getValue();
        e.date=dateField->getValue(); e.time=timeField->getValue();
        e.hallId=hallField->getValue(); e.department=deptField->getValue();
        if(mode==ExamMode::Add){bool ok=mgr.addExam(e);statusOk=ok;statusMsg=ok?"Exam added.":"ID exists.";}
        else if(mode==ExamMode::Update){bool ok=mgr.updateExam(e);statusOk=ok;statusMsg=ok?"Updated.":"Not found.";}
        else if(mode==ExamMode::Search){
            auto* s=mgr.searchExam(idField->getValue());
            if(s){subjField->setValue(s->subject);dateField->setValue(s->date);timeField->setValue(s->time);hallField->setValue(s->hallId);deptField->setValue(s->department);statusOk=true;statusMsg="Found.";}
            else{statusOk=false;statusMsg="Not found.";}
        } else if(mode==ExamMode::Delete){bool ok=mgr.deleteExam(idField->getValue());statusOk=ok;statusMsg=ok?"Deleted.":"Not found.";if(ok)clearForm();}
    }
};
