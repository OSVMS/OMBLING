#include "game.h"

#include "ai_easy.h"
#include "ai_hard.h"
#include "ai_medium.h"

bool Game::estModeSolo() const {
    return modeJeu == ModeJeu::SoloFacile || modeJeu == ModeJeu::SoloMoyen || modeJeu == ModeJeu::SoloDifficile;
}

void Game::preparerTourSolo() {
    if (!estModeSolo()) {
        return;
    }

    if (modeJeu == ModeJeu::SoloFacile) {
        predictionIA = AIEasy::choose(rng);
    } else if (modeJeu == ModeJeu::SoloMoyen) {
        predictionIA = AIMedium::choose(cartesConnuesIA(), rng);
    } else {
        predictionIA = AIHard::choose(cartesConnuesIA(), suiteStreakIA, rng);
    }

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

    const int pointsIA = PredictionEngine::points(predictionIA);
    if (predictionReussie(predictionIA)) {
        scoreIA += pointsIA;
        dernierResultatIA = "IA " + PredictionEngine::name(predictionIA) + ": Reussi +" + std::to_string(pointsIA);
    } else {
        scoreIA -= pointsIA;
        dernierResultatIA = "IA " + PredictionEngine::name(predictionIA) + ": Rate " + std::to_string(-pointsIA) + " pts";
    }
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
