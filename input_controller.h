#pragma once

#include <SFML/Window/Event.hpp>

class Game;

class InputController {
public:
    explicit InputController(Game& game);

    void handleEvent(const sf::Event& event);
    void handleResized(const sf::Event::Resized& resized);
    void handleKeyPressed(const sf::Event::KeyPressed& key);
    void handleMousePressed(const sf::Event::MouseButtonPressed& click);
    void handleTextEntered(const sf::Event::TextEntered& text);

private:
    Game& game;
};
