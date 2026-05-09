#include "game.h"

#include "ai_easy.h"
#include "ai_hard.h"
#include "ai_medium.h"
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

std::string Game::cheminCarte(const carte& c) const {
    std::string valStr;
    const std::string nom = c.getNomValeur();
    if      (nom == "As")     valStr = "ace";
    else if (nom == "Valet")  valStr = "jack";
    else if (nom == "Reine")  valStr = "queen";
    else if (nom == "Roi")    valStr = "king";
    else                      valStr = nom; // "2"-"10"

    std::string coulStr;
    switch (c.getCouleur()) {
        case 1: coulStr = "hearts";   break;
        case 2: coulStr = "diamonds"; break;
        case 3: coulStr = "clubs";    break;
        case 4: coulStr = "spades";   break;
        default: coulStr = "spades";  break;
    }

    return asset("asset/carte/" + valStr + "_of_" + coulStr + ".png");
}

void Game::setupPredictionUi() {
    for (int i = 0; i < kPredictionCount; ++i) {
        btnPredictions[i].setSize({165.f, 29.f});
        btnPredictions[i].setPosition({20.f, 70.f + i * 35.f});
        btnPredictions[i].setFillColor({35, 60, 95});
        btnPredictions[i].setOutlineThickness(2.f);
        btnPredictions[i].setOutlineColor(sf::Color::White);

        txtPredictions[i].emplace(font, PredictionEngine::predictionLabels()[i], 14);
        txtPredictions[i]->setFillColor(sf::Color::White);
        centrerTexte(*txtPredictions[i], btnPredictions[i]);
    }
}

void Game::memoriserCarteVue(int id) {
    cartesVues.push_back(id);
    cartesVues10.push_back(id);
    if (cartesVues10.size() > 10) {
        cartesVues10.pop_front();
    }
}

std::vector<int> Game::cartesConnuesIA() const {
    if (modeJeu == ModeJeu::SoloDifficile) {
        return cartesVues;
    }
    if (modeJeu == ModeJeu::SoloMoyen) {
        return std::vector<int>(cartesVues10.begin(), cartesVues10.end());
    }
    return {};
}

void Game::tirerDeuxCartes() {
    if (fin || index + 2 > kDeckSize) {
        fin = true;
        return;
    }

    preparerTourSolo();

    cartes[0] = &paquet[index];
    cartes[1] = &paquet[index + 1];
    memoriserCarteVue(cartes[0]->getId());
    memoriserCarteVue(cartes[1]->getId());
    index += 2;

    if (index >= kDeckSize) {
        fin = true;
    }

    appliquerPrediction();
    appliquerScoreJoueurSelonMode();
    appliquerTourSolo();

    if (fin) {
        terminerPartie();
    }
}

bool Game::predictionReussie(PredictionType type) const {
    if (!cartes[0] || !cartes[1]) {
        return false;
    }

    const int gaucheValeur = cartes[0]->getValeur();
    const int droiteValeur = cartes[1]->getValeur();
    const int gaucheCouleur = cartes[0]->getCouleur();
    const int droiteCouleur = cartes[1]->getCouleur();

    return PredictionEngine::isSuccessful(type, gaucheValeur, gaucheCouleur, droiteValeur, droiteCouleur);
}

void Game::appliquerPrediction() {
    if (predictionReussie(predictionActive)) {
        derniersPoints = PredictionEngine::points(predictionActive);
        dernierResultat = "Reussi: +" + std::to_string(derniersPoints) + " pts";
    } else {
        derniersPoints = 0;
        dernierResultat = "Rate: +0 pt";
    }
}

bool Game::loadAssets() {
    const std::vector<std::string> candidates = {
        asset("asset/font/arial.ttf"),
        asset("asset/font/Arial.ttf"),
        "/System/Library/Fonts/Supplemental/Arial.ttf",
        "/System/Library/Fonts/Supplemental/Helvetica.ttf",
        "/Library/Fonts/Arial.ttf",
        "C:/Windows/Fonts/arial.ttf",
        "C:/Windows/Fonts/segoeui.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf"
    };

    bool fontLoaded = false;
    for (const auto& path : candidates) {
        if (font.openFromFile(path)) {
            fontLoaded = true;
            break;
        }
    }

    if (!fontLoaded) {
        return false;
    }

    if (!textureFond.loadFromFile(asset("asset/fond/felt_green.jpg"))) {
        return false;
    }
    spriteFond.emplace(textureFond);
    spriteFond->setScale({
        800.f / static_cast<float>(textureFond.getSize().x),
        560.f / static_cast<float>(textureFond.getSize().y)
    });

    return true;
}

