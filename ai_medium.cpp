#include "ai_medium.h"

PredictionType AIMedium::choose(const std::vector<int>& knownCards, std::mt19937& rng) {
    return PredictionEngine::chooseForIA(
        knownCards,
        true,
        false,
        0,
        rng
    );
}
