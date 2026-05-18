#pragma once

#include "prediction_engine.h"

#include <random>
#include <vector>

class IAIStrategy {
public:
    virtual ~IAIStrategy() = default;
    virtual PredictionType choose(const std::vector<int>& knownCards, int suiteStreak, std::mt19937& rng) const = 0;
};

class EasyAIStrategy final : public IAIStrategy {
public:
    PredictionType choose(const std::vector<int>& knownCards, int suiteStreak, std::mt19937& rng) const override;
};

class MediumAIStrategy final : public IAIStrategy {
public:
    PredictionType choose(const std::vector<int>& knownCards, int suiteStreak, std::mt19937& rng) const override;
};

class HardAIStrategy final : public IAIStrategy {
public:
    PredictionType choose(const std::vector<int>& knownCards, int suiteStreak, std::mt19937& rng) const override;
};
