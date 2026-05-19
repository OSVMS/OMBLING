#include "ai_strategy.h"

#include "ai_easy.h"
#include "ai_hard.h"
#include "ai_medium.h"

PredictionType EasyAIStrategy::choose(const std::vector<int>&, const carte*, int, std::mt19937& rng) const {
    return AIEasy::choose(rng);
}

std::optional<JokerAction> EasyAIStrategy::chooseJoker(
    const std::vector<int>&,
    const std::vector<JokerAction>& jokers,
    const carte*,
    int,
    std::mt19937& rng
) const {
    return AIEasy::chooseJoker(jokers, rng);
}

PredictionType MediumAIStrategy::choose(const std::vector<int>& knownCards, const carte* currentCard, int, std::mt19937& rng) const {
    return AIMedium::choose(knownCards, currentCard, rng);
}

std::optional<JokerAction> MediumAIStrategy::chooseJoker(
    const std::vector<int>&,
    const std::vector<JokerAction>& jokers,
    const carte*,
    int,
    std::mt19937& rng
) const {
    return AIMedium::chooseJoker(jokers, rng);
}

PredictionType HardAIStrategy::choose(const std::vector<int>& knownCards, const carte* currentCard, int suiteStreak, std::mt19937& rng) const {
    return AIHard::choose(knownCards, currentCard, suiteStreak, rng);
}

std::optional<JokerAction> HardAIStrategy::chooseJoker(
    const std::vector<int>& knownCards,
    const std::vector<JokerAction>& jokers,
    const carte* currentCard,
    int suiteStreak,
    std::mt19937& rng
) const {
    return AIHard::chooseJoker(knownCards, jokers, currentCard, suiteStreak, rng);
}
