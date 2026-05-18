#include "turn_resolver.h"

#include "game.h"

#include <algorithm>
#include <array>
#include <random>
#include <string>

TurnResolver::TurnResolver(Game& game) : game(game) {}

void TurnResolver::memoriserCarteVue(int id) {
    game.cartesVues.push_back(id);
    game.cartesVues10.push_back(id);
    if (game.cartesVues10.size() > 10) {
        game.cartesVues10.pop_front();
    }
}

void TurnResolver::tirerDeuxCartes() {
    if (game.fin || game.index >= game.kDeckSize) {
        game.fin = true;
        return;
    }

    if (game.pendingJoker != JokerAction::None) {
        useJoker(game.pendingJoker);
        game.pendingJoker = JokerAction::None;
    }

    game.preparerTourSolo();

    game.cartes[1] = &game.paquet[game.index];
    game.derniereCarteId = game.cartes[1]->getId();
    memoriserCarteVue(game.cartes[1]->getId());
    game.index += 1;

    if (game.index >= game.kDeckSize) {
        game.fin = true;
    }

    if (game.joueurCourant >= 1 && game.joueurCourant <= 2) {
        game.players[game.joueurCourant - 1].revealedCardId = 0;
    }

    appliquerPrediction();
    game.appliquerScoreJoueurSelonMode();
    game.appliquerTourSolo();

    game.cartes[0] = game.cartes[1];
    game.cartes[1] = nullptr;
    game.cartesChargees[0] = nullptr;
    game.cartesChargees[1] = nullptr;
    game.spritesCarte[1].reset();

    if (game.joueurCourant >= 1 && game.joueurCourant <= 2) {
        auto& state = game.players[game.joueurCourant - 1];
        if (state.extraDraws > 0 && !game.fin) {
            state.extraDraws -= 1;
            tirerDeuxCartes();
            return;
        }
    }

    if (game.fin) {
        game.terminerPartie();
    }
}

void TurnResolver::appliquerPredictionPourJoueur(int playerIndex) {
    static const std::array<JokerAction, 8> rewardPool = {
        JokerAction::X2,
        JokerAction::Next,
        JokerAction::Mix,
        JokerAction::Vision,
        JokerAction::Tirage,
        JokerAction::Retry,
        JokerAction::Swap,
        JokerAction::Plus5
    };

    const auto& activePredictions = PredictionEngine::predictionTypes();
    int selectedCount = 0;
    int totalPoints = 0;
    bool anyFailure = false;
    auto& state = game.players[playerIndex];

    if (!game.cartes[0] || !game.cartes[1]) {
        game.derniersPoints = 0;
        return;
    }

    for (int i = 0; i < game.kPredictionCount; ++i) {
        if (!game.predictionSelected[i]) {
            continue;
        }
        ++selectedCount;
        const PredictionType type = activePredictions[i];
        bool success = game.predictionReussie(type);
        int points = PredictionEngine::points(type);
        if (success) {
            totalPoints += points;
        } else {
            totalPoints -= points;
            anyFailure = true;
        }
    }

    if (selectedCount == 0) {
        game.derniersPoints = 0;
        game.dernierResultat = "Aucune prediction selectionnee";
        return;
    }

    if (state.immunity && totalPoints < 0) {
        totalPoints = 0;
        state.immunity = false;
        game.dernierResultat = "Immunite active : aucune perte de points";
    } else {
        game.dernierResultat = "";
        if (totalPoints >= 0) {
            game.dernierResultat = "Reussi: +" + std::to_string(totalPoints) + " pts";
        } else {
            game.dernierResultat = "Rate: " + std::to_string(totalPoints) + " pts";
        }
    }

    totalPoints *= state.multiplier;
    totalPoints += state.temporaryModifier;

    if (state.multiplier > 1 || state.temporaryModifier != 0) {
        game.dernierResultat += " (bonus actif)";
    }

    if (totalPoints > 0 && !anyFailure) {
        state.successStreak += 1;
        if (state.successStreak >= 3) {
            std::uniform_int_distribution<int> dist(0, static_cast<int>(rewardPool.size()) - 1);
            const JokerAction gainedJoker = rewardPool[dist(game.rng)];
            state.jokers.push_back(gainedJoker);
            state.successStreak = 0;
            game.dernierResultat += " | Serie x3 : joker gagne (" + game.jokerLabel(gainedJoker) + ")";
        }
    } else if (totalPoints <= 0) {
        state.successStreak = 0;
    }

    state.multiplier = 1;
    state.temporaryModifier = 0;
    state.retryAvailable = true;
    game.derniersPoints = totalPoints;
}

void TurnResolver::appliquerPrediction() {
    if (game.estModeMultijoueur()) {
        appliquerPredictionPourJoueur(game.joueurCourant - 1);
        return;
    }
    appliquerPredictionPourJoueur(0);
}

void TurnResolver::useJoker(JokerAction action) {
    if (game.joueurCourant < 1 || game.joueurCourant > 2) {
        return;
    }
    auto& state = game.players[game.joueurCourant - 1];
    auto it = std::find(state.jokers.begin(), state.jokers.end(), action);
    if (it == state.jokers.end()) {
        game.dernierResultat = "Joker non disponible";
        return;
    }
    state.jokers.erase(it);

    auto joker = game.createJokerEffect(action);
    if (joker) {
        joker->apply(
            &game.players[0],
            game.joueurCourant,
            game.paquet,
            game.index,
            game.rng,
            game.estModeMultijoueur(),
            game.estModeMultijoueurOnline(),
            game.dernierResultat
        );
    } else {
        game.dernierResultat = "Joker inconnu";
    }
}
