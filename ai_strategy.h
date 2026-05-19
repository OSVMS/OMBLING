#pragma once

#include "carte.h"
#include "game_types.h"
#include "prediction_engine.h"

#include <optional>
#include <random>
#include <vector>

class IAIStrategy {
public:
    virtual ~IAIStrategy() = default;
    virtual PredictionType choose(const std::vector<int>& knownCards, const carte* currentCard, int suiteStreak, std::mt19937& rng) const = 0;
    virtual std::optional<JokerAction> chooseJoker(
        const std::vector<int>& knownCards,
        const std::vector<JokerAction>& jokers,
        const carte* currentCard,
        int suiteStreak,
        std::mt19937& rng
    ) const = 0;
};

class EasyAIStrategy final : public IAIStrategy {
public:
    PredictionType choose(const std::vector<int>& knownCards, const carte* currentCard, int suiteStreak, std::mt19937& rng) const override;
    std::optional<JokerAction> chooseJoker(
        const std::vector<int>& knownCards,
        const std::vector<JokerAction>& jokers,
        const carte* currentCard,
        int suiteStreak,
        std::mt19937& rng
    ) const override;
};

class MediumAIStrategy final : public IAIStrategy {
public:
    PredictionType choose(const std::vector<int>& knownCards, const carte* currentCard, int suiteStreak, std::mt19937& rng) const override;
    std::optional<JokerAction> chooseJoker(
        const std::vector<int>& knownCards,
        const std::vector<JokerAction>& jokers,
        const carte* currentCard,
        int suiteStreak,
        std::mt19937& rng
    ) const override;
};

class HardAIStrategy final : public IAIStrategy {
public:
    PredictionType choose(const std::vector<int>& knownCards, const carte* currentCard, int suiteStreak, std::mt19937& rng) const override;
    std::optional<JokerAction> chooseJoker(
        const std::vector<int>& knownCards,
        const std::vector<JokerAction>& jokers,
        const carte* currentCard,
        int suiteStreak,
        std::mt19937& rng
    ) const override;
};
