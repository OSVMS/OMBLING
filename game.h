#pragma once

#include "carte.h"
#include <SFML/Graphics.hpp>
#include <optional>
#include <random>
#include <string>

class Game {
public:
    bool initialize(char* argv[]);
    void run();

private:
    enum class Ecran {
        Menu,
        Jeu
    };

    static constexpr int kDeckSize = 52;

    sf::RenderWindow window{sf::VideoMode({800, 560}), "OMBLING"};
    sf::View vue{sf::FloatRect({0.f, 0.f}, {800.f, 560.f})};

    sf::Texture textureFond;
    std::optional<sf::Sprite> spriteFond;

    sf::Texture textureBtnTirer;
    std::optional<sf::Sprite> spriteBtnTirer;

    sf::Font font;

    Ecran ecran = Ecran::Menu;
    bool afficherMessageParam = false;
    int selectionMenu = 0;
    int selectionJeu = 0;

    const sf::Color couleurBaseLancer{28, 116, 58};
    const sf::Color couleurBaseParam{40, 90, 160};
    const sf::Color couleurBaseQuitter{160, 40, 40};

    std::optional<sf::Text> txtTitreMenu;
    sf::RectangleShape btnMenuLancer{{320.f, 62.f}};
    sf::RectangleShape btnMenuParam{{320.f, 62.f}};
    sf::RectangleShape btnMenuQuitter{{320.f, 62.f}};

    std::optional<sf::Text> txtMenuLancer;
    std::optional<sf::Text> txtMenuParam;
    std::optional<sf::Text> txtMenuQuitter;
    std::optional<sf::Text> txtParamInfo;

    sf::RectangleShape btnQuitter{{160.f, 55.f}};
    std::optional<sf::Text> txtQuitter;
    std::optional<sf::Text> txtTitre;

    carte paquet[kDeckSize];
    std::mt19937 rng{std::random_device{}()};
    int index = 0;
    carte* cartes[2] = {nullptr, nullptr};
    bool fin = false;

    std::string baseDir;

    std::string asset(const std::string& rel) const;
    bool loadAssets();
    void setupUi();
    void reinitialiserPartie();

    void handleEvent(const sf::Event& event);
    void handleResized(const sf::Event::Resized& resized);
    void handleKeyPressed(const sf::Event::KeyPressed& key);
    void handleMousePressed(const sf::Event::MouseButtonPressed& click);

    void renderFrame();
    void renderMenu();
    void renderJeu();
    void updateMenuHighlight();
    void updateJeuHighlight();
};
