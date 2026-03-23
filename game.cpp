#include "game.h"

#include "ui_helpers.h"
#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <optional>

bool Game::initialize(char* argv[]) {
    baseDir = std::filesystem::canonical(argv[0]).parent_path().string();

    window.setFramerateLimit(60);
    window.setView(vue);

    if (!loadAssets()) {
        return false;
    }

    setupUi();
    return true;
}

std::string Game::asset(const std::string& rel) const {
    return (std::filesystem::path(baseDir) / rel).string();
}

bool Game::loadAssets() {
    if (!textureFond.loadFromFile(asset("asset/fond/Fond table.png"))) {
        return false;
    }
    spriteFond.emplace(textureFond);
    spriteFond->setScale({
        800.f / textureFond.getSize().x,
        560.f / textureFond.getSize().y
    });

    if (!font.openFromFile("/System/Library/Fonts/Supplemental/Arial.ttf")) {
        return false;
    }

    if (!textureBtnTirer.loadFromFile(asset("asset/menu/Bouton TirerCarte.png"))) {
        return false;
    }
    spriteBtnTirer.emplace(textureBtnTirer);
    float s = 220.f / textureBtnTirer.getSize().x;
    spriteBtnTirer->setScale({s, s});
    spriteBtnTirer->setPosition({140.f, 430.f});

    return true;
}

void Game::setupUi() {
    txtTitreMenu.emplace(font, "Jeu de Cartes", 48);
    txtTitreMenu->setFillColor(sf::Color::White);
    {
        sf::FloatRect tb = txtTitreMenu->getLocalBounds();
        txtTitreMenu->setOrigin({tb.position.x + tb.size.x / 2.f, tb.position.y});
        txtTitreMenu->setPosition({400.f, 52.f});
    }

    btnMenuLancer.setPosition({240.f, 210.f});
    btnMenuParam.setPosition({240.f, 295.f});
    btnMenuQuitter.setPosition({240.f, 380.f});

    btnMenuLancer.setFillColor(couleurBaseLancer);
    btnMenuParam.setFillColor(couleurBaseParam);
    btnMenuQuitter.setFillColor(couleurBaseQuitter);

    btnMenuLancer.setOutlineThickness(2.f);
    btnMenuParam.setOutlineThickness(2.f);
    btnMenuQuitter.setOutlineThickness(2.f);
    btnMenuLancer.setOutlineColor(sf::Color::White);
    btnMenuParam.setOutlineColor(sf::Color::White);
    btnMenuQuitter.setOutlineColor(sf::Color::White);

    txtMenuLancer.emplace(font, "Lancer une partie", 28);
    txtMenuParam.emplace(font, "Parametre", 28);
    txtMenuQuitter.emplace(font, "Quitter", 28);

    txtMenuLancer->setFillColor(sf::Color::White);
    txtMenuParam->setFillColor(sf::Color::White);
    txtMenuQuitter->setFillColor(sf::Color::White);

    centrerTexte(*txtMenuLancer, btnMenuLancer);
    centrerTexte(*txtMenuParam, btnMenuParam);
    centrerTexte(*txtMenuQuitter, btnMenuQuitter);

    txtParamInfo.emplace(font, "Parametre: bientot disponible", 20);
    txtParamInfo->setFillColor({255, 245, 170});
    {
        sf::FloatRect pb = txtParamInfo->getLocalBounds();
        txtParamInfo->setOrigin({pb.position.x + pb.size.x / 2.f, pb.position.y});
        txtParamInfo->setPosition({400.f, 470.f});
    }

    btnQuitter.setPosition({450.f, 460.f});
    btnQuitter.setFillColor({180, 30, 30});
    btnQuitter.setOutlineThickness(2.f);
    btnQuitter.setOutlineColor(sf::Color::White);

    txtQuitter.emplace(font, "Retour au menu", 22);
    txtQuitter->setFillColor(sf::Color::White);
    centrerTexte(*txtQuitter, btnQuitter);

    txtTitre.emplace(font, "Jeu de Cartes", 34);
    txtTitre->setFillColor(sf::Color::White);
    {
        sf::FloatRect tb = txtTitre->getLocalBounds();
        txtTitre->setOrigin({tb.position.x + tb.size.x / 2.f, tb.position.y});
        txtTitre->setPosition({400.f, 18.f});
    }
}

