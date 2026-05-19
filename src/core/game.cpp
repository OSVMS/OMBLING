#include "game.h"

#include "ai_easy.h"
#include "ai_hard.h"
#include "ai_medium.h"
#include "ui_helpers.h"
#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <optional>

// Retourne le label court d'un joker pour l'affichage bouton
std::string Game::jokerLabel(JokerAction a) {
    auto joker = createJokerEffect(a);
    return joker ? joker->label() : "?";
}

// Calcule la zone (FloatRect) d'un bouton joker a l'indice i
sf::FloatRect Game::jokerButtonBounds(int i) {
    return { {510.f, 130.f + static_cast<float>(i) * 42.f}, {155.f, 34.f} };
}

Game::Game() : turnResolver(*this), inputController(*this), renderController(*this) {}

bool Game::initialize(char* argv[]) {
    std::filesystem::path exeDir = std::filesystem::canonical(argv[0]).parent_path();
    std::filesystem::path resolvedBase = exeDir;

    // Cherche un dossier asset a partir du dossier de l'executable,
    // puis remonte dans les parents (utile pour build_macos/app).
    for (std::filesystem::path p = exeDir; !p.empty(); p = p.parent_path()) {
        if (std::filesystem::exists(p / "asset")) {
            resolvedBase = p;
            break;
        }
        if (p == p.parent_path()) {
            break;
        }
    }

    baseDir = resolvedBase.string();

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
    turnResolver.memoriserCarteVue(id);
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
    turnResolver.tirerDeuxCartes();
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
        if (!std::filesystem::exists(path)) {
            continue;
        }
        if (font.openFromFile(path)) {
            fontLoaded = true;
            break;
        }
    }

    if (!fontLoaded) {
        return false;
    }

    if (!textureFond.loadFromFile(asset("asset/fond/bg.png"))) {
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

    // ---- Ecran Multi Online ----
    txtOnlineTitle.emplace(font, "Multijoueur Online", 38);
    txtOnlineTitle->setFillColor(sf::Color::White);
    {
        sf::FloatRect tb = txtOnlineTitle->getLocalBounds();
        txtOnlineTitle->setOrigin({tb.position.x + tb.size.x / 2.f, tb.position.y});
        txtOnlineTitle->setPosition({400.f, 52.f});
    }

    btnOnlineIp.setPosition({250.f, 165.f});
    btnOnlinePort.setPosition({250.f, 230.f});
    btnOnlineHost.setPosition({250.f, 295.f});
    btnOnlineJoin.setPosition({250.f, 355.f});
    btnOnlineBack.setPosition({30.f, 468.f});

    btnOnlineIp.setFillColor({50, 50, 70});
    btnOnlinePort.setFillColor({50, 50, 70});
    btnOnlineHost.setFillColor(couleurBaseMaison);
    btnOnlineJoin.setFillColor(couleurBaseInternet);
    btnOnlineBack.setFillColor(couleurBaseRetour);

    for (auto* btn : {&btnOnlineIp, &btnOnlinePort, &btnOnlineHost, &btnOnlineJoin, &btnOnlineBack}) {
        btn->setOutlineThickness(2.f);
        btn->setOutlineColor(sf::Color::White);
    }

    txtOnlineIpLabel.emplace(font, "IP: " + onlineIp, 20);
    txtOnlinePortLabel.emplace(font, "Port: " + onlinePort, 20);
    txtOnlineHost.emplace(font, "Creer session", 22);
    txtOnlineJoin.emplace(font, "Rejoindre session", 22);
    txtOnlineBack.emplace(font, "< Retour", 20);
    txtOnlineConnectionStatus.emplace(font, "", 18);

    txtOnlineIpLabel->setFillColor(sf::Color::White);
    txtOnlinePortLabel->setFillColor(sf::Color::White);
    txtOnlineHost->setFillColor(sf::Color::White);
    txtOnlineJoin->setFillColor(sf::Color::White);
    txtOnlineBack->setFillColor(sf::Color::White);
    txtOnlineConnectionStatus->setFillColor(sf::Color::White);

    centrerTexte(*txtOnlineIpLabel, btnOnlineIp);
    centrerTexte(*txtOnlinePortLabel, btnOnlinePort);
    centrerTexte(*txtOnlineHost, btnOnlineHost);
    centrerTexte(*txtOnlineJoin, btnOnlineJoin);
    centrerTexte(*txtOnlineBack, btnOnlineBack);

    txtOnlineConnectionStatus->setPosition({400.f, 430.f});

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

    btnPasserMain.setPosition({290.f, 504.f});
    btnPasserMain.setFillColor({20, 120, 140});
    btnPasserMain.setOutlineThickness(2.f);
    btnPasserMain.setOutlineColor(sf::Color::White);

    txtBtnPasserMain.emplace(font, "Passer la main", 22);
    txtBtnPasserMain->setFillColor(sf::Color::White);
    centrerTexte(*txtBtnPasserMain, btnPasserMain);

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
    setupDeckMenuUi();
}