void Game::setupUi() {
    // ---- Menu principal ----
    txtTitreMenu.emplace(font, "OMBLING", 52);
    txtTitreMenu->setFillColor(sf::Color::White);
    {
        sf::FloatRect tb = txtTitreMenu->getLocalBounds();
        txtTitreMenu->setOrigin({tb.position.x + tb.size.x / 2.f, tb.position.y});
        txtTitreMenu->setPosition({400.f, 45.f});
    }

    btnMenuPartieTest.setPosition({240.f, 148.f});
    btnMenuSolo.setPosition({240.f, 223.f});
    btnMenuMulti.setPosition({240.f, 298.f});
    btnMenuQuitter.setPosition({240.f, 373.f});

    btnMenuPartieTest.setFillColor(couleurBasePartieTest);
    btnMenuSolo.setFillColor(couleurBaseSolo);
    btnMenuMulti.setFillColor(couleurBaseMulti);
    btnMenuQuitter.setFillColor(couleurBaseQuitter);

    for (auto* btn : {&btnMenuPartieTest, &btnMenuSolo, &btnMenuMulti, &btnMenuQuitter}) {
        btn->setOutlineThickness(2.f);
        btn->setOutlineColor(sf::Color::White);
    }

    txtMenuPartieTest.emplace(font, "Partie Test", 28);
    txtMenuSolo.emplace(font, "Solo", 28);
    txtMenuMulti.emplace(font, "Multijoueur", 28);
    txtMenuQuitter.emplace(font, "Quitter", 28);

    txtMenuPartieTest->setFillColor(sf::Color::White);
    txtMenuSolo->setFillColor(sf::Color::White);
    txtMenuMulti->setFillColor(sf::Color::White);
    txtMenuQuitter->setFillColor(sf::Color::White);

    centrerTexte(*txtMenuPartieTest, btnMenuPartieTest);
    centrerTexte(*txtMenuSolo, btnMenuSolo);
    centrerTexte(*txtMenuMulti, btnMenuMulti);
    centrerTexte(*txtMenuQuitter, btnMenuQuitter);

    // ---- Sous-menu Solo ----
    txtTitreSolo.emplace(font, "Mode Solo", 46);
    txtTitreSolo->setFillColor(sf::Color::White);
    {
        sf::FloatRect tb = txtTitreSolo->getLocalBounds();
        txtTitreSolo->setOrigin({tb.position.x + tb.size.x / 2.f, tb.position.y});
        txtTitreSolo->setPosition({400.f, 52.f});
    }

    btnSoloFacile.setPosition({250.f, 165.f});
    btnSoloMoyen.setPosition({250.f, 245.f});
    btnSoloDifficile.setPosition({250.f, 325.f});
    btnSoloRetour.setPosition({30.f, 468.f});

    btnSoloFacile.setFillColor(couleurBaseFacile);
    btnSoloMoyen.setFillColor(couleurBaseMoyen);
    btnSoloDifficile.setFillColor(couleurBaseDifficile);
    btnSoloRetour.setFillColor(couleurBaseRetour);

    for (auto* btn : {&btnSoloFacile, &btnSoloMoyen, &btnSoloDifficile, &btnSoloRetour}) {
        btn->setOutlineThickness(2.f);
        btn->setOutlineColor(sf::Color::White);
    }

    txtSoloFacile.emplace(font, "Facile", 26);
    txtSoloMoyen.emplace(font, "Moyen", 26);
    txtSoloDifficile.emplace(font, "Difficile", 26);
    txtSoloRetour.emplace(font, "< Retour", 20);

    txtSoloFacile->setFillColor(sf::Color::White);
    txtSoloMoyen->setFillColor(sf::Color::White);
    txtSoloDifficile->setFillColor(sf::Color::White);
    txtSoloRetour->setFillColor(sf::Color::White);

    centrerTexte(*txtSoloFacile, btnSoloFacile);
    centrerTexte(*txtSoloMoyen, btnSoloMoyen);
    centrerTexte(*txtSoloDifficile, btnSoloDifficile);
    centrerTexte(*txtSoloRetour, btnSoloRetour);

    // ---- Sous-menu Multi ----
    txtTitreMulti.emplace(font, "Multijoueur", 46);
    txtTitreMulti->setFillColor(sf::Color::White);
    {
        sf::FloatRect tb = txtTitreMulti->getLocalBounds();
        txtTitreMulti->setOrigin({tb.position.x + tb.size.x / 2.f, tb.position.y});
        txtTitreMulti->setPosition({400.f, 52.f});
    }

    btnMultiMaison.setPosition({250.f, 195.f});
    btnMultiInternet.setPosition({250.f, 285.f});
    btnMultiRetour.setPosition({30.f, 468.f});

    btnMultiMaison.setFillColor(couleurBaseMaison);
    btnMultiInternet.setFillColor(couleurBaseInternet);
    btnMultiRetour.setFillColor(couleurBaseRetour);

    for (auto* btn : {&btnMultiMaison, &btnMultiInternet, &btnMultiRetour}) {
        btn->setOutlineThickness(2.f);
        btn->setOutlineColor(sf::Color::White);
    }

    txtMultiMaison.emplace(font, "Maison", 26);
    txtMultiInternet.emplace(font, "Online", 26);
    txtMultiRetour.emplace(font, "< Retour", 20);

    txtMultiMaison->setFillColor(sf::Color::White);
    txtMultiInternet->setFillColor(sf::Color::White);
    txtMultiRetour->setFillColor(sf::Color::White);

    centrerTexte(*txtMultiMaison, btnMultiMaison);
    centrerTexte(*txtMultiInternet, btnMultiInternet);
    centrerTexte(*txtMultiRetour, btnMultiRetour);

    // ---- Ecran Jeu ----
    btnQuitter.setPosition({630.f, 495.f});
    btnQuitter.setFillColor({180, 30, 30});
    btnQuitter.setOutlineThickness(2.f);
    btnQuitter.setOutlineColor(sf::Color::White);

    txtQuitter.emplace(font, "Retour au menu", 22);
    txtQuitter->setFillColor(sf::Color::White);
    centrerTexte(*txtQuitter, btnQuitter);

    btnTirer.setPosition({290.f, 440.f});
    btnTirer.setFillColor(couleurBaseTirer);
    btnTirer.setOutlineThickness(2.f);
    btnTirer.setOutlineColor(sf::Color::White);

    txtBtnTirer.emplace(font, "Tirer une carte", 24);
    txtBtnTirer->setFillColor(sf::Color::White);
    centrerTexte(*txtBtnTirer, btnTirer);

    txtTitre.emplace(font, "Partie Test", 34);
    txtTitre->setFillColor(sf::Color::White);
    {
        sf::FloatRect tb = txtTitre->getLocalBounds();
        txtTitre->setOrigin({tb.position.x + tb.size.x / 2.f, tb.position.y});
        txtTitre->setPosition({400.f, 18.f});
    }

    // ---- Ecran Fin de Partie ----
    btnFinMenu.setPosition({260.f, 405.f});
    btnFinMenu.setFillColor({40, 90, 160});
    btnFinMenu.setOutlineThickness(2.f);
    btnFinMenu.setOutlineColor(sf::Color::White);

    txtFinMenu.emplace(font, "Retour au menu", 26);
    txtFinMenu->setFillColor(sf::Color::White);
    centrerTexte(*txtFinMenu, btnFinMenu);

    txtFinTitre.emplace(font, "Resultat", 56);
    txtFinTitre->setFillColor(sf::Color::White);

    txtFinDetail.emplace(font, "", 24);
    txtFinDetail->setFillColor({230, 230, 230});

    setupPredictionUi();
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
    cartesVues.clear();
    cartesVues10.clear();
    cartes[0] = nullptr;
    cartes[1] = nullptr;
    cartesChargees[0] = nullptr;
    cartesChargees[1] = nullptr;
    spritesCarte[0].reset();
    spritesCarte[1].reset();
    selectionJeu = 0;
    score = 0;
    scoreIA = 0;
    scoreJ1 = 0;
    scoreJ2 = 0;
    joueurCourant = 1;
    derniersPoints = 0;
    predictionActive = PredictionType::Couleur;
    predictionIA = PredictionType::Couleur;
    dernierePredictionIA = PredictionType::Couleur;
    dernierResultat = "Choisis une prediction puis tire 2 cartes";
    dernierResultatIA = "IA en attente";
    suiteStreakIA = 0;
    fin = false;
}

