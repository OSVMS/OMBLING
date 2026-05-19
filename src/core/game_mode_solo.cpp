#include "game.h"

#include "ai_strategy.h"

#include <algorithm>
#include <memory>

void Game::configurerIASolo() {
    if (modeJeu == ModeJeu::SoloFacile) {
        soloAi = std::make_unique<EasyAIStrategy>();
        return;
    }
    if (modeJeu == ModeJeu::SoloMoyen) {
        soloAi = std::make_unique<MediumAIStrategy>();
        return;
    }
    if (modeJeu == ModeJeu::SoloDifficile) {
        soloAi = std::make_unique<HardAIStrategy>();
        return;
    }
    soloAi.reset();
}

bool Game::estModeSolo() const {
    return modeJeu == ModeJeu::SoloFacile || modeJeu == ModeJeu::SoloMoyen || modeJeu == ModeJeu::SoloDifficile;
}

void Game::preparerTourSolo() {
    if (!estModeSolo()) {
        return;
    }

    if (!soloAi) {
        configurerIASolo();
    }
    if (!soloAi) {
        return;
    }

    auto maybeJoker = soloAi->chooseJoker(cartesConnuesIA(), players[1].jokers, cartes[0], suiteStreakIA, rng);
    if (maybeJoker.has_value()) {
        appliquerJokerIASolo(*maybeJoker);
    }

    predictionIA = soloAi->choose(cartesConnuesIA(), cartes[0], suiteStreakIA, rng);

    if (predictionIA == PredictionType::Suite && dernierePredictionIA == PredictionType::Suite) {
        ++suiteStreakIA;
    } else if (predictionIA == PredictionType::Suite) {
        suiteStreakIA = 1;
    } else {
        suiteStreakIA = 0;
    }
    dernierePredictionIA = predictionIA;
}

void Game::appliquerTourSolo() {
    if (!estModeSolo()) {
        return;
    }

    auto& iaState = players[1];
    const int pointsIA = PredictionEngine::points(predictionIA);
    int delta = 0;
    if (predictionReussie(predictionIA)) {
        delta = pointsIA;
    } else {
        delta = -pointsIA;
    }

    if (iaState.immunity && delta < 0) {
        delta = 0;
        iaState.immunity = false;
    }

    delta *= iaState.multiplier;
    delta += iaState.temporaryModifier;

    iaState.multiplier = 1;
    iaState.temporaryModifier = 0;

    scoreIA += delta;
    if (delta >= 0) {
        dernierResultatIA = "IA " + PredictionEngine::name(predictionIA) + ": Reussi +" + std::to_string(delta);
    } else {
        dernierResultatIA = "IA " + PredictionEngine::name(predictionIA) + ": Rate " + std::to_string(delta) + " pts";
    }
}

void Game::appliquerJokerIASolo(JokerAction action) {
    auto& iaState = players[1];
    auto it = std::find(iaState.jokers.begin(), iaState.jokers.end(), action);
    if (it == iaState.jokers.end()) {
        return;
    }

    iaState.jokers.erase(it);
    auto joker = createJokerEffect(action);
    if (!joker) {
        return;
    }

    std::string message;
    joker->apply(players, 2, paquet, index, rng, false, false, message);
}

void Game::configurerFinPartieSolo(std::string& titre, std::string& detail, sf::Color& couleur) const {
    if (score > scoreIA) {
        titre = "Victoire";
        couleur = {110, 230, 120};
    } else {
        titre = "Defaite";
        couleur = {240, 110, 110};
    }
    detail = "Joueur: " + std::to_string(score) + "  |  IA: " + std::to_string(scoreIA);
}

void Game::renderResultatSecondaireSelonMode() {
    if (!estModeSolo()) {
        return;
    }

    sf::Text txtResultatIA(font, dernierResultatIA, 16);
    txtResultatIA.setFillColor({215, 235, 255});
    txtResultatIA.setPosition({205.f, 104.f});
    window.draw(txtResultatIA);
}
