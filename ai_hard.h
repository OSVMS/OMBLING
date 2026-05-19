#pragma once

#include "carte.h"
#include "game_types.h"
#include "prediction_engine.h"

#include <optional>
#include <random>
#include <vector>

class AIHard {
public:
    static PredictionType choose(const std::vector<int>& knownCards, const carte* currentCard, int suiteStreak, std::mt19937& rng);
    static std::optional<JokerAction> chooseJoker(
        const std::vector<int>& knownCards,
        const std::vector<JokerAction>& jokers,
        const carte* currentCard,
        int suiteStreak,
        std::mt19937& rng
    );
};