void Game::reinitialiserPartie() {
    int id = 1;
    for (int couleur = 1; couleur <= 4; ++couleur) {
        for (int valeur = 1; valeur <= 13; ++valeur) {
            paquet[id - 1] = carte(id, valeur, couleur);
            ++id;
        }
    }
    std::shuffle(paquet, paquet + kDeckSize, rng);
    index = 0;
    cartes[0] = nullptr;
    cartes[1] = nullptr;
    fin = false;
}

void Game::run() {
    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            handleEvent(*event);
        }
        renderFrame();
    }
}

void Game::handleEvent(const sf::Event& event) {
    if (event.is<sf::Event::Closed>()) {
        window.close();
        return;
    }

    if (const auto* resized = event.getIf<sf::Event::Resized>()) {
        handleResized(*resized);
    }

    if (const auto* key = event.getIf<sf::Event::KeyPressed>()) {
        handleKeyPressed(*key);
    }

    if (const auto* click = event.getIf<sf::Event::MouseButtonPressed>()) {
        handleMousePressed(*click);
    }
}

void Game::handleResized(const sf::Event::Resized& resized) {
    float winW = static_cast<float>(resized.size.x);
    float winH = static_cast<float>(resized.size.y);
    float ratio = std::min(winW / 800.f, winH / 560.f);
    float vpW = (800.f * ratio) / winW;
    float vpH = (560.f * ratio) / winH;
    vue.setViewport(sf::FloatRect(
        {(1.f - vpW) / 2.f, (1.f - vpH) / 2.f},
        {vpW, vpH}
    ));
    window.setView(vue);
}

void Game::handleKeyPressed(const sf::Event::KeyPressed& key) {
    if (ecran == Ecran::Menu) {
        if (key.code == sf::Keyboard::Key::Up) {
            selectionMenu = (selectionMenu + 2) % 3;
        } else if (key.code == sf::Keyboard::Key::Down) {
            selectionMenu = (selectionMenu + 1) % 3;
        } else if (key.code == sf::Keyboard::Key::Enter) {
            switch (selectionMenu) {
                case 0:
                    reinitialiserPartie();
                    afficherMessageParam = false;
                    ecran = Ecran::Jeu;
                    break;
                case 1:
                    afficherMessageParam = true;
                    break;
                case 2:
                    window.close();
                    break;
            }
        }
        return;
    }

    if (key.code == sf::Keyboard::Key::Left || key.code == sf::Keyboard::Key::Right) {
        selectionJeu = 1 - selectionJeu;
    } else if (key.code == sf::Keyboard::Key::Enter) {
        if (selectionJeu == 0) {
            if (!fin && index + 2 <= kDeckSize) {
                cartes[0] = &paquet[index];
                cartes[1] = &paquet[index + 1];
                index += 2;
                if (index >= kDeckSize) {
                    fin = true;
                }
            }
        } else {
            afficherMessageParam = false;
            ecran = Ecran::Menu;
        }
    }
}

void Game::handleMousePressed(const sf::Event::MouseButtonPressed& click) {
    sf::Vector2f pos = window.mapPixelToCoords(sf::Vector2i(click.position));

    if (ecran == Ecran::Menu) {
        if (btnMenuLancer.getGlobalBounds().contains(pos)) {
            reinitialiserPartie();
            afficherMessageParam = false;
            ecran = Ecran::Jeu;
        } else if (btnMenuParam.getGlobalBounds().contains(pos)) {
            afficherMessageParam = true;
        } else if (btnMenuQuitter.getGlobalBounds().contains(pos)) {
            window.close();
        }
        return;
    }

    if (!fin && spriteBtnTirer->getGlobalBounds().contains(pos)) {
        if (index + 2 <= kDeckSize) {
            cartes[0] = &paquet[index];
            cartes[1] = &paquet[index + 1];
            index += 2;
            if (index >= kDeckSize) {
                fin = true;
            }
        }
    }

    if (btnQuitter.getGlobalBounds().contains(pos)) {
        afficherMessageParam = false;
        ecran = Ecran::Menu;
    }
}