void Game::demarrerPartie(ModeJeu mode) {
    modeJeu = mode;
    reinitialiserPartie();
    configurerTitreSelonMode();
    sf::FloatRect tb = txtTitre->getLocalBounds();
    txtTitre->setOrigin({tb.position.x + tb.size.x / 2.f, tb.position.y});
    txtTitre->setPosition({400.f, 18.f});
    ecran = Ecran::Jeu;
}

void Game::terminerPartie() {
    std::string titre;
    std::string detail;

    sf::Color couleurTitre = sf::Color::White;
    if (estModeSolo()) {
        configurerFinPartieSolo(titre, detail, couleurTitre);
    } else if (estModeMultiMaison()) {
        configurerFinPartieMulti(titre, detail, couleurTitre);
    } else {
        configurerFinPartieTest(titre, detail, couleurTitre);
    }
    txtFinTitre->setFillColor(couleurTitre);

    txtFinTitre->setString(titre);
    txtFinDetail->setString(detail);

    {
        sf::FloatRect tb = txtFinTitre->getLocalBounds();
        txtFinTitre->setOrigin({tb.position.x + tb.size.x / 2.f, tb.position.y});
        txtFinTitre->setPosition({400.f, 185.f});
    }
    {
        sf::FloatRect db = txtFinDetail->getLocalBounds();
        txtFinDetail->setOrigin({db.position.x + db.size.x / 2.f, db.position.y});
        txtFinDetail->setPosition({400.f, 282.f});
    }

    ecran = Ecran::FinPartie;
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
    if (ecran == Ecran::FinPartie) {
        if (key.code == sf::Keyboard::Key::Enter || key.code == sf::Keyboard::Key::Escape) {
            ecran = Ecran::Menu;
        }
        return;
    }

    if (ecran == Ecran::Menu) {
        if (key.code == sf::Keyboard::Key::Up) {
            selectionMenu = (selectionMenu + 3) % 4;
        } else if (key.code == sf::Keyboard::Key::Down) {
            selectionMenu = (selectionMenu + 1) % 4;
        } else if (key.code == sf::Keyboard::Key::Enter) {
            switch (selectionMenu) {
                case 0:
                    demarrerPartie(ModeJeu::PartieTest);
                    break;
                case 1:
                    selectionMenuSolo = 0;
                    ecran = Ecran::MenuSolo;
                    break;
                case 2:
                    selectionMenuMulti = 0;
                    ecran = Ecran::MenuMulti;
                    break;
                case 3:
                    window.close();
                    break;
                default:
                    break;
            }
        }
        return;
    }

    if (ecran == Ecran::MenuSolo) {
        if (key.code == sf::Keyboard::Key::Up) {
            selectionMenuSolo = (selectionMenuSolo + 3) % 4;
        } else if (key.code == sf::Keyboard::Key::Down) {
            selectionMenuSolo = (selectionMenuSolo + 1) % 4;
        } else if (key.code == sf::Keyboard::Key::Enter) {
            if (selectionMenuSolo == 0) {
                demarrerPartie(ModeJeu::SoloFacile);
            } else if (selectionMenuSolo == 1) {
                demarrerPartie(ModeJeu::SoloMoyen);
            } else if (selectionMenuSolo == 2) {
                demarrerPartie(ModeJeu::SoloDifficile);
            } else if (selectionMenuSolo == 3) {
                ecran = Ecran::Menu;
            }
        }
        return;
    }

    if (ecran == Ecran::MenuMulti) {
        if (key.code == sf::Keyboard::Key::Up) {
            selectionMenuMulti = (selectionMenuMulti + 2) % 3;
        } else if (key.code == sf::Keyboard::Key::Down) {
            selectionMenuMulti = (selectionMenuMulti + 1) % 3;
        } else if (key.code == sf::Keyboard::Key::Enter) {
            if (selectionMenuMulti == 0) {
                demarrerPartie(ModeJeu::MultiMaison);
            } else if (selectionMenuMulti == 2) {
                ecran = Ecran::Menu;
            }
        }
        return;
    }

    if (key.code == sf::Keyboard::Key::Left || key.code == sf::Keyboard::Key::Right) {
        selectionJeu = 1 - selectionJeu;
    } else if (key.code == sf::Keyboard::Key::Enter) {
        if (selectionJeu == 0) {
            tirerDeuxCartes();
        } else {
            ecran = Ecran::Menu;
        }
    }
}

