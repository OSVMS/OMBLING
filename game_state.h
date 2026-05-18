#pragma once

#include "carte.h"
#include "game_types.h"
#include "prediction_engine.h"

#include <deque>
#include <random>
#include <string>
#include <vector>

struct GameState {
    static constexpr int kDeckSize = 52;

    PredictionType predictionActive = PredictionType::Couleur;
    PredictionType predictionIA = PredictionType::Couleur;
    PredictionType dernierePredictionIA = PredictionType::Couleur;
    JokerAction pendingJoker = JokerAction::None;

    PlayerState players[2];
    int score = 0;
    int scoreIA = 0;
    int scoreJ1 = 0;
    int scoreJ2 = 0;
    int joueurCourant = 1;
    int derniersPoints = 0;
    std::string dernierResultat = "Choisis une prediction puis tire 2 cartes";
    std::string dernierResultatIA = "IA en attente";
    int suiteStreakIA = 0;

    carte paquet[kDeckSize];
    std::mt19937 rng{std::random_device{}()};
    int index = 0;
    carte* cartes[2] = {nullptr, nullptr};
    bool fin = false;
    std::vector<int> cartesVues;
    std::deque<int> cartesVues10;
    int derniereCarteId = 0;
};