void Game::setupDeckMenuUi() {
    txtDeckTitle.emplace(font, "Choix du deck", 42);
    txtDeckTitle->setFillColor(sf::Color::White);
    {
        sf::FloatRect tb = txtDeckTitle->getLocalBounds();
        txtDeckTitle->setOrigin({tb.position.x + tb.size.x / 2.f, tb.position.y});
        txtDeckTitle->setPosition({400.f, 52.f});
    }

    const std::array<std::string, 5> labels = {
        "Classic: 2 jokers",
        "Malediction: 2 jokers",
        "Benit: 2 jokers",
        "Divin: 3 jokers",
        "Enfer: 1 joker"
    };

    for (int i = 0; i < 5; ++i) {
        btnDeckOptions[i].setSize({440.f, 56.f});
        btnDeckOptions[i].setPosition({180.f, 140.f + i * 70.f});
        btnDeckOptions[i].setFillColor({40, 90, 160});
        btnDeckOptions[i].setOutlineThickness(2.f);
        btnDeckOptions[i].setOutlineColor(sf::Color::White);
        txtDeckOptions[i].emplace(font, labels[i], 22);
        txtDeckOptions[i]->setFillColor(sf::Color::White);
        centrerTexte(*txtDeckOptions[i], btnDeckOptions[i]);
    }

    btnDeckOptions[5].setSize({220.f, 50.f});
    btnDeckOptions[5].setPosition({30.f, 468.f});
    btnDeckOptions[5].setFillColor(couleurBaseRetour);
    btnDeckOptions[5].setOutlineThickness(2.f);
    btnDeckOptions[5].setOutlineColor(sf::Color::White);
    txtDeckOptions[5].emplace(font, "Retour", 20);
    txtDeckOptions[5]->setFillColor(sf::Color::White);
    centrerTexte(*txtDeckOptions[5], btnDeckOptions[5]);

    txtDeckInfo.emplace(font, "Selectionne un deck pour commencer.", 18);
    txtDeckInfo->setFillColor(sf::Color::White);
    txtDeckInfo->setPosition({400.f, 470.f});
    updateDeckInfoText();
}

void Game::updateDeckInfoText() {
    const std::array<std::string, 5> descriptions = {
        "Classic: 2 jokers.",
        "Malediction: 2 jokers.",
        "Benit: 2 jokers.",
        "Divin: 3 jokers.",
        "Enfer: 1 joker."
    };
    if (selectionDeck >= 0 && selectionDeck < 5) {
        txtDeckInfo->setString(descriptions[selectionDeck]);
    }
}

void Game::renderMenuDeck() {
    window.clear(sf::Color::Black);
    updateDeckHighlight();
    if (txtDeckTitle) window.draw(*txtDeckTitle);
    for (int i = 0; i < 6; ++i) {
        window.draw(btnDeckOptions[i]);
        if (txtDeckOptions[i]) window.draw(*txtDeckOptions[i]);
    }
    if (txtDeckInfo) window.draw(*txtDeckInfo);
}