void Game::handleMousePressed(const sf::Event::MouseButtonPressed& click) {
    sf::Vector2f pos = window.mapPixelToCoords(sf::Vector2i(click.position));

    if (ecran == Ecran::Menu) {
        if (btnMenuPartieTest.getGlobalBounds().contains(pos)) {
            demarrerPartie(ModeJeu::PartieTest);
        } else if (btnMenuSolo.getGlobalBounds().contains(pos)) {
            selectionMenuSolo = 0;
            ecran = Ecran::MenuSolo;
        } else if (btnMenuMulti.getGlobalBounds().contains(pos)) {
            selectionMenuMulti = 0;
            ecran = Ecran::MenuMulti;
        } else if (btnMenuQuitter.getGlobalBounds().contains(pos)) {
            window.close();
        }
        return;
    }

    if (ecran == Ecran::MenuSolo) {
        if (btnSoloFacile.getGlobalBounds().contains(pos)) {
            demarrerPartie(ModeJeu::SoloFacile);
        } else if (btnSoloMoyen.getGlobalBounds().contains(pos)) {
            demarrerPartie(ModeJeu::SoloMoyen);
        } else if (btnSoloDifficile.getGlobalBounds().contains(pos)) {
            demarrerPartie(ModeJeu::SoloDifficile);
        } else if (btnSoloRetour.getGlobalBounds().contains(pos)) {
            ecran = Ecran::Menu;
        }
        return;
    }

    if (ecran == Ecran::MenuMulti) {
        if (btnMultiMaison.getGlobalBounds().contains(pos)) {
            demarrerPartie(ModeJeu::MultiMaison);
        } else if (btnMultiRetour.getGlobalBounds().contains(pos)) {
            ecran = Ecran::Menu;
        }
        return;
    }

    if (ecran == Ecran::FinPartie) {
        if (btnFinMenu.getGlobalBounds().contains(pos)) {
            ecran = Ecran::Menu;
        }
        return;
    }

    for (int i = 0; i < kPredictionCount; ++i) {
        if (btnPredictions[i].getGlobalBounds().contains(pos)) {
            predictionActive = PredictionEngine::predictionTypes()[i];
            return;
        }
    }

    if (!fin && btnTirer.getGlobalBounds().contains(pos)) {
        tirerDeuxCartes();
    }

    if (btnQuitter.getGlobalBounds().contains(pos)) {
        ecran = Ecran::Menu;
    }
}

