#include "carte.h"
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <random>
#include <string>
#include <filesystem>

// Centre un texte dans un rectangle
static void centrerTexte(sf::Text& txt, const sf::RectangleShape& rect) {
    sf::FloatRect tb = txt.getLocalBounds();
    txt.setOrigin({tb.position.x + tb.size.x / 2.f, tb.position.y + tb.size.y / 2.f});
    sf::FloatRect rb = rect.getGlobalBounds();
    txt.setPosition({rb.position.x + rb.size.x / 2.f, rb.position.y + rb.size.y / 2.f});
}

// Dessine une carte à la position donnée
static void dessinerCarte(sf::RenderWindow& window, carte& c, float x, float y, const sf::Font& font) {
    const float W = 150.f, H = 220.f;

    sf::RectangleShape fond({W, H});
    fond.setPosition({x, y});
    fond.setFillColor(sf::Color::White);
    fond.setOutlineThickness(3.f);
    fond.setOutlineColor({80, 80, 80});
    window.draw(fond);

    sf::Color couleur = (c.getCouleur() <= 2) ? sf::Color::Red : sf::Color::Black;

    // Valeur en haut à gauche
    sf::Text valHaut(font, c.getNomValeur(), 22);
    valHaut.setFillColor(couleur);
    sf::FloatRect vb = valHaut.getLocalBounds();
    valHaut.setOrigin({vb.position.x, vb.position.y});
    valHaut.setPosition({x + 10.f, y + 8.f});
    window.draw(valHaut);

    // Couleur au centre
    sf::Text nomCouleur(font, c.getNomCouleur(), 20);
    nomCouleur.setFillColor(couleur);
    sf::FloatRect cb = nomCouleur.getLocalBounds();
    nomCouleur.setOrigin({cb.position.x + cb.size.x / 2.f, cb.position.y + cb.size.y / 2.f});
    nomCouleur.setPosition({x + W / 2.f, y + H / 2.f});
    window.draw(nomCouleur);

    // Valeur en bas à droite (retournée visuellement)
    sf::Text valBas(font, c.getNomValeur(), 22);
    valBas.setFillColor(couleur);
    sf::FloatRect vb2 = valBas.getLocalBounds();
    valBas.setOrigin({vb2.position.x + vb2.size.x, vb2.position.y + vb2.size.y});
    valBas.setPosition({x + W - 10.f, y + H - 8.f});
    valBas.setRotation(sf::degrees(180.f));
    window.draw(valBas);
}

