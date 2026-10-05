#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <functional>
#include "Theme.h"

// Deep blue sidebar matching PPT: nav buttons + "Learn•Grow•Succeed" at bottom
class Sidebar {
public:
    struct Item { std::string label; std::function<void()> onClick; };

    Sidebar(sf::Font& font, std::vector<Item> items, std::function<void()> onBack)
        : font(font) {
        float y = 96.f;
        for (size_t i = 0; i < items.size(); i++) {
            labels.push_back(items[i].label);
            callbacks.push_back(items[i].onClick);
            y += 58.f;
        }
        backCb = onBack;

        learnText.setFont(font);
        learnText.setString("Learn  *  Grow  *  Succeed");
        learnText.setCharacterSize(12);
        learnText.setFillColor(sf::Color(180, 210, 255));
        learnText.setPosition(20.f, 650.f);
    }

    void handleMouseMove(sf::Vector2f m) { hovered = hitTest(m); }

    bool handleClick(sf::Vector2f m) {
        int idx = hitTest(m);
        if (idx >= 0 && idx < (int)callbacks.size()) {
            active = idx; callbacks[idx](); return true;
        }
        // Back button
        if (m.x >= 10 && m.x <= 240 && m.y >= backY() && m.y <= backY() + 44) {
            if (backCb) backCb(); return true;
        }
        return false;
    }

    void setActive(int idx) { active = idx; }

    void draw(sf::RenderWindow& win) {
        // Deep blue sidebar background
        sf::RectangleShape bg({250.f, 720.f});
        bg.setPosition(0.f, 0.f);
        bg.setFillColor(sf::Color(13, 71, 161));
        win.draw(bg);

        float y = 96.f;
        for (int i = 0; i < (int)labels.size(); i++) {
            bool isActive = (i == active);
            bool isHov    = (i == hovered);

            sf::RectangleShape btn({240.f, 46.f});
            btn.setPosition(5.f, y);
            btn.setFillColor(isActive ? sf::Color(21, 101, 192) :
                             isHov    ? sf::Color(25, 118, 210) :
                                        sf::Color(13, 71, 161));
            win.draw(btn);

            // Active indicator stripe
            if (isActive) {
                sf::RectangleShape stripe({4.f, 46.f});
                stripe.setPosition(5.f, y);
                stripe.setFillColor(sf::Color(100, 181, 246));
                win.draw(stripe);
            }

            sf::Text lbl;
            lbl.setFont(font);
            lbl.setString(labels[i]);
            lbl.setCharacterSize(14);
            lbl.setFillColor(sf::Color::White);
            lbl.setPosition(20.f, y + 14.f);
            win.draw(lbl);

            // Chevron for active
            if (isActive) {
                sf::Text chev;
                chev.setFont(font);
                chev.setString(">");
                chev.setCharacterSize(14);
                chev.setFillColor(sf::Color(180, 210, 255));
                chev.setPosition(220.f, y + 14.f);
                win.draw(chev);
            }

            y += 58.f;
        }

        // Back to Dashboard button
        float by = backY();
        sf::RectangleShape backBtn({240.f, 44.f});
        backBtn.setPosition(5.f, by);
        backBtn.setFillColor(sf::Color(21, 101, 192));
        win.draw(backBtn);
        sf::Text backLbl;
        backLbl.setFont(font);
        backLbl.setString("< Back to Dashboard");
        backLbl.setCharacterSize(13);
        backLbl.setFillColor(sf::Color(200, 220, 255));
        backLbl.setPosition(14.f, by + 14.f);
        win.draw(backLbl);

        // "Learn * Grow * Succeed"
        win.draw(learnText);
    }

private:
    sf::Font& font;
    std::vector<std::string> labels;
    std::vector<std::function<void()>> callbacks;
    std::function<void()> backCb;
    sf::Text learnText;
    int active = 0;
    int hovered = -1;

    float backY() const { return 96.f + labels.size() * 58.f + 20.f; }

    int hitTest(sf::Vector2f m) {
        if (m.x < 5 || m.x > 245) return -1;
        float y = 96.f;
        for (int i = 0; i < (int)labels.size(); i++) {
            if (m.y >= y && m.y <= y + 46.f) return i;
            y += 58.f;
        }
        return -1;
    }
};