void Game::renderFrame() {
    if (ecran == Ecran::Menu) {
        renderMenu();
    } else if (ecran == Ecran::MenuSolo) {
        renderMenuSolo();
    } else if (ecran == Ecran::MenuMulti) {
        renderMenuMulti();
    } else if (ecran == Ecran::FinPartie) {
        renderFinPartie();
    } else {
        renderJeu();
    }
    window.display();
}

void Game::updateMenuHighlight() {
    sf::Vector2f mp = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    if (btnMenuPartieTest.getGlobalBounds().contains(mp)) {
        selectionMenu = 0;
    } else if (btnMenuSolo.getGlobalBounds().contains(mp)) {
        selectionMenu = 1;
    } else if (btnMenuMulti.getGlobalBounds().contains(mp)) {
        selectionMenu = 2;
    } else if (btnMenuQuitter.getGlobalBounds().contains(mp)) {
        selectionMenu = 3;
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

    surbrillance(btnMenuPartieTest, couleurBasePartieTest, selectionMenu == 0);
    surbrillance(btnMenuSolo, couleurBaseSolo, selectionMenu == 1);
    surbrillance(btnMenuMulti, couleurBaseMulti, selectionMenu == 2);
    surbrillance(btnMenuQuitter, couleurBaseQuitter, selectionMenu == 3);
}

void Game::updateSoloHighlight() {
    sf::Vector2f mp = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    if (btnSoloFacile.getGlobalBounds().contains(mp)) selectionMenuSolo = 0;
    else if (btnSoloMoyen.getGlobalBounds().contains(mp)) selectionMenuSolo = 1;
    else if (btnSoloDifficile.getGlobalBounds().contains(mp)) selectionMenuSolo = 2;
    else if (btnSoloRetour.getGlobalBounds().contains(mp)) selectionMenuSolo = 3;

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

    surbrillance(btnSoloFacile, couleurBaseFacile, selectionMenuSolo == 0);
    surbrillance(btnSoloMoyen, couleurBaseMoyen, selectionMenuSolo == 1);
    surbrillance(btnSoloDifficile, couleurBaseDifficile, selectionMenuSolo == 2);
    surbrillance(btnSoloRetour, couleurBaseRetour, selectionMenuSolo == 3);
}

void Game::updateMultiHighlight() {
    sf::Vector2f mp = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    if (btnMultiMaison.getGlobalBounds().contains(mp)) selectionMenuMulti = 0;
    else if (btnMultiInternet.getGlobalBounds().contains(mp)) selectionMenuMulti = 1;
    else if (btnMultiRetour.getGlobalBounds().contains(mp)) selectionMenuMulti = 2;

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

    surbrillance(btnMultiMaison, couleurBaseMaison, selectionMenuMulti == 0);
    surbrillance(btnMultiInternet, couleurBaseInternet, selectionMenuMulti == 1);
    surbrillance(btnMultiRetour, couleurBaseRetour, selectionMenuMulti == 2);
}

void Game::renderMenu() {
    window.clear(sf::Color::Black);
    updateMenuHighlight();

    window.draw(*txtTitreMenu);
    window.draw(btnMenuPartieTest);
    window.draw(btnMenuSolo);
    window.draw(btnMenuMulti);
    window.draw(btnMenuQuitter);
    window.draw(*txtMenuPartieTest);
    window.draw(*txtMenuSolo);
    window.draw(*txtMenuMulti);
    window.draw(*txtMenuQuitter);
}

void Game::renderMenuSolo() {
    window.clear(sf::Color::Black);
    updateSoloHighlight();

    window.draw(*txtTitreSolo);
    window.draw(btnSoloFacile);
    window.draw(btnSoloMoyen);
    window.draw(btnSoloDifficile);
    window.draw(btnSoloRetour);
    window.draw(*txtSoloFacile);
    window.draw(*txtSoloMoyen);
    window.draw(*txtSoloDifficile);
    window.draw(*txtSoloRetour);
}

void Game::renderMenuMulti() {
    window.clear(sf::Color::Black);
    updateMultiHighlight();

    window.draw(*txtTitreMulti);
    window.draw(btnMultiMaison);
    window.draw(btnMultiInternet);
    window.draw(btnMultiRetour);
    window.draw(*txtMultiMaison);
    window.draw(*txtMultiInternet);
    window.draw(*txtMultiRetour);
}

void Game::updateFinPartieHighlight() {
    sf::Vector2f mp = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    const bool hover = btnFinMenu.getGlobalBounds().contains(mp);

    if (hover) {
        btnFinMenu.setFillColor({85, 140, 210});
        btnFinMenu.setOutlineColor(sf::Color::Yellow);
        btnFinMenu.setOutlineThickness(3.f);
    } else {
        btnFinMenu.setFillColor({40, 90, 160});
        btnFinMenu.setOutlineColor(sf::Color::White);
        btnFinMenu.setOutlineThickness(2.f);
    }

    centrerTexte(*txtFinMenu, btnFinMenu);
}

void Game::renderFinPartie() {
    window.draw(*spriteFond);

    sf::RectangleShape voile({800.f, 560.f});
    voile.setFillColor({0, 0, 0, 170});
    window.draw(voile);

    updateFinPartieHighlight();

    window.draw(*txtFinTitre);
    window.draw(*txtFinDetail);
    window.draw(btnFinMenu);
    window.draw(*txtFinMenu);
}

void Game::updatePredictionHighlight() {
    sf::Vector2f mp = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    for (int i = 0; i < kPredictionCount; ++i) {
        const bool hovered = btnPredictions[i].getGlobalBounds().contains(mp);
        const bool active = PredictionEngine::predictionTypes()[i] == predictionActive;

        sf::Color base = active ? sf::Color{65, 95, 155} : sf::Color{35, 60, 95};
        if (hovered) {
            base = sf::Color{
                static_cast<std::uint8_t>(std::min(255, base.r + 35)),
                static_cast<std::uint8_t>(std::min(255, base.g + 35)),
                static_cast<std::uint8_t>(std::min(255, base.b + 35))
            };
        }

        btnPredictions[i].setFillColor(base);
        btnPredictions[i].setOutlineColor(active ? sf::Color::Yellow : sf::Color::White);
        btnPredictions[i].setOutlineThickness(active ? 3.f : 2.f);
    }
}

void Game::updateJeuHighlight() {
    sf::Vector2f mp = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    if (btnTirer.getGlobalBounds().contains(mp)) {
        selectionJeu = 0;
    } else if (btnQuitter.getGlobalBounds().contains(mp)) {
        selectionJeu = 1;
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

    surbrillance(btnTirer, couleurBaseTirer, selectionJeu == 0);
    surbrillance(btnQuitter, {180, 30, 30}, selectionJeu == 1);
}

void Game::renderJeu() {
    // Fond tapis vert (texture réelle)
    window.draw(*spriteFond);

    // Bordure decorative
    sf::RectangleShape bordure({740.f, 500.f});
    bordure.setPosition({30.f, 30.f});
    bordure.setFillColor(sf::Color::Transparent);
    bordure.setOutlineThickness(4.f);
    bordure.setOutlineColor({20, 65, 30});
    window.draw(bordure);

    window.draw(*txtTitre);

    renderScoreSelonMode();

    sf::Text txtResultat(font, dernierResultat, 16);
    txtResultat.setFillColor(sf::Color::White);
    txtResultat.setPosition({205.f, 82.f});
    window.draw(txtResultat);

    renderResultatSecondaireSelonMode();

    updatePredictionHighlight();
    for (int i = 0; i < kPredictionCount; ++i) {
        window.draw(btnPredictions[i]);
        window.draw(*txtPredictions[i]);
    }

    sf::Text txtRestant(font, "Cartes restantes : " + std::to_string(kDeckSize - index), 18);
    txtRestant.setFillColor(sf::Color::White);
    txtRestant.setPosition({10.f, 10.f});
    window.draw(txtRestant);

    const float cardW = 150.f;
    const float cardH = 220.f;
    const float cardY = 170.f;
    for (int i = 0; i < 2; ++i) {
        float cardX = (i == 0) ? 175.f : 475.f;
        if (cartes[i]) {
            // Charger la texture si la carte a changé
            if (cartesChargees[i] != cartes[i]) {
                if (texturesCarte[i].loadFromFile(cheminCarte(*cartes[i]))) {
                    spritesCarte[i].emplace(texturesCarte[i]);
                    float sx = cardW / static_cast<float>(texturesCarte[i].getSize().x);
                    float sy = cardH / static_cast<float>(texturesCarte[i].getSize().y);
                    spritesCarte[i]->setScale({sx, sy});
                    spritesCarte[i]->setPosition({cardX, cardY});
                }
                cartesChargees[i] = cartes[i];
            }
            if (spritesCarte[i]) {
                window.draw(*spritesCarte[i]);
            } else {
                dessinerCarte(window, *cartes[i], cardX, cardY, font);
            }
        } else {
            sf::RectangleShape vide({cardW, cardH});
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
        txtFin.setPosition({400.f, 405.f});
        window.draw(txtFin);
    }

    updateJeuHighlight();
    window.draw(btnTirer);
    window.draw(*txtBtnTirer);
    window.draw(btnQuitter);
    window.draw(*txtQuitter);
}
