#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <functional>
#include "Theme.h"

// SFML has no built-in UI controls, so this is a small hand-rolled widget
// set: Button, TextField, Dropdown and Table. Each widget owns its own
// drawing and hit-testing; screens just lay them out and wire callbacks.

class Button {
public:
    Button(sf::Font& font, std::string text, sf::FloatRect bounds,
           sf::Color bg, sf::Color bgHover)
        : label(text, font, 16), normalColor(bg), hoverColor(bgHover) {
        shape.setPosition(bounds.left, bounds.top);
        shape.setSize({bounds.width, bounds.height});
        shape.setFillColor(bg);
        label.setFillColor(sf::Color::White);
        centerLabel(bounds);
    }

    void setOnClick(std::function<void()> cb) { onClick = std::move(cb); }

    void handleMouseMove(sf::Vector2f mouse) {
        hovered = shape.getGlobalBounds().contains(mouse);
        shape.setFillColor(hovered ? hoverColor : normalColor);
    }

    // Returns true if this click was consumed.
    bool handleClick(sf::Vector2f mouse) {
        if (shape.getGlobalBounds().contains(mouse)) {
            if (onClick) onClick();
            return true;
        }
        return false;
    }

    void draw(sf::RenderWindow& win) { win.draw(shape); win.draw(label); }

    void setText(const std::string& t) {
        label.setString(t);
        auto b = shape.getGlobalBounds();
        centerLabel(b);
    }

private:
    sf::RectangleShape shape;
    sf::Text label;
    sf::Color normalColor, hoverColor;
    bool hovered = false;
    std::function<void()> onClick;

    void centerLabel(sf::FloatRect bounds) {
        auto lb = label.getLocalBounds();
        label.setPosition(bounds.left + (bounds.width - lb.width) / 2.f - lb.left,
                           bounds.top + (bounds.height - lb.height) / 2.f - lb.top);
    }
};

class TextField {
public:
    TextField(sf::Font& font, std::string labelText, sf::FloatRect bounds)
        : caption(labelText, font, 14), value("", font, 16) {
        box.setPosition(bounds.left, bounds.top);
        box.setSize({bounds.width, bounds.height});
        box.setFillColor(Theme::textFieldBg());
        box.setOutlineThickness(1.5f);
        box.setOutlineColor(Theme::textFieldBorder());
        caption.setFillColor(Theme::textDark());
        caption.setPosition(bounds.left, bounds.top - 20);
        value.setFillColor(Theme::textDark());
        value.setPosition(bounds.left + 8, bounds.top + (bounds.height - 18) / 2.f);
        bounds_ = bounds;
    }

    void setNumericOnly(bool n) { numericOnly = n; }
    void setMaxLength(int n) { maxLen = n; }
    void setPassword(bool p) { isPassword = p; }

    void handleClick(sf::Vector2f mouse) {
        active = box.getGlobalBounds().contains(mouse);
        box.setOutlineColor(active ? Theme::textFieldActive() : Theme::textFieldBorder());
    }

    void handleTextEntered(sf::Uint32 unicode) {
        if (!active) return;
        if (unicode == 8) { // backspace
            if (!text.empty()) text.pop_back();
        } else if (unicode == 13 || unicode == 9) {
            // enter/tab - ignored here, screens can poll value instead
        } else if (unicode >= 32 && unicode < 127) {
            char c = static_cast<char>(unicode);
            if (numericOnly && !(isdigit(c) || c == '.' || c == '-')) return;
            if (static_cast<int>(text.size()) < maxLen) text += c;
        }
        value.setString(isPassword ? std::string(text.size(), '*') : text);
    }

    void setValue(const std::string& v) { text = v; value.setString(v); }
    std::string getValue() const { return text; }
    void clear() { text.clear(); value.setString(""); }

    void draw(sf::RenderWindow& win) {
        win.draw(box);
        win.draw(caption);
        win.draw(value);
        if (active) {
            // simple blinking-less caret at end of text
            auto vb = value.getGlobalBounds();
            sf::RectangleShape caret({1.5f, box.getSize().y - 10.f});
            caret.setPosition(vb.left + vb.width + 2, box.getPosition().y + 5);
            caret.setFillColor(Theme::textDark());
            win.draw(caret);
        }
    }

    sf::FloatRect getBounds() const { return bounds_; }

private:
    sf::RectangleShape box;
    sf::Text caption, value;
    std::string text;
    bool active = false;
    bool numericOnly = false;
    bool isPassword = false;
    int maxLen = 40;
    sf::FloatRect bounds_;
};