void Game::updateDeckHighlight() {
    sf::Vector2f mp = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    for (int i = 0; i < 5; ++i) {
        if (btnDeckOptions[i].getGlobalBounds().contains(mp)) {
            selectionDeck = i;
            break;
        }
    }
    const bool backHover = btnDeckOptions[5].getGlobalBounds().contains(mp);

    for (int i = 0; i < 5; ++i) {
        bool actif = (i == selectionDeck);
        sf::Color base = actif ? sf::Color{70, 120, 190} : sf::Color{40, 90, 160};
        btnDeckOptions[i].setFillColor(base);
        btnDeckOptions[i].setOutlineColor(actif ? sf::Color::Yellow : sf::Color::White);
        btnDeckOptions[i].setOutlineThickness(actif ? 3.f : 2.f);
    }
    btnDeckOptions[5].setFillColor(backHover ? sf::Color{110, 110, 110} : couleurBaseRetour);
}

void Game::assignPlayerDeck() {
    deckSelection = static_cast<DeckType>(selectionDeck);
    assignPlayerJokers(0);
    assignPlayerJokers(1);
    assignPlayerSpecialCards(0);
    assignPlayerSpecialCards(1);
}

void Game::assignPlayerJokers(int playerIndex) {
    static const std::array<JokerAction, 9> possible = {
        JokerAction::X2,
        JokerAction::Next,
        JokerAction::Mix,
        JokerAction::Vision,
        JokerAction::Tirage,
        JokerAction::Retry,
        JokerAction::Swap,
        JokerAction::Plus5,
        JokerAction::Minus5
    };
    auto& state = players[playerIndex];
    state.jokers.clear();
    int jokerCount = 2;
    switch (deckSelection) {
        case DeckType::Classic: jokerCount = 2; break;
        case DeckType::Malediction: jokerCount = 2; break;
        case DeckType::Benit: jokerCount = 2; break;
        case DeckType::Divin: jokerCount = 3; break;
        case DeckType::Enfer: jokerCount = 1; break;
    }
    std::vector<JokerAction> available(possible.begin(), possible.end());
    if (estModeSolo()) {
        available.erase(std::remove(available.begin(), available.end(), JokerAction::Next), available.end());
        available.erase(std::remove(available.begin(), available.end(), JokerAction::Swap), available.end());
    }
    std::shuffle(available.begin(), available.end(), rng);
    for (int i = 0; i < std::min(jokerCount, static_cast<int>(available.size())); ++i) {
        state.jokers.push_back(available[i]);
    }
}

void Game::assignPlayerSpecialCards(int playerIndex) {
    auto& state = players[playerIndex];
    // Cartes speciales desactivees.
    state.specialCards.clear();
}

void Game::togglePredictionSelection(int index) {
    if (index < 0 || index >= kPredictionCount) {
        return;
    }
    // Radio button : sélectionne uniquement la prédiction cliquée
    for (int i = 0; i < kPredictionCount; ++i) {
        predictionSelected[i] = (i == index);
    }
}

void Game::syncPredictionCouranteDepuisJoueur() {
    if (joueurCourant < 1 || joueurCourant > 2) {
        return;
    }
    int idx = players[joueurCourant - 1].chosenPredictionIndex;
    if (idx < 0 || idx >= kPredictionCount) {
        idx = 0;
        players[joueurCourant - 1].chosenPredictionIndex = 0;
    }
    togglePredictionSelection(idx);
    predictionActive = PredictionEngine::predictionTypes()[idx];
}

void Game::appliquerPredictionPourJoueur(int playerIndex) {
    turnResolver.appliquerPredictionPourJoueur(playerIndex);
}

void Game::appliquerPrediction() {
    turnResolver.appliquerPrediction();
}

void Game::useJoker(JokerAction action) {
    turnResolver.useJoker(action);
}

std::unique_ptr<IJokerEffect> Game::createJokerEffect(JokerAction action) {
    switch (action) {
        case JokerAction::X2:     return std::make_unique<JokerX2>();
        case JokerAction::Next:   return std::make_unique<JokerNext>();
        case JokerAction::Mix:    return std::make_unique<JokerMix>();
        case JokerAction::Vision: return std::make_unique<JokerVision>();
        case JokerAction::Tirage: return std::make_unique<JokerTirage>();
        case JokerAction::Retry:  return std::make_unique<JokerRetry>();
        case JokerAction::Swap:   return std::make_unique<JokerSwap>();
        case JokerAction::Plus5:  return std::make_unique<JokerPlus5>();
        case JokerAction::Minus5: return std::make_unique<JokerMinus5>();
        default:                  return nullptr;
    }
}

