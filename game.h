#pragma once

#include "carte.h"
#include "prediction_engine.h"
#include <SFML/Graphics.hpp>
#include <array>
#include <deque>
#include <optional>
#include <random>
#include <string>
#include <vector>

class Game {
public:
    bool initialize(char* argv[]);
    void run();

private:
    enum class Ecran {
        Menu,
        MenuSolo,
        MenuMulti,
        Jeu,
        FinPartie
    };

    enum class ModeJeu {
        PartieTest,
        SoloFacile,
        SoloMoyen,
        SoloDifficile,
        MultiMaison
    };

    static constexpr int kDeckSize = 52;
    static constexpr int kPredictionCount = PredictionEngine::kPredictionCount;

    sf::RenderWindow window{sf::VideoMode({800, 560}), "OMBLING"};
    sf::View vue{sf::FloatRect({0.f, 0.f}, {800.f, 560.f})};

    sf::Font font;

    Ecran ecran = Ecran::Menu;
    int selectionMenu = 0;
    int selectionMenuSolo = 0;
    int selectionMenuMulti = 0;
    int selectionJeu = 0;

    const sf::Color couleurBasePartieTest{28, 116, 58};
    const sf::Color couleurBaseSolo{40, 90, 160};
    const sf::Color couleurBaseMulti{100, 40, 160};
    const sf::Color couleurBaseQuitter{160, 40, 40};
    const sf::Color couleurBaseRetour{80, 80, 80};
    const sf::Color couleurBaseFacile{30, 140, 30};
    const sf::Color couleurBaseMoyen{180, 110, 0};
    const sf::Color couleurBaseDifficile{160, 30, 30};
    const sf::Color couleurBaseMaison{40, 90, 160};
    const sf::Color couleurBaseInternet{0, 130, 130};
    const sf::Color couleurBaseTirer{180, 120, 0};

    std::optional<sf::Text> txtTitreMenu;
    std::optional<sf::Text> txtTitreSolo;
    std::optional<sf::Text> txtTitreMulti;

    // Menu principal
    sf::RectangleShape btnMenuPartieTest{{320.f, 62.f}};
    sf::RectangleShape btnMenuSolo{{320.f, 62.f}};
    sf::RectangleShape btnMenuMulti{{320.f, 62.f}};
    sf::RectangleShape btnMenuQuitter{{320.f, 62.f}};
    std::optional<sf::Text> txtMenuPartieTest;
    std::optional<sf::Text> txtMenuSolo;
    std::optional<sf::Text> txtMenuMulti;
    std::optional<sf::Text> txtMenuQuitter;

    // Sous-menu Solo
    sf::RectangleShape btnSoloFacile{{300.f, 58.f}};
    sf::RectangleShape btnSoloMoyen{{300.f, 58.f}};
    sf::RectangleShape btnSoloDifficile{{300.f, 58.f}};
    sf::RectangleShape btnSoloRetour{{180.f, 45.f}};
    std::optional<sf::Text> txtSoloFacile;
    std::optional<sf::Text> txtSoloMoyen;
    std::optional<sf::Text> txtSoloDifficile;
    std::optional<sf::Text> txtSoloRetour;

    // Sous-menu Multi
    sf::RectangleShape btnMultiMaison{{300.f, 58.f}};
    sf::RectangleShape btnMultiInternet{{300.f, 58.f}};
    sf::RectangleShape btnMultiRetour{{180.f, 45.f}};
    std::optional<sf::Text> txtMultiMaison;
    std::optional<sf::Text> txtMultiInternet;
    std::optional<sf::Text> txtMultiRetour;

    sf::RectangleShape btnQuitter{{160.f, 55.f}};
    std::optional<sf::Text> txtQuitter;
    std::optional<sf::Text> txtTitre;

    sf::RectangleShape btnTirer{{220.f, 58.f}};
    std::optional<sf::Text> txtBtnTirer;

    sf::RectangleShape btnFinMenu{{280.f, 60.f}};
    std::optional<sf::Text> txtFinMenu;
    std::optional<sf::Text> txtFinTitre;
    std::optional<sf::Text> txtFinDetail;

    sf::Texture textureFond;
    std::optional<sf::Sprite> spriteFond;

    sf::Texture texturesCarte[2];
    std::optional<sf::Sprite> spritesCarte[2];
    const carte* cartesChargees[2] = {nullptr, nullptr};

    PredictionType predictionActive = PredictionType::Couleur;
    PredictionType predictionIA = PredictionType::Couleur;
    PredictionType dernierePredictionIA = PredictionType::Couleur;
    ModeJeu modeJeu = ModeJeu::PartieTest;
    int score = 0;
    int scoreIA = 0;
    int scoreJ1 = 0;
    int scoreJ2 = 0;
    int joueurCourant = 1;
    int derniersPoints = 0;
    std::string dernierResultat = "Choisis une prediction puis tire 2 cartes";
    std::string dernierResultatIA = "IA en attente";
    int suiteStreakIA = 0;

    std::array<sf::RectangleShape, kPredictionCount> btnPredictions;
    std::array<std::optional<sf::Text>, kPredictionCount> txtPredictions;
    carte paquet[kDeckSize];
    std::mt19937 rng{std::random_device{}()};
    int index = 0;
    carte* cartes[2] = {nullptr, nullptr};
    bool fin = false;
    std::vector<int> cartesVues;
    std::deque<int> cartesVues10;

    std::string baseDir;

    std::string asset(const std::string& rel) const;
    std::string cheminCarte(const carte& c) const;
    bool loadAssets();
    void setupUi();
    void setupPredictionUi();
    void reinitialiserPartie();
    void demarrerPartie(ModeJeu mode);
    void terminerPartie();
    void memoriserCarteVue(int id);
    std::vector<int> cartesConnuesIA() const;
    bool estModeSolo() const;
    bool estModeMultiMaison() const;
    void preparerTourSolo();
    void appliquerScoreJoueurSelonMode();
    void appliquerTourSolo();
    void configurerTitreSelonMode();
    void configurerFinPartieSolo(std::string& titre, std::string& detail, sf::Color& couleur) const;
    void configurerFinPartieMulti(std::string& titre, std::string& detail, sf::Color& couleur) const;
    void configurerFinPartieTest(std::string& titre, std::string& detail, sf::Color& couleur) const;
    void tirerDeuxCartes();
    bool predictionReussie(PredictionType type) const;
    void appliquerPrediction();

    void handleEvent(const sf::Event& event);
    void handleResized(const sf::Event::Resized& resized);
    void handleKeyPressed(const sf::Event::KeyPressed& key);
    void handleMousePressed(const sf::Event::MouseButtonPressed& click);

    void renderFrame();
    void renderMenu();
    void renderMenuSolo();
    void renderMenuMulti();
    void renderJeu();
    void renderScoreSelonMode();
    void renderResultatSecondaireSelonMode();
    void renderFinPartie();
    void updateMenuHighlight();
    void updateSoloHighlight();
    void updateMultiHighlight();
    void updatePredictionHighlight();
    void updateJeuHighlight();
    void updateFinPartieHighlight();
};