void Game::renderFrame() {
    if (ecran == Ecran::Menu) {
        renderMenu();
    } else {
        renderJeu();
    }
    window.display();
}

void Game::updateMenuHighlight() {
    sf::Vector2f mp = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    if (btnMenuLancer.getGlobalBounds().contains(mp)) {
        selectionMenu = 0;
    } else if (btnMenuParam.getGlobalBounds().contains(mp)) {
        selectionMenu = 1;
    } else if (btnMenuQuitter.getGlobalBounds().contains(mp)) {
        selectionMenu = 2;
    }

    auto surbrillance = [](sf::RectangleShape& btn, sf::Color base, bool actif) {
        if (actif) {
            btn.setFillColor({
                static_cast<std::uint8_t>(std::min(255, base.r + 55)),
                static_cast<std::uint8_t>(std::min(255, base.g + 55)),
                static_cast<std::uint8_t>(std::min(255, base.b + 55))
            });
            btn.setOutlineColor(sf::Color::Yellow);
            btn.setOutlineThickness(3.f);
        } else {
            btn.setFillColor(base);
            btn.setOutlineColor(sf::Color::White);
            btn.setOutlineThickness(2.f);
        }
    };

    surbrillance(btnMenuLancer, couleurBaseLancer, selectionMenu == 0);
    surbrillance(btnMenuParam, couleurBaseParam, selectionMenu == 1);
    surbrillance(btnMenuQuitter, couleurBaseQuitter, selectionMenu == 2);
}

void Game::renderMenu() {
    window.clear(sf::Color::Black);
    updateMenuHighlight();

    window.draw(*txtTitreMenu);
    window.draw(btnMenuLancer);
    window.draw(btnMenuParam);
    window.draw(btnMenuQuitter);
    window.draw(*txtMenuLancer);
    window.draw(*txtMenuParam);
    window.draw(*txtMenuQuitter);
    if (afficherMessageParam) {
        window.draw(*txtParamInfo);
    }
}

void Game::updateJeuHighlight() {
    sf::Vector2f mp = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    if (spriteBtnTirer->getGlobalBounds().contains(mp)) {
        selectionJeu = 0;
    } else if (btnQuitter.getGlobalBounds().contains(mp)) {
        selectionJeu = 1;
    }

    spriteBtnTirer->setColor(selectionJeu == 0 ? sf::Color{255, 255, 180} : sf::Color::White);
    btnQuitter.setFillColor(selectionJeu == 1 ? sf::Color{220, 70, 70} : sf::Color{180, 30, 30});
}

void Game::renderJeu() {
    window.clear();
    window.draw(*spriteFond);

    sf::Text txtRestant(font, "Cartes restantes : " + std::to_string(kDeckSize - index), 18);
    txtRestant.setFillColor(sf::Color::White);
    txtRestant.setPosition({10.f, 10.f});
    window.draw(txtRestant);

    const float cardY = 170.f;
    for (int i = 0; i < 2; ++i) {
        float cardX = (i == 0) ? 175.f : 475.f;
        if (cartes[i]) {
            dessinerCarte(window, *cartes[i], cardX, cardY, font);
        } else {
            sf::RectangleShape vide({150.f, 220.f});
            vide.setPosition({cardX, cardY});
            vide.setFillColor({20, 80, 20});
            vide.setOutlineThickness(2.f);
            vide.setOutlineColor({150, 150, 150});
            window.draw(vide);
        }
    }

    if (fin) {
        sf::Text txtFin(font, "Paquet epuise !", 26);
        txtFin.setFillColor(sf::Color::Yellow);
        sf::FloatRect fb = txtFin.getLocalBounds();
        txtFin.setOrigin({fb.position.x + fb.size.x / 2.f, fb.position.y});
        txtFin.setPosition({400.f, 420.f});
        window.draw(txtFin);
    }

    updateJeuHighlight();
    window.draw(*spriteBtnTirer);
    window.draw(btnQuitter);
    window.draw(*txtQuitter);
}
