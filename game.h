#pragma once

#include "ai_strategy.h"
#include "carte.h"
#include "game_state.h"
#include "game_types.h"
#include "input_controller.h"
#include "prediction_engine.h"
#include "render_controller.h"
#include "turn_resolver.h"
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <array>
#include <deque>
#include <optional>
#include <random>
#include <memory>
#include <string>
#include <vector>

#include "joker_effect.h"

class Game {
public:
    Game();
    bool initialize(char* argv[]);
    void run();

private:
    friend class TurnResolver;
    friend class InputController;
    friend class RenderController;
    enum class Ecran {
        Menu,
        MenuSolo,
        MenuMulti,
        MenuMultiOnline,
        MenuDeck,
        OnlineWaiting,
        Jeu,
        FinPartie
    };

    enum class ModeJeu {
        PartieTest,
        SoloFacile,
        SoloMoyen,
        SoloDifficile,
        MultiMaison,
        MultiOnlineHost,
        MultiOnlineClient
    };

    enum class DeckType {
        Classic,
        Malediction,
        Benit,
        Divin,
        Enfer,
    };

    struct DeckDefinition {
        DeckType type;
        const char* label;
        int bonusCount;
        int malusCount;
        int jokerCount;
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

    sf::RectangleShape btnOnlineHost{{300.f, 58.f}};
    sf::RectangleShape btnOnlineJoin{{300.f, 58.f}};
    sf::RectangleShape btnOnlineBack{{180.f, 45.f}};
    sf::RectangleShape btnOnlineIp{{300.f, 48.f}};
    sf::RectangleShape btnOnlinePort{{300.f, 48.f}};
    std::optional<sf::Text> txtOnlineHost;
    std::optional<sf::Text> txtOnlineJoin;
    std::optional<sf::Text> txtOnlineBack;
    std::optional<sf::Text> txtOnlineTitle;
    std::optional<sf::Text> txtOnlineIpLabel;
    std::optional<sf::Text> txtOnlinePortLabel;
    std::optional<sf::Text> txtOnlineConnectionStatus;

    sf::RectangleShape btnQuitter{{160.f, 55.f}};
    std::optional<sf::Text> txtQuitter;
    std::optional<sf::Text> txtTitre;

    sf::RectangleShape btnTirer{{220.f, 58.f}};
    std::optional<sf::Text> txtBtnTirer;
    sf::RectangleShape btnPasserMain{{220.f, 50.f}};
    std::optional<sf::Text> txtBtnPasserMain;

    sf::RectangleShape btnFinMenu{{280.f, 60.f}};
    std::optional<sf::Text> txtFinMenu;
    std::optional<sf::Text> txtFinTitre;
    std::optional<sf::Text> txtFinDetail;

    sf::Texture textureFond;
    std::optional<sf::Sprite> spriteFond;

    sf::Texture texturesCarte[2];
    std::optional<sf::Sprite> spritesCarte[2];
    const carte* cartesChargees[2] = {nullptr, nullptr};

    GameState state;
    PredictionType& predictionActive = state.predictionActive;
    PredictionType& predictionIA = state.predictionIA;
    PredictionType& dernierePredictionIA = state.dernierePredictionIA;
    JokerAction& pendingJoker = state.pendingJoker;
    ModeJeu modeJeu = ModeJeu::PartieTest;
    DeckType deckSelection = DeckType::Classic;
    ModeJeu prochaineMode = ModeJeu::PartieTest;
    Ecran ecranApresDeck = Ecran::Menu;
    PlayerState (&players)[2] = state.players;
    int& score = state.score;
    int& scoreIA = state.scoreIA;
    int& scoreJ1 = state.scoreJ1;
    int& scoreJ2 = state.scoreJ2;
    int& joueurCourant = state.joueurCourant;
    int (&reussitesConsecutives)[2] = state.reussitesConsecutives;
    int& derniersPoints = state.derniersPoints;
    std::string& dernierResultat = state.dernierResultat;
    std::string& dernierResultatIA = state.dernierResultatIA;
    int& suiteStreakIA = state.suiteStreakIA;
    std::array<bool, kPredictionCount> predictionSelected{};

    std::array<sf::RectangleShape, kPredictionCount> btnPredictions;
    std::array<std::optional<sf::Text>, kPredictionCount> txtPredictions;

