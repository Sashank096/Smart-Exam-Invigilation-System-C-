#pragma once
#include <SFML/Graphics.hpp>

// Every screen (Login, Dashboard, Student Management, ...) implements this
// so App can hold them uniformly and dispatch events without knowing which
// concrete screen is active.
class Screen {
public:
    virtual ~Screen() = default;
    virtual void handleMouseMove(sf::Vector2f mouse) {}
    virtual void handleClick(sf::Vector2f mouse) {}
    virtual void handleTextEntered(sf::Uint32 unicode) {}
    virtual void handleScroll(float delta) {}
    virtual void onShow() {} // called every time the screen becomes active - good place to refresh tables
    virtual void draw(sf::RenderWindow& win) = 0;
};
