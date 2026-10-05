#pragma once
#include <SFML/Graphics.hpp>
#include "Theme.h"

// White header bar matching PPT: logo+name left | module title+subtitle center | Admin right
class HeaderBar {
public:
    HeaderBar(sf::Font& font, const std::string& moduleTitle, const std::string& moduleSub)
        : font(font) {
        logoLoaded = logoTex.loadFromFile("assets/logo_small.png");
        if (logoLoaded) {
            logoSpr.setTexture(logoTex);
            auto sz = logoTex.getSize();
            float scale = 56.f / sz.y;
            logoSpr.setScale(scale, scale);
            logoSpr.setPosition(12.f, 10.f);
        }

        adityaT.setFont(font);
        adityaT.setString("ADITYA");
        adityaT.setCharacterSize(13);
        adityaT.setStyle(sf::Text::Bold);
        adityaT.setFillColor(Theme::brandOrange());

        uniT.setFont(font);
        uniT.setString("UNIVERSITY");
        uniT.setCharacterSize(11);
        uniT.setStyle(sf::Text::Bold);
        uniT.setFillColor(Theme::brandNavy());

        modTitle.setFont(font);
        modTitle.setString(moduleTitle);
        modTitle.setCharacterSize(18);
        modTitle.setStyle(sf::Text::Bold);
        modTitle.setFillColor(Theme::brandNavy());
        modTitle.setPosition(310.f, 12.f);

        modSub.setFont(font);
        modSub.setString(moduleSub);
        modSub.setCharacterSize(12);
        modSub.setFillColor(Theme::textMid());
        modSub.setPosition(310.f, 40.f);

        adminT.setFont(font);
        adminT.setString("Admin");
        adminT.setCharacterSize(14);
        adminT.setFillColor(Theme::brandNavy());
        adminT.setPosition(1160.f, 26.f);
    }

    void draw(sf::RenderWindow& win) {
        // White header bar
        sf::RectangleShape bar({1280.f, 76.f});
        bar.setPosition(0.f, 0.f);
        bar.setFillColor(sf::Color::White);
        win.draw(bar);

        // Bottom border
        sf::RectangleShape border({1280.f, 1.5f});
        border.setPosition(0.f, 76.f);
        border.setFillColor(sf::Color(210, 225, 245));
        win.draw(border);

        // Vertical divider between logo and title
        sf::RectangleShape div({2.f, 50.f});
        div.setPosition(288.f, 13.f);
        div.setFillColor(sf::Color(210, 225, 245));
        win.draw(div);

        if (logoLoaded) {
            win.draw(logoSpr);
            float lx = logoSpr.getPosition().x + logoTex.getSize().x * logoSpr.getScale().x + 8.f;
            adityaT.setPosition(lx, 14.f);
            uniT.setPosition(lx, 36.f);
            win.draw(adityaT);
            win.draw(uniT);
        }

        win.draw(modTitle);
        win.draw(modSub);

        // Admin circle icon
        sf::CircleShape adminCircle(18.f);
        adminCircle.setFillColor(sf::Color(25, 118, 210));
        adminCircle.setPosition(1138.f, 20.f);
        win.draw(adminCircle);

        // "A" in circle
        sf::Text at;
        at.setFont(font);
        at.setString("A");
        at.setCharacterSize(16);
        at.setStyle(sf::Text::Bold);
        at.setFillColor(sf::Color::White);
        at.setPosition(1148.f, 22.f);
        win.draw(at);

        win.draw(adminT);
    }

private:
    sf::Font& font;
    sf::Texture logoTex;
    sf::Sprite  logoSpr;
    bool        logoLoaded = false;
    sf::Text    adityaT, uniT, modTitle, modSub, adminT;
};
