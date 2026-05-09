#pragma once

#include "prediction_engine.h"

#include <random>
#include <vector>

class AIHard {
public:
    static PredictionType choose(const std::vector<int>& knownCards, int suiteStreak, std::mt19937& rng);
};
