#pragma once
#include "Screen.h"
#include "Widgets.h"
#include "Theme.h"
#include "HeaderBar.h"
#include "Sidebar.h"
#include "../Invigilator.h"
#include <functional>

enum class InvMode { Add, View, Search, Update, Delete };

class InvigilatorScreen : public Screen {
public:
    InvigilatorScreen(sf::Font& font, InvigilatorManager& mgr, std::function<void()> onBack)
        : font(font), mgr(mgr),
          header(font, "Invigilator Management", "Manage and maintain invigilator details"),
          sidebar(font, {
              {"Add Invigilator",    [this]{ setMode(InvMode::Add);    }},
              {"View Invigilators",  [this]{ setMode(InvMode::View);   }},
              {"Search Invigilator", [this]{ setMode(InvMode::Search); }},
              {"Update Invigilator", [this]{ setMode(InvMode::Update); }},
              {"Delete Invigilator", [this]{ setMode(InvMode::Delete); }},
          }, std::move(onBack)) {

        float fx = 270.f, fy = 168.f;
        idField   = std::make_unique<TextField>(font, "Invigilator ID :", sf::FloatRect(fx,       fy,       370.f, 38.f));
        nameField = std::make_unique<TextField>(font, "Name :",           sf::FloatRect(fx,       fy+90.f,  370.f, 38.f));
        deptField = std::make_unique<Dropdown> (font, "Department :",     sf::FloatRect(fx,       fy+180.f, 370.f, 38.f),
            {"AIML","CSE","ECE","MECH","DS","CIVIL","EEE","IT"});
        phoneField= std::make_unique<TextField>(font, "Phone :",          sf::FloatRect(fx,       fy+270.f, 370.f, 38.f));
        phoneField->setNumericOnly(true); phoneField->setMaxLength(10);
        emailField= std::make_unique<TextField>(font, "Email :",          sf::FloatRect(fx+410.f, fy+90.f,  370.f, 38.f));
        availField= std::make_unique<Dropdown> (font, "Availability :",   sf::FloatRect(fx,       fy+360.f, 370.f, 38.f),
            {"Available","Not Available"});

        saveBtn  = std::make_unique<Button>(font, "Save",  sf::FloatRect(fx,       fy+430.f, 170.f, 44.f), Theme::primaryBtn(), Theme::primaryBtnHover());
        clearBtn2= std::make_unique<Button>(font, "Clear", sf::FloatRect(fx+190.f, fy+430.f, 170.f, 44.f), Theme::clearBtn(), sf::Color(100,112,128));
        saveBtn->setOnClick([this] { onSave(); });
        clearBtn2->setOnClick([this]{ clearForm(); });

        table = std::make_unique<Table>(font, sf::FloatRect(260.f,130.f,1010.f,480.f),
            std::vector<std::string>{"ID","Name","Department","Phone","Email","Availability"},
            std::vector<float>{120.f,180.f,130.f,130.f,240.f,140.f});

        statusText.setFont(font); statusText.setCharacterSize(14);
        statusText.setPosition(270.f, 680.f);
        setMode(InvMode::Add);
    }

    void handleMouseMove(sf::Vector2f m) override {
        sidebar.handleMouseMove(m);
        saveBtn->handleMouseMove(m); clearBtn2->handleMouseMove(m);
    }
    void handleClick(sf::Vector2f m) override {
        if (sidebar.handleClick(m)) return;
        if (mode==InvMode::View) return;
        idField->handleClick(m); nameField->handleClick(m);
        phoneField->handleClick(m); emailField->handleClick(m);
        deptField->handleClick(m); availField->handleClick(m);
        if (saveBtn->handleClick(m)) return;
        clearBtn2->handleClick(m);
    }
    void handleTextEntered(sf::Uint32 u) override {
        if (mode==InvMode::View) return;
        idField->handleTextEntered(u); nameField->handleTextEntered(u);
        phoneField->handleTextEntered(u); emailField->handleTextEntered(u);
    }
    void handleScroll(float d) override { if (mode==InvMode::View) table->handleScroll(d); }
    void onShow() override { refreshTable(); statusMsg.clear(); }