void Game::afficherJokersEtInstructions() {
    const auto& state = players[joueurCourant - 1];
    if (state.revealedCardId != 0) {
        std::string visionText;
        if (index < kDeckSize) {
            carte card = paquet[index];
            if (card.getId() != state.revealedCardId) {
                for (int i = index; i < kDeckSize; ++i) {
                    if (paquet[i].getId() == state.revealedCardId) {
                        card = paquet[i];
                        break;
                    }
                }
            }
            visionText = "Vision prochaine carte: " + card.getNomValeur() + " de " + card.getNomCouleur();
        } else {
            visionText = "Vision impossible : paquet vide";
        }

        sf::RectangleShape panel({380.f, 34.f});
        panel.setPosition({210.f, 126.f});
        panel.setFillColor({0, 0, 0, 160});
        panel.setOutlineThickness(2.f);
        panel.setOutlineColor({0, 220, 220, 220});
        window.draw(panel);

        sf::Text txtVision(font, visionText, 18);
        txtVision.setFillColor({230, 255, 255});
        centrerTexte(txtVision, panel);
        window.draw(txtVision);
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
    reussitesConsecutives[0] = 0;
    reussitesConsecutives[1] = 0;
    derniersPoints = 0;
    predictionActive = PredictionType::Couleur;
    predictionIA = PredictionType::Couleur;
    dernierePredictionIA = PredictionType::Couleur;
    dernierResultat = "Choisis une prediction puis tire la prochaine carte";
        pendingJoker = JokerAction::None;
    dernierResultatIA = "IA en attente";
    suiteStreakIA = 0;
    for (auto& state : players) {
        state = PlayerState{};
    }
    players[0].chosenPredictionIndex = 0;
    players[1].chosenPredictionIndex = 0;
    syncPredictionCouranteDepuisJoueur();
    assignPlayerDeck();
    fin = false;

    // Tirer la premiere carte automatiquement
    cartes[0] = &paquet[0];
    cartes[1] = nullptr;
    index = 1;
    memoriserCarteVue(cartes[0]->getId());
}

bool Game::estModeMultijoueur() const {
    return modeJeu == ModeJeu::MultiMaison || modeJeu == ModeJeu::MultiOnlineHost || modeJeu == ModeJeu::MultiOnlineClient;
}

bool Game::estModeMultijoueurMaison() const {
    return modeJeu == ModeJeu::MultiMaison;
}

bool Game::estModeMultijoueurOnline() const {
    return modeJeu == ModeJeu::MultiOnlineHost || modeJeu == ModeJeu::MultiOnlineClient;
}

void Game::resetOnlineSession() {
    listener.close();
    socket.disconnect();
    onlineHost = false;
    onlineSocketConnected = false;
    onlineAwaitingHost = false;
    onlineWaitingForDeck = false;
    onlineIp = "127.0.0.1";
    onlinePort = "55001";
    selectionOnline = 0;
    myPlayerNumber = 1;
    if (txtOnlineConnectionStatus) {
        txtOnlineConnectionStatus->setString("");
        txtOnlineConnectionStatus->setPosition({400.f, 430.f});
    }
}

bool Game::startOnlineHost(const std::string& port) {
    resetOnlineSession();
    onlineHost = true;
    myPlayerNumber = 1;
    if (listener.listen(static_cast<unsigned short>(std::stoi(port))) != sf::Socket::Status::Done) {
        if (txtOnlineConnectionStatus) {
            txtOnlineConnectionStatus->setString("Impossible d'ouvrir le port.");
        }
        return false;
    }
    listener.setBlocking(false);
    if (txtOnlineConnectionStatus) {
        txtOnlineConnectionStatus->setString("En attente d'un joueur sur le port " + port + "...");
    }
    return true;
}

bool Game::startOnlineClient(const std::string& ip, const std::string& port) {
    resetOnlineSession();
    onlineHost = false;
    myPlayerNumber = 2;
    onlineIp = ip;
    onlinePort = port;
    socket.setBlocking(true);
    auto maybeAddress = sf::IpAddress::resolve(ip);
    if (!maybeAddress.has_value() || socket.connect(*maybeAddress, static_cast<unsigned short>(std::stoi(port)), sf::seconds(3)) != sf::Socket::Status::Done) {
        if (txtOnlineConnectionStatus) {
            txtOnlineConnectionStatus->setString("Echec de connexion a " + ip + ":" + port);
        }
        socket.disconnect();
        socket.setBlocking(false);
        return false;
    }
    socket.setBlocking(false);
    onlineSocketConnected = true;
    onlineAwaitingHost = true;
    if (txtOnlineConnectionStatus) {
        txtOnlineConnectionStatus->setString("Connecte. En attente du deck du serveur...");
    }
    return true;
}

bool Game::sendDeck() {
    if (!onlineSocketConnected) {
        return false;
    }
    sf::Packet packet;
    packet << int(1);
    for (int i = 0; i < kDeckSize; ++i) {
        packet << paquet[i].getId();
    }
    return socket.send(packet) == sf::Socket::Status::Done;
}

bool Game::sendExecuteMove(int joueur, int predictionIndex, int cardIdA, int cardIdB) {
    if (!onlineSocketConnected) {
        return false;
    }
    sf::Packet packet;
    packet << int(3) << joueur << predictionIndex << cardIdA << cardIdB;
    return socket.send(packet) == sf::Socket::Status::Done;
}

bool Game::sendMoveRequest(int predictionIndex) {
    if (!onlineSocketConnected) {
        return false;
    }
    sf::Packet packet;
    packet << int(2) << myPlayerNumber << predictionIndex;
    return socket.send(packet) == sf::Socket::Status::Done;
}

bool Game::sendPassRequest() {
    if (!onlineSocketConnected) {
        return false;
    }
    sf::Packet packet;
    packet << int(4) << myPlayerNumber;
    return socket.send(packet) == sf::Socket::Status::Done;
}

bool Game::sendPassExecution(int joueur) {
    if (!onlineSocketConnected) {
        return false;
    }
    sf::Packet packet;
    packet << int(5) << joueur;
    return socket.send(packet) == sf::Socket::Status::Done;
}

void Game::processRemoteAction(int joueur, int predictionIndex) {
    if (!estModeMultijoueurOnline()) {
        return;
    }
    predictionActive = PredictionEngine::predictionTypes()[predictionIndex];
    if (joueurCourant != joueur) {
        joueurCourant = joueur;
    }

    const int prevId = cartes[0] ? cartes[0]->getId() : 0;
    tirerDeuxCartes();

    if (onlineHost) {
        const int newId = derniereCarteId;
        if (prevId != 0 && newId != 0) {
            sendExecuteMove(joueur, predictionIndex, prevId, newId);
        }
    }
}

void Game::processRemoteMoveExecution(int joueur, int predictionIndex, int cardIdA, int cardIdB) {
    if (!estModeMultijoueurOnline()) {
        return;
    }
    predictionActive = PredictionEngine::predictionTypes()[predictionIndex];
    if (joueurCourant != joueur) {
        joueurCourant = joueur;
    }

    auto findCard = [this](int id) -> carte* {
        for (int i = 0; i < kDeckSize; ++i) {
            if (paquet[i].getId() == id) {
                return &paquet[i];
            }
        }
        return nullptr;
    };

    // cardIdA = carte précédente (déjà dans cartes[0]), cardIdB = nouvelle carte
    carte* cardB = findCard(cardIdB);
    if (cardB) {
        cartes[1] = cardB;
        memoriserCarteVue(cardB->getId());
        index += 1;
        if (index >= kDeckSize) {
            fin = true;
        }
    }

    appliquerPrediction();
    appliquerScoreJoueurSelonMode();
    appliquerTourSolo();

    // Shift: la nouvelle carte devient la précédente
    cartes[0] = cartes[1];
    cartes[1] = nullptr;
    cartesChargees[0] = nullptr;
    cartesChargees[1] = nullptr;
    spritesCarte[1].reset();

    if (fin) {
        terminerPartie();
    }
}

void Game::processRemotePassExecution(int joueur) {
    if (!estModeMultijoueurOnline()) {
        return;
    }

    if (joueurCourant != joueur) {
        joueurCourant = joueur;
    }

    if (joueur < 1 || joueur > 2) {
        return;
    }

    reussitesConsecutives[joueur - 1] = 0;
    joueurCourant = (joueurCourant == 1) ? 2 : 1;
    dernierResultat = "J" + std::to_string(joueur) + " passe la main a J" + std::to_string(joueurCourant);
}

void Game::processPacket(sf::Packet& packet) {
    int type = 0;
    if (!(packet >> type)) {
        return;
    }
    if (type == 1) {
        int cardId;
        for (int i = 0; i < kDeckSize; ++i) {
            if (!(packet >> cardId)) {
                return;
            }
            int couleur = (cardId - 1) / 13 + 1;
            int valeur = cardId - (couleur - 1) * 13;
            paquet[i] = carte(cardId, valeur, couleur);
        }
        onlineWaitingForDeck = false;
        demarrerPartie(ModeJeu::MultiOnlineClient);
        txtTitre->setString("Multijoueur Online");
        return;
    }
    if (type == 2) {
        int remotePlayer;
        int predictionIndex;
        if (!(packet >> remotePlayer >> predictionIndex)) {
            return;
        }
        if (onlineHost && remotePlayer == 2) {
            processRemoteAction(remotePlayer, predictionIndex);
        }
        return;
    }
    if (type == 3) {
        int remotePlayer;
        int predictionIndex;
        int cardIdA;
        int cardIdB;
        if (!(packet >> remotePlayer >> predictionIndex >> cardIdA >> cardIdB)) {
            return;
        }
        processRemoteMoveExecution(remotePlayer, predictionIndex, cardIdA, cardIdB);
        return;
    }
    if (type == 4) {
        int remotePlayer;
        if (!(packet >> remotePlayer)) {
            return;
        }
        if (onlineHost && remotePlayer == 2 && remotePlayer == joueurCourant && reussitesConsecutives[remotePlayer - 1] >= 2) {
            processRemotePassExecution(remotePlayer);
            sendPassExecution(remotePlayer);
        }
        return;
    }
    if (type == 5) {
        int remotePlayer;
        if (!(packet >> remotePlayer)) {
            return;
        }
        processRemotePassExecution(remotePlayer);
    }
}

void Game::processNetworkEvents() {
    if (ecran == Ecran::OnlineWaiting && onlineHost && !onlineSocketConnected) {
        if (listener.accept(socket) == sf::Socket::Status::Done) {
            socket.setBlocking(false);
            onlineSocketConnected = true;
            if (txtOnlineConnectionStatus) {
                txtOnlineConnectionStatus->setString("Joueur connecte. Envoi du deck...");
            }
            if (sendDeck()) {
                demarrerPartie(ModeJeu::MultiOnlineHost);
                txtTitre->setString("Multijoueur Online");
                if (txtOnlineConnectionStatus) {
                    txtOnlineConnectionStatus->setString("Session demarree !");
                }
                ecran = Ecran::Jeu;
            }
        }
    }

    if (onlineSocketConnected) {
        sf::Packet packet;
        while (true) {
            sf::Socket::Status status = socket.receive(packet);
            if (status == sf::Socket::Status::Done) {
                processPacket(packet);
                packet.clear();
                continue;
            }
            if (status == sf::Socket::Status::NotReady) {
                break;
            }
            resetOnlineSession();
            ecran = Ecran::MenuMulti;
            if (txtOnlineConnectionStatus) {
                txtOnlineConnectionStatus->setString("Connection perdue.");
            }
            break;
        }
    }
}

void Game::demarrerPartie(ModeJeu mode) {
    modeJeu = mode;
    configurerIASolo();
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
    } else if (estModeMultijoueur()) {
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
        processNetworkEvents();
        renderFrame();
    }
}

