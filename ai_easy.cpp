#include "ai_easy.h"

PredictionType AIEasy::choose(std::mt19937& rng) {
    std::uniform_int_distribution<int> dist(0, PredictionEngine::kPredictionCount - 1);
    return PredictionEngine::predictionTypes()[dist(rng)];
}
