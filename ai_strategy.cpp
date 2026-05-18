#include "ai_strategy.h"

#include "ai_easy.h"
#include "ai_hard.h"
#include "ai_medium.h"

PredictionType EasyAIStrategy::choose(const std::vector<int>&, int, std::mt19937& rng) const {
    return AIEasy::choose(rng);
}

PredictionType MediumAIStrategy::choose(const std::vector<int>& knownCards, int, std::mt19937& rng) const {
    return AIMedium::choose(knownCards, rng);
}

PredictionType HardAIStrategy::choose(const std::vector<int>& knownCards, int suiteStreak, std::mt19937& rng) const {
    return AIHard::choose(knownCards, suiteStreak, rng);
}