void Game::handleEvent(const sf::Event& event) {
    inputController.handleEvent(event);
}

void Game::handleResized(const sf::Event::Resized& resized) {
    inputController.handleResized(resized);
}

void Game::handleKeyPressed(const sf::Event::KeyPressed& key) {
    inputController.handleKeyPressed(key);
}

void Game::handleMousePressed(const sf::Event::MouseButtonPressed& click) {
    inputController.handleMousePressed(click);
}

void Game::handleTextEntered(const sf::Event::TextEntered& text) {
    inputController.handleTextEntered(text);
}

void Game::renderFrame() {
    renderController.renderFrame();
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

void Game::updateOnlineHighlight() {
    sf::Vector2f mp = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    if (btnOnlineIp.getGlobalBounds().contains(mp)) selectionOnline = 0;
    else if (btnOnlinePort.getGlobalBounds().contains(mp)) selectionOnline = 1;
    else if (btnOnlineHost.getGlobalBounds().contains(mp)) selectionOnline = 2;
    else if (btnOnlineJoin.getGlobalBounds().contains(mp)) selectionOnline = 3;
    else if (btnOnlineBack.getGlobalBounds().contains(mp)) selectionOnline = 4;

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

    surbrillance(btnOnlineIp, {50, 50, 70}, selectionOnline == 0);
    surbrillance(btnOnlinePort, {50, 50, 70}, selectionOnline == 1);
    surbrillance(btnOnlineHost, couleurBaseMaison, selectionOnline == 2);
    surbrillance(btnOnlineJoin, couleurBaseInternet, selectionOnline == 3);
    surbrillance(btnOnlineBack, couleurBaseRetour, selectionOnline == 4);
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

void Game::renderMenuMultiOnline() {
    window.clear(sf::Color::Black);
    updateOnlineHighlight();

    window.draw(*txtOnlineTitle);
    window.draw(btnOnlineIp);
    window.draw(btnOnlinePort);
    window.draw(btnOnlineHost);
    window.draw(btnOnlineJoin);
    window.draw(btnOnlineBack);
    window.draw(*txtOnlineIpLabel);
    window.draw(*txtOnlinePortLabel);
    window.draw(*txtOnlineHost);
    window.draw(*txtOnlineJoin);
    window.draw(*txtOnlineBack);
    window.draw(*txtOnlineConnectionStatus);
}

void Game::renderOnlineWaiting() {
    window.clear(sf::Color::Black);
    window.draw(*txtOnlineTitle);
    window.draw(*txtOnlineConnectionStatus);
    sf::Text attente(font, "Appuyez Echap pour revenir au menu.", 18);
    attente.setFillColor(sf::Color::White);
    sf::FloatRect tb = attente.getLocalBounds();
    attente.setOrigin({tb.position.x + tb.size.x / 2.f, tb.position.y});
    attente.setPosition({400.f, 500.f});
    window.draw(attente);
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
        const bool active = predictionSelected[i];

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

    if (peutPasserMainMaintenant()) {
        const bool hovered = btnPasserMain.getGlobalBounds().contains(mp);
        sf::Color base = {20, 120, 140};
        if (hovered) {
            base = {
                static_cast<std::uint8_t>(std::min(255, base.r + 55)),
                static_cast<std::uint8_t>(std::min(255, base.g + 55)),
                static_cast<std::uint8_t>(std::min(255, base.b + 55))
            };
            btnPasserMain.setOutlineColor(sf::Color::Yellow);
            btnPasserMain.setOutlineThickness(3.f);
        } else {
            btnPasserMain.setOutlineColor(sf::Color::White);
            btnPasserMain.setOutlineThickness(2.f);
        }
        btnPasserMain.setFillColor(base);
    }
}

void Game::renderJeu() {
    // Fond tapis vert (texture réelle)
    window.draw(*spriteFond);

    window.draw(*txtTitre);

    renderScoreSelonMode();

    std::string deckName;
    switch (deckSelection) {
        case DeckType::Classic: deckName = "Classic"; break;
        case DeckType::Malediction: deckName = "Malediction"; break;
        case DeckType::Benit: deckName = "Benit"; break;
        case DeckType::Divin: deckName = "Divin"; break;
        case DeckType::Enfer: deckName = "Enfer"; break;
    }
    sf::Text txtDeck(font, "Deck: " + deckName, 16);
    txtDeck.setFillColor(sf::Color::White);
    txtDeck.setPosition({10.f, 40.f});
    window.draw(txtDeck);

    const auto& currentPlayerState = players[joueurCourant - 1];

    // --- Boutons jokers (cote droit, cliquables) ---
    {
        sf::Text txtJokerTitle(font, "JOKERS", 15);
        txtJokerTitle.setFillColor({200, 150, 255});
        txtJokerTitle.setPosition({510.f, 110.f});
        window.draw(txtJokerTitle);

        const auto& jokers = currentPlayerState.jokers;
        if (jokers.empty()) {
            sf::Text txtNoJoker(font, "Aucun joker", 14);
            txtNoJoker.setFillColor({150, 150, 150});
            txtNoJoker.setPosition({510.f, 132.f});
            window.draw(txtNoJoker);
        } else {
            sf::Vector2f mp = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            for (int i = 0; i < static_cast<int>(jokers.size()); ++i) {
                sf::FloatRect bounds = jokerButtonBounds(i);
                sf::RectangleShape btn(bounds.size);
                btn.setPosition(bounds.position);
                const bool hovered = bounds.contains(mp);
                const bool selected = (pendingJoker == jokers[i]);
                if (selected)
                    btn.setFillColor({120, 80, 30});
                else if (hovered)
                    btn.setFillColor({100, 60, 130});
                else
                    btn.setFillColor({65, 35, 90});
                btn.setOutlineThickness(selected ? 3.f : 2.f);
                btn.setOutlineColor(selected ? sf::Color::Yellow : sf::Color{200, 150, 255});
                window.draw(btn);
                sf::Text lbl(font, jokerLabel(jokers[i]), 14);
                lbl.setFillColor(sf::Color::White);
                centrerTexte(lbl, btn);
                window.draw(lbl);
            }
        }
    }
    afficherJokersEtInstructions();

    if (estModeMultijoueurOnline()) {
        sf::Text txtOnlineInfo(font, "Vous: J" + std::to_string(myPlayerNumber) + " | Tour: J" + std::to_string(joueurCourant), 16);
        txtOnlineInfo.setFillColor(sf::Color::White);
        txtOnlineInfo.setPosition({10.f, 110.f});
        window.draw(txtOnlineInfo);
    }

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
    const float cardX = 325.f; // centrée dans la fenêtre (800px)
    // Affiche uniquement la carte courante (cartes[0])
    if (cartes[0]) {
        if (cartesChargees[0] != cartes[0]) {
            if (texturesCarte[0].loadFromFile(cheminCarte(*cartes[0]))) {
                spritesCarte[0].emplace(texturesCarte[0]);
                float sx = cardW / static_cast<float>(texturesCarte[0].getSize().x);
                float sy = cardH / static_cast<float>(texturesCarte[0].getSize().y);
                spritesCarte[0]->setScale({sx, sy});
                spritesCarte[0]->setPosition({cardX, cardY});
            }
            cartesChargees[0] = cartes[0];
        }
        if (spritesCarte[0]) {
            window.draw(*spritesCarte[0]);
        } else {
            dessinerCarte(window, *cartes[0], cardX, cardY, font);
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
    if (peutPasserMainMaintenant()) {
        window.draw(btnPasserMain);
        if (txtBtnPasserMain) {
            centrerTexte(*txtBtnPasserMain, btnPasserMain);
            window.draw(*txtBtnPasserMain);
        }
    }
    window.draw(btnQuitter);
    window.draw(*txtQuitter);
}

bool Game::peutPasserMainMaintenant() const {
    if (!estModeMultijoueur() || fin || joueurCourant < 1 || joueurCourant > 2) {
        return false;
    }
    if (estModeMultijoueurOnline() && joueurCourant != myPlayerNumber) {
        return false;
    }
    return reussitesConsecutives[joueurCourant - 1] >= 2;
}

void Game::passerMainVolontairement() {
    if (!peutPasserMainMaintenant()) {
        return;
    }

    const int joueurQuiPasse = joueurCourant;
    reussitesConsecutives[joueurQuiPasse - 1] = 0;
    joueurCourant = (joueurCourant == 1) ? 2 : 1;
    dernierResultat = "J" + std::to_string(joueurQuiPasse) + " passe la main a J" + std::to_string(joueurCourant);
}