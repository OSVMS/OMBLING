#include "ai_hard.h"

PredictionType AIHard::choose(const std::vector<int>& knownCards, int suiteStreak, std::mt19937& rng) {
    return PredictionEngine::chooseForIA(
        knownCards,
        false,
        true,
        suiteStreak,
        rng
    );
}