// Simplified dropdown: click cycles to the next option. Shows current
// value plus a small arrow. Avoids building a full popup-list system
// while still behaving like a dropdown for data entry purposes.
class Dropdown {
public:
    Dropdown(sf::Font& font, std::string labelText, sf::FloatRect bounds,
             std::vector<std::string> opts)
        : caption(labelText, font, 14), value("", font, 16), options(std::move(opts)) {
        box.setPosition(bounds.left, bounds.top);
        box.setSize({bounds.width, bounds.height});
        box.setFillColor(Theme::textFieldBg());
        box.setOutlineThickness(1.5f);
        box.setOutlineColor(Theme::textFieldBorder());
        caption.setFillColor(Theme::textDark());
        caption.setPosition(bounds.left, bounds.top - 20);
        arrow.setString(sf::String(L"\u25BC"));
        arrow.setFont(font);
        arrow.setCharacterSize(12);
        arrow.setFillColor(Theme::textDark());
        arrow.setPosition(bounds.left + bounds.width - 22, bounds.top + bounds.height / 2.f - 8);
        value.setFillColor(Theme::textDark());
        value.setPosition(bounds.left + 8, bounds.top + (bounds.height - 18) / 2.f);
        if (!options.empty()) value.setString(options[0]);
        bounds_ = bounds;
    }

    bool handleClick(sf::Vector2f mouse) {
        if (!box.getGlobalBounds().contains(mouse)) return false;
        if (options.empty()) return true;
        index = (index + 1) % options.size();
        value.setString(options[index]);
        return true;
    }

    std::string getValue() const { return options.empty() ? "" : options[index]; }

    void setValue(const std::string& v) {
        for (size_t i = 0; i < options.size(); i++) {
            if (options[i] == v) { index = static_cast<int>(i); value.setString(v); return; }
        }
    }

    void setOptions(std::vector<std::string> opts) {
        options = std::move(opts);
        index = 0;
        value.setString(options.empty() ? "" : options[0]);
    }

    void draw(sf::RenderWindow& win) {
        win.draw(box); win.draw(caption); win.draw(value); win.draw(arrow);
    }

    sf::FloatRect getBounds() const { return bounds_; }

private:
    sf::RectangleShape box;
    sf::Text caption, value, arrow;
    std::vector<std::string> options;
    int index = 0;
    sf::FloatRect bounds_;
};

// Simple table: header row + striped data rows, with a small mouse-wheel
// scroll offset for lists longer than the visible area.
class Table {
public:
    Table(sf::Font& font, sf::FloatRect bounds, std::vector<std::string> headers,
          std::vector<float> colWidths)
        : font(font), bounds(bounds), headers(std::move(headers)), colWidths(std::move(colWidths)) {}

    void setRows(std::vector<std::vector<std::string>> r) { rows = std::move(r); scrollOffset = 0; }

    void handleScroll(float delta) {
        int maxOffset = std::max(0, static_cast<int>(rows.size()) - visibleRows());
        scrollOffset -= static_cast<int>(delta);
        if (scrollOffset < 0) scrollOffset = 0;
        if (scrollOffset > maxOffset) scrollOffset = maxOffset;
    }

    int visibleRows() const { return static_cast<int>(bounds.height / rowHeight) - 1; }

    void draw(sf::RenderWindow& win) {
        float y = bounds.top;
        // header
        sf::RectangleShape headerRow({bounds.width, rowHeight});
        headerRow.setPosition(bounds.left, y);
        headerRow.setFillColor(Theme::tableHeaderBg());
        win.draw(headerRow);
        float x = bounds.left;
        for (size_t i = 0; i < headers.size(); i++) {
            sf::Text t(headers[i], font, 14);
            t.setStyle(sf::Text::Bold);
            t.setFillColor(sf::Color::White);
            t.setPosition(x + 6, y + 6);
            win.draw(t);
            x += colWidths[i];
        }
        y += rowHeight;

        int shown = 0;
        for (int r = scrollOffset; r < static_cast<int>(rows.size()) && shown < visibleRows(); r++, shown++) {
            sf::RectangleShape rowBg({bounds.width, rowHeight});
            rowBg.setPosition(bounds.left, y);
            rowBg.setFillColor(shown % 2 == 0 ? Theme::tableRowA() : Theme::tableRowB());
            win.draw(rowBg);
            x = bounds.left;
            for (size_t c = 0; c < rows[r].size() && c < colWidths.size(); c++) {
                sf::Text t(rows[r][c], font, 13);
                t.setFillColor(Theme::textDark());
                t.setPosition(x + 6, y + 6);
                win.draw(t);
                x += colWidths[c];
            }
            y += rowHeight;
        }

        if (rows.empty()) {
            sf::Text empty("No records yet.", font, 14);
            empty.setFillColor(sf::Color(120, 120, 120));
            empty.setPosition(bounds.left + 8, y + 10);
            win.draw(empty);
        }
    }

    // Returns the row index (into the full `rows` vector) clicked, or -1.
    int rowClicked(sf::Vector2f mouse) const {
        if (mouse.x < bounds.left || mouse.x > bounds.left + bounds.width) return -1;
        if (mouse.y < bounds.top + rowHeight) return -1;
        int rowIdx = static_cast<int>((mouse.y - bounds.top - rowHeight) / rowHeight);
        int actual = scrollOffset + rowIdx;
        if (actual >= 0 && actual < static_cast<int>(rows.size())) return actual;
        return -1;
    }

private:
    sf::Font& font;
    sf::FloatRect bounds;
    std::vector<std::string> headers;
    std::vector<float> colWidths;
    std::vector<std::vector<std::string>> rows;
    float rowHeight = 30.f;
    int scrollOffset = 0;
};
