#pragma once

#include "game_types.h"

class Game;

class TurnResolver {
public:
    explicit TurnResolver(Game& game);

    void memoriserCarteVue(int id);
    void tirerDeuxCartes();
    void appliquerPredictionPourJoueur(int playerIndex);
    void appliquerPrediction();
    void useJoker(JokerAction action);

private:
    Game& game;
};