    void draw(sf::RenderWindow& win) override {
        win.clear(Theme::contentBg());
        header.draw(win); sidebar.draw(win);

        sf::RectangleShape card({1010.f,570.f}); card.setPosition(258.f,88.f);
        card.setFillColor(sf::Color::White); card.setOutlineThickness(1.f);
        card.setOutlineColor(sf::Color(210,225,245)); win.draw(card);

        sf::RectangleShape subhdr({1010.f,64.f}); subhdr.setPosition(258.f,88.f);
        subhdr.setFillColor(sf::Color(240,246,255)); win.draw(subhdr);

        sf::Text t; t.setFont(font);
        t.setString(mode==InvMode::Add?"Add Invigilator":mode==InvMode::View?"View Invigilators":
                    mode==InvMode::Search?"Search Invigilator":mode==InvMode::Update?"Update Invigilator":"Delete Invigilator");
        t.setCharacterSize(17); t.setStyle(sf::Text::Bold);
        t.setFillColor(Theme::brandNavy()); t.setPosition(316.f,98.f); win.draw(t);

        sf::Text sub; sub.setFont(font);
        sub.setString(mode==InvMode::View?"All registered invigilators":"Enter invigilator details");
        sub.setCharacterSize(12); sub.setFillColor(Theme::textMid()); sub.setPosition(316.f,122.f); win.draw(sub);

        if (mode==InvMode::View) { table->draw(win); }
        else {
            idField->draw(win); nameField->draw(win); deptField->draw(win);
            phoneField->draw(win); emailField->draw(win); availField->draw(win);
            saveBtn->draw(win); clearBtn2->draw(win);
        }
        if (!statusMsg.empty()) {
            statusText.setString(statusMsg);
            statusText.setFillColor(statusOk?Theme::successText():Theme::errorText());
            win.draw(statusText);
        }
    }

private:
    sf::Font& font; InvigilatorManager& mgr; InvMode mode=InvMode::Add;
    HeaderBar header; Sidebar sidebar; sf::Text statusText;
    std::unique_ptr<TextField> idField,nameField,phoneField,emailField;
    std::unique_ptr<Dropdown>  deptField,availField;
    std::unique_ptr<Button>    saveBtn,clearBtn2;
    std::unique_ptr<Table>     table;
    std::string statusMsg; bool statusOk=false;

    void setMode(InvMode m){ mode=m; clearForm(); sidebar.setActive((int)m); if(m==InvMode::View) refreshTable(); }
    void clearForm(){ idField->clear(); nameField->clear(); phoneField->clear(); emailField->clear();
                      deptField->setValue("AIML"); availField->setValue("Available"); statusMsg.clear(); }
    void refreshTable(){
        std::vector<std::vector<std::string>> rows;
        for(auto& i:mgr.viewInvigilators())
            rows.push_back({i.id,i.name,i.department,i.phone,i.email,i.availability});
        table->setRows(rows);
    }
    void onSave(){
        if(idField->getValue().empty()){statusOk=false;statusMsg="ID required.";return;}
        Invigilator inv; inv.id=idField->getValue(); inv.name=nameField->getValue();
        inv.department=deptField->getValue(); inv.phone=phoneField->getValue();
        inv.email=emailField->getValue(); inv.availability=availField->getValue();
        if(mode==InvMode::Add){ bool ok=mgr.addInvigilator(inv); statusOk=ok; statusMsg=ok?"Added successfully.":"ID already exists."; }
        else if(mode==InvMode::Update){ bool ok=mgr.updateInvigilator(inv); statusOk=ok; statusMsg=ok?"Updated.":"ID not found."; }
        else if(mode==InvMode::Search){
            auto* s=mgr.searchInvigilator(idField->getValue());
            if(s){ nameField->setValue(s->name); deptField->setValue(s->department);
                   phoneField->setValue(s->phone); emailField->setValue(s->email);
                   availField->setValue(s->availability); statusOk=true; statusMsg="Found.";
            } else { statusOk=false; statusMsg="Not found."; }
        } else if(mode==InvMode::Delete){ bool ok=mgr.deleteInvigilator(idField->getValue()); statusOk=ok; statusMsg=ok?"Deleted.":"Not found."; if(ok)clearForm(); }
    }
};