    sf::RectangleShape btnDeckOptions[6];
    std::array<std::optional<sf::Text>, 6> txtDeckOptions;
    std::optional<sf::Text> txtDeckTitle;
    std::optional<sf::Text> txtDeckInfo;
    int selectionDeck = 0;
    carte (&paquet)[kDeckSize] = state.paquet;
    std::mt19937& rng = state.rng;
    int& index = state.index;
    carte* (&cartes)[2] = state.cartes;
    bool& fin = state.fin;
    std::vector<int>& cartesVues = state.cartesVues;
    std::deque<int>& cartesVues10 = state.cartesVues10;

    std::string baseDir;
    int& derniereCarteId = state.derniereCarteId; // ID de la derniere carte tiree (utile pour le mode online)

    sf::TcpListener listener;
    sf::TcpSocket socket;
    bool onlineHost = false;
    bool onlineSocketConnected = false;
    bool onlineAwaitingHost = false;
    bool onlineWaitingForDeck = false;
    std::string onlineIp = "127.0.0.1";
    std::string onlinePort = "55001";
    int selectionOnline = 0;
    int myPlayerNumber = 1;
    std::unique_ptr<IAIStrategy> soloAi;
    TurnResolver turnResolver;
    InputController inputController;
    RenderController renderController;

    std::string asset(const std::string& rel) const;
    std::string cheminCarte(const carte& c) const;
    bool loadAssets();
    void setupUi();
    void setupDeckMenuUi();
    void setupPredictionUi();
    void reinitialiserPartie();
    void demarrerPartie(ModeJeu mode);
    void terminerPartie();
    void memoriserCarteVue(int id);
    std::vector<int> cartesConnuesIA() const;
    void configurerIASolo();
    bool estModeSolo() const;
    bool estModeMultiMaison() const;
    void preparerTourSolo();
    void appliquerScoreJoueurSelonMode();
    void appliquerTourSolo();
    void assignPlayerDeck();
    void assignPlayerJokers(int playerIndex);
    void assignPlayerSpecialCards(int playerIndex);
    void appliquerPredictionPourJoueur(int playerIndex);
    void appliquerTourEffectifs(int playerIndex);
    void togglePredictionSelection(int index);
    void updateDeckInfoText();
    void useJoker(JokerAction action);
    void afficherJokersEtInstructions();
    static std::string jokerLabel(JokerAction a);
    static sf::FloatRect jokerButtonBounds(int i);
    void configurerTitreSelonMode();
    void configurerFinPartieSolo(std::string& titre, std::string& detail, sf::Color& couleur) const;
    void configurerFinPartieMulti(std::string& titre, std::string& detail, sf::Color& couleur) const;
    void configurerFinPartieTest(std::string& titre, std::string& detail, sf::Color& couleur) const;
    void tirerDeuxCartes();
    bool predictionReussie(PredictionType type) const;
    void appliquerPrediction();
    bool peutPasserMainMaintenant() const;
    void passerMainVolontairement();

    void handleEvent(const sf::Event& event);
    void handleResized(const sf::Event::Resized& resized);
    void handleKeyPressed(const sf::Event::KeyPressed& key);
    void handleMousePressed(const sf::Event::MouseButtonPressed& click);

    void renderFrame();
    void renderMenu();
    void renderMenuSolo();
    void renderMenuMulti();
    void renderMenuDeck();
    void renderMenuMultiOnline();
    void renderOnlineWaiting();
    void renderJeu();
    void renderScoreSelonMode();
    void renderResultatSecondaireSelonMode();
    void renderFinPartie();
    void updateMenuHighlight();
    void updateSoloHighlight();
    void updateMultiHighlight();
    void updateDeckHighlight();
    void updateOnlineHighlight();
    void updatePredictionHighlight();
    void updateJeuHighlight();
    void updateFinPartieHighlight();

    bool estModeMultijoueur() const;
    bool estModeMultijoueurMaison() const;
    bool estModeMultijoueurOnline() const;
    static std::unique_ptr<IJokerEffect> createJokerEffect(JokerAction action);
    void handleTextEntered(const sf::Event::TextEntered& text);
    bool startOnlineHost(const std::string& port);
    bool startOnlineClient(const std::string& ip, const std::string& port);
    void resetOnlineSession();
    void processNetworkEvents();
    void processPacket(sf::Packet& packet);
    void processRemoteAction(int joueur, int predictionIndex);
    void processRemoteMoveExecution(int joueur, int predictionIndex, int cardIdA, int cardIdB);
    void processRemotePassExecution(int joueur);
    bool sendDeck();
    bool sendExecuteMove(int joueur, int predictionIndex, int cardIdA, int cardIdB);
    bool sendMoveRequest(int predictionIndex);
    bool sendPassRequest();
    bool sendPassExecution(int joueur);
};
