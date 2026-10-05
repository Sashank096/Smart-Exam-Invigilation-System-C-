#pragma once
#include "Screen.h"
#include "Widgets.h"
#include "Theme.h"
#include "../Admin.h"
#include <functional>

class LoginScreen : public Screen {
public:
    LoginScreen(sf::Font& font, Admin& admin, std::function<void()> onLoginSuccess)
        : font(font), admin(admin), onSuccess(std::move(onLoginSuccess)) {

        // Load Aditya University logo (centered at top)
        logoLoaded = logoTex.loadFromFile("assets/logo_small.png");
        if (logoLoaded) {
            logoSpr.setTexture(logoTex);
            auto sz = logoTex.getSize();
            float scale = 72.f / sz.y;
            logoSpr.setScale(scale, scale);
            float logoW = sz.x * scale;
            logoSpr.setPosition(640.f - logoW / 2.f - 10.f, 42.f);
        }

        // "ADITYA" in orange, "UNIVERSITY" in navy — stacked right of logo
        adityaText.setFont(font);
        adityaText.setString("ADITYA");
        adityaText.setCharacterSize(22);
        adityaText.setStyle(sf::Text::Bold);
        adityaText.setFillColor(Theme::brandOrange());

        uniText.setFont(font);
        uniText.setString("UNIVERSITY");
        uniText.setCharacterSize(18);
        uniText.setStyle(sf::Text::Bold);
        uniText.setFillColor(Theme::brandNavy());

        // System title
        titleText.setFont(font);
        titleText.setString("SMART EXAM INVIGILATION SYSTEM");
        titleText.setCharacterSize(22);
        titleText.setStyle(sf::Text::Bold);
        titleText.setFillColor(Theme::brandNavy());

        // Subtitle
        subText.setFont(font);
        subText.setString("Smart Exam Invigilation System - Login");
        subText.setCharacterSize(14);
        subText.setFillColor(Theme::textMid());

        // Form fields — plain labels above, white rounded box
        userField = std::make_unique<TextField>(font, "Username :", sf::FloatRect(415, 358, 450, 40));
        passField = std::make_unique<TextField>(font, "Password :", sf::FloatRect(415, 440, 450, 40));
        passField->setPassword(true);

        // Login button (blue, full width left half)
        loginBtn = std::make_unique<Button>(font, "Login",
            sf::FloatRect(415, 510, 210, 44),
            Theme::primaryBtn(), Theme::primaryBtnHover());
        loginBtn->setOnClick([this]() { attemptLogin(); });

        // Reset button (grey, right half)
        resetBtn = std::make_unique<Button>(font, "Reset",
            sf::FloatRect(655, 510, 210, 44),
            Theme::clearBtn(), sf::Color(100, 112, 128));
        resetBtn->setOnClick([this]() {
            userField->clear(); passField->clear(); statusMsg = "";
        });

        statusText.setFont(font);
        statusText.setCharacterSize(14);
        statusText.setPosition(415, 566);
    }

    void handleMouseMove(sf::Vector2f m) override {
        loginBtn->handleMouseMove(m);
        resetBtn->handleMouseMove(m);
    }
    void handleClick(sf::Vector2f m) override {
        userField->handleClick(m);
        passField->handleClick(m);
        if (loginBtn->handleClick(m)) return;
        resetBtn->handleClick(m);
    }
    void handleTextEntered(sf::Uint32 u) override {
        userField->handleTextEntered(u);
        passField->handleTextEntered(u);
    }

    void draw(sf::RenderWindow& win) override {
        // Light blue-grey background
        win.clear(sf::Color(232, 240, 253));

        // Faint watermark circle (bottom-left) — like PPT background
        sf::CircleShape wm(220.f);
        wm.setFillColor(sf::Color(200, 215, 240, 60));
        wm.setPosition(-80.f, 320.f);
        win.draw(wm);

        // Wave shape bottom-right
        sf::RectangleShape wave({600.f, 160.f});
        wave.setFillColor(sf::Color(180, 210, 245, 80));
        wave.setPosition(750.f, 560.f);
        win.draw(wave);

        // ── Logo centered at top ─────────────────────────────────────
        if (logoLoaded) {
            win.draw(logoSpr);
            // "ADITYA" right of logo
            float lx = logoSpr.getPosition().x + logoTex.getSize().x * logoSpr.getScale().x + 12.f;
            float ly = logoSpr.getPosition().y + 8.f;
            adityaText.setPosition(lx, ly);
            uniText.setPosition(lx, ly + 30.f);
            win.draw(adityaText);
            win.draw(uniText);
        } else {
            // Fallback: text only
            adityaText.setCharacterSize(28);
            centerX(adityaText, 640.f, 55.f);
            win.draw(adityaText);
        }

        // System title
        centerX(titleText, 640.f, 148.f);
        win.draw(titleText);

        // Subtitle
        centerX(subText, 640.f, 182.f);
        win.draw(subText);

        // ── White login card ─────────────────────────────────────────
        sf::RectangleShape card({490.f, 280.f});
        card.setPosition(395.f, 310.f);
        card.setFillColor(sf::Color::White);
        card.setOutlineThickness(1.f);
        card.setOutlineColor(sf::Color(210, 225, 245));
        // Rounded look via slight corner shadow
        win.draw(card);

        userField->draw(win);
        passField->draw(win);
        loginBtn->draw(win);
        resetBtn->draw(win);

        if (!statusMsg.empty()) {
            statusText.setString(statusMsg);
            statusText.setFillColor(loginOk ? Theme::successText() : Theme::errorText());
            win.draw(statusText);
        }
    }

    void onShow() override {
        userField->clear(); passField->clear(); statusMsg.clear();
    }

private:
    sf::Font& font;
    Admin& admin;
    std::function<void()> onSuccess;

    sf::Texture logoTex;
    sf::Sprite  logoSpr;
    bool        logoLoaded = false;

    sf::Text adityaText, uniText, titleText, subText, statusText;
    std::unique_ptr<TextField> userField, passField;
    std::unique_ptr<Button> loginBtn, resetBtn;
    std::string statusMsg;
    bool loginOk = false;

    void centerX(sf::Text& t, float cx, float y) {
        auto b = t.getLocalBounds();
        t.setPosition(cx - b.width / 2.f - b.left, y);
    }
    void attemptLogin() {
        if (admin.login(userField->getValue(), passField->getValue())) {
            loginOk = true; statusMsg = "";
            if (onSuccess) onSuccess();
        } else {
            loginOk = false;
            statusMsg = "  Invalid username or password.";
        }
    }
};
