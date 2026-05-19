#pragma once

#include "prediction_engine.h"
#include "game_types.h"

#include <optional>
#include <random>
#include <vector>

class AIEasy {
public:
    static PredictionType choose(std::mt19937& rng);
    static std::optional<JokerAction> chooseJoker(const std::vector<JokerAction>& jokers, std::mt19937& rng);
};