int main(int /*argc*/, char* argv[]) {

    // Répertoire de base = dossier de l'exécutable
    std::filesystem::path baseDir = std::filesystem::canonical(argv[0]).parent_path();
    auto asset = [&](const std::string& rel) { return (baseDir / rel).string(); };

    sf::RenderWindow window(sf::VideoMode({800, 560}), "OMBLING");
    window.setFramerateLimit(60);

    // Vue fixe 800x560 : tout le jeu reste en coordonnées logiques
    sf::View vue(sf::FloatRect({0.f, 0.f}, {800.f, 560.f}));
    window.setView(vue);

    sf::Texture textureFond;
    if (!textureFond.loadFromFile(asset("asset/fond/Fond table.png")))
        return -1;
    sf::Sprite spriteFond(textureFond);
    // Mise à l'échelle pour couvrir toute la fenêtre
    spriteFond.setScale({
        800.f / textureFond.getSize().x,
        560.f / textureFond.getSize().y
    });

    sf::Font font;
    if (!font.openFromFile("/System/Library/Fonts/Supplemental/Arial.ttf"))
        return -1;

    enum class Ecran {
        Menu,
        Jeu
    };
    Ecran ecran = Ecran::Menu;
    bool afficherMessageParam = false;
    int selectionMenu = 0; // 0=Lancer, 1=Param, 2=Quitter
    int selectionJeu  = 0; // 0=Tirer, 1=Retour menu

    const sf::Color couleurBaseLancer  = {28, 116, 58};
    const sf::Color couleurBaseParam   = {40, 90, 160};
    const sf::Color couleurBaseQuitter = {160, 40, 40};

    // Titre principal (menu)
    sf::Text txtTitreMenu(font, "Jeu de Cartes", 48);
    txtTitreMenu.setFillColor(sf::Color::White);
    {
        sf::FloatRect tb = txtTitreMenu.getLocalBounds();
        txtTitreMenu.setOrigin({tb.position.x + tb.size.x / 2.f, tb.position.y});
        txtTitreMenu.setPosition({400.f, 52.f});
    }

    // Boutons du menu
    sf::RectangleShape btnMenuLancer({320.f, 62.f});
    sf::RectangleShape btnMenuParam({320.f, 62.f});
    sf::RectangleShape btnMenuQuitter({320.f, 62.f});

    btnMenuLancer.setPosition({240.f, 210.f});
    btnMenuParam.setPosition({240.f, 295.f});
    btnMenuQuitter.setPosition({240.f, 380.f});

    btnMenuLancer.setFillColor({28, 116, 58});
    btnMenuParam.setFillColor({40, 90, 160});
    btnMenuQuitter.setFillColor({160, 40, 40});

    btnMenuLancer.setOutlineThickness(2.f);
    btnMenuParam.setOutlineThickness(2.f);
    btnMenuQuitter.setOutlineThickness(2.f);
    btnMenuLancer.setOutlineColor(sf::Color::White);
    btnMenuParam.setOutlineColor(sf::Color::White);
    btnMenuQuitter.setOutlineColor(sf::Color::White);

    sf::Text txtMenuLancer(font, "Lancer une partie", 28);
    sf::Text txtMenuParam(font, "Parametre", 28);
    sf::Text txtMenuQuitter(font, "Quitter", 28);
    txtMenuLancer.setFillColor(sf::Color::White);
    txtMenuParam.setFillColor(sf::Color::White);
    txtMenuQuitter.setFillColor(sf::Color::White);
    centrerTexte(txtMenuLancer, btnMenuLancer);
    centrerTexte(txtMenuParam, btnMenuParam);
    centrerTexte(txtMenuQuitter, btnMenuQuitter);

    sf::Text txtParamInfo(font, "Parametre: bientot disponible", 20);
    txtParamInfo.setFillColor({255, 245, 170});
    {
        sf::FloatRect pb = txtParamInfo.getLocalBounds();
        txtParamInfo.setOrigin({pb.position.x + pb.size.x / 2.f, pb.position.y});
        txtParamInfo.setPosition({400.f, 470.f});
    }

    // Créer et mélanger le paquet
    carte paquet[52];
    std::mt19937 rng(std::random_device{}());

    int index = 0;
    carte* cartes[2] = {nullptr, nullptr};
    bool fin = false;

    auto reinitialiserPartie = [&]() {
        int id = 1;
        for (int couleur = 1; couleur <= 4; couleur++) {
            for (int valeur = 1; valeur <= 13; valeur++) {
                paquet[id++ - 1] = carte(id - 1, valeur, couleur);
            }
        }
        std::shuffle(paquet, paquet + 52, rng);
        index = 0;
        cartes[0] = nullptr;
        cartes[1] = nullptr;
        fin = false;
    };

    // Boutons
    sf::Texture textureBtnTirer;
    if (!textureBtnTirer.loadFromFile(asset("asset/menu/Bouton TirerCarte.png")))
        return -1;
    sf::Sprite spriteBtnTirer(textureBtnTirer);
    // Échelle uniforme : largeur cible 220px pour garder un gros bouton sans déformation
    {
        float s = 220.f / textureBtnTirer.getSize().x;
        spriteBtnTirer.setScale({s, s});
    }
    spriteBtnTirer.setPosition({140.f, 430.f});

    sf::RectangleShape btnQuitter({160.f, 55.f});
    btnQuitter.setPosition({450.f, 460.f});
    btnQuitter.setFillColor({180, 30, 30});
    btnQuitter.setOutlineThickness(2.f);
    btnQuitter.setOutlineColor(sf::Color::White);

    sf::Text txtQuitter(font, "Retour au menu", 22);
    txtQuitter.setFillColor(sf::Color::White);
    centrerTexte(txtQuitter, btnQuitter);

    // Titre
    sf::Text txtTitre(font, "Jeu de Cartes", 34);
    txtTitre.setFillColor(sf::Color::White);
    {
        sf::FloatRect tb = txtTitre.getLocalBounds();
        txtTitre.setOrigin({tb.position.x + tb.size.x / 2.f, tb.position.y});
        txtTitre.setPosition({400.f, 18.f});
    }

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();

            // Mise à jour de la vue lors du redimensionnement (letterboxing)
            if (const auto* resized = event->getIf<sf::Event::Resized>()) {
                float winW = static_cast<float>(resized->size.x);
                float winH = static_cast<float>(resized->size.y);
                float ratio = std::min(winW / 800.f, winH / 560.f);
                float vpW = (800.f * ratio) / winW;
                float vpH = (560.f * ratio) / winH;
                vue.setViewport(sf::FloatRect(
                    {(1.f - vpW) / 2.f, (1.f - vpH) / 2.f},
                    {vpW, vpH}
                ));
                window.setView(vue);
            }

            if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
                if (ecran == Ecran::Menu) {
                    if (key->code == sf::Keyboard::Key::Up)
                        selectionMenu = (selectionMenu + 2) % 3;
                    else if (key->code == sf::Keyboard::Key::Down)
                        selectionMenu = (selectionMenu + 1) % 3;
                    else if (key->code == sf::Keyboard::Key::Enter) {
                        switch (selectionMenu) {
                            case 0: reinitialiserPartie(); afficherMessageParam = false; ecran = Ecran::Jeu; break;
                            case 1: afficherMessageParam = true; break;
                            case 2: window.close(); break;
                        }
                    }
                } else {
                    if (key->code == sf::Keyboard::Key::Left || key->code == sf::Keyboard::Key::Right)
                        selectionJeu = 1 - selectionJeu;
                    else if (key->code == sf::Keyboard::Key::Enter) {
                        if (selectionJeu == 0) {
                            if (!fin && index + 2 <= 52) {
                                cartes[0] = &paquet[index];
                                cartes[1] = &paquet[index + 1];
                                index += 2;
                                if (index >= 52) fin = true;
                            }
                        } else {
                            afficherMessageParam = false;
                            ecran = Ecran::Menu;
                        }
                    }
                }
            }

            if (const auto* click = event->getIf<sf::Event::MouseButtonPressed>()) {
                // Convertir les pixels écran en coordonnées logiques du jeu
                sf::Vector2f pos = window.mapPixelToCoords(sf::Vector2i(click->position));

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
                } else {
                    if (!fin && spriteBtnTirer.getGlobalBounds().contains(pos)) {
                        if (index + 2 <= 52) {
                            cartes[0] = &paquet[index];
                            cartes[1] = &paquet[index + 1];
                            index += 2;
                            if (index >= 52) fin = true;
                        }
                    }
                    if (btnQuitter.getGlobalBounds().contains(pos)) {
                        afficherMessageParam = false;
                        ecran = Ecran::Menu;
                    }
                }
            }
        }

        if (ecran == Ecran::Menu) {
            window.clear(sf::Color::Black);

            // Surbrillance des boutons du menu (souris + clavier)
            {
                sf::Vector2f mp = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                if (btnMenuLancer.getGlobalBounds().contains(mp))       selectionMenu = 0;
                else if (btnMenuParam.getGlobalBounds().contains(mp))   selectionMenu = 1;
                else if (btnMenuQuitter.getGlobalBounds().contains(mp)) selectionMenu = 2;

                auto surbrillance = [](sf::RectangleShape& btn, sf::Color base, bool actif) {
                    if (actif) {
                        btn.setFillColor({(uint8_t)std::min(255, base.r + 55),
                                          (uint8_t)std::min(255, base.g + 55),
                                          (uint8_t)std::min(255, base.b + 55)});
                        btn.setOutlineColor(sf::Color::Yellow);
                        btn.setOutlineThickness(3.f);
                    } else {
                        btn.setFillColor(base);
                        btn.setOutlineColor(sf::Color::White);
                        btn.setOutlineThickness(2.f);
                    }
                };
                surbrillance(btnMenuLancer,  couleurBaseLancer,  selectionMenu == 0);
                surbrillance(btnMenuParam,   couleurBaseParam,   selectionMenu == 1);
                surbrillance(btnMenuQuitter, couleurBaseQuitter, selectionMenu == 2);
            }

            window.draw(txtTitreMenu);
            window.draw(btnMenuLancer);
            window.draw(btnMenuParam);
            window.draw(btnMenuQuitter);
            window.draw(txtMenuLancer);
            window.draw(txtMenuParam);
            window.draw(txtMenuQuitter);
            if (afficherMessageParam)
                window.draw(txtParamInfo);
        } else {
            window.clear();
            window.draw(spriteFond);

            // Compteur
            sf::Text txtRestant(font, "Cartes restantes : " + std::to_string(52 - index), 18);
            txtRestant.setFillColor(sf::Color::White);
            txtRestant.setPosition({10.f, 10.f});
            window.draw(txtRestant);

            // Cartes vides ou tirees
            float cardY = 170.f;
            for (int i = 0; i < 2; i++) {
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

            // Message fin de paquet
            if (fin) {
                sf::Text txtFin(font, "Paquet epuise !", 26);
                txtFin.setFillColor(sf::Color::Yellow);
                sf::FloatRect fb = txtFin.getLocalBounds();
                txtFin.setOrigin({fb.position.x + fb.size.x / 2.f, fb.position.y});
                txtFin.setPosition({400.f, 420.f});
                window.draw(txtFin);
            }

            // Surbrillance des boutons en jeu (souris + clavier)
            {
                sf::Vector2f mp = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                if (spriteBtnTirer.getGlobalBounds().contains(mp))      selectionJeu = 0;
                else if (btnQuitter.getGlobalBounds().contains(mp))     selectionJeu = 1;

                spriteBtnTirer.setColor(selectionJeu == 0
                    ? sf::Color{255, 255, 180} : sf::Color::White);
                btnQuitter.setFillColor(selectionJeu == 1
                    ? sf::Color{220, 70, 70} : sf::Color{180, 30, 30});
            }

            window.draw(spriteBtnTirer);
            window.draw(btnQuitter);
            window.draw(txtQuitter);
        }

        window.display();
    }

    return 0;
}
