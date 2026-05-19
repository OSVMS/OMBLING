#include "ai_easy.h"

PredictionType AIEasy::choose(std::mt19937& rng) {
    std::uniform_int_distribution<int> dist(0, PredictionEngine::kPredictionCount - 1);
    return PredictionEngine::predictionTypes()[dist(rng)];
}

std::optional<JokerAction> AIEasy::chooseJoker(const std::vector<JokerAction>& jokers, std::mt19937& rng) {
    if (jokers.empty()) {
        return std::nullopt;
    }

    // Aleatoire total: meme chance de ne rien jouer que de jouer un joker.
    std::uniform_int_distribution<int> dist(0, static_cast<int>(jokers.size()));
    const int pick = dist(rng);
    if (pick == 0) {
        return std::nullopt;
    }
    return jokers[pick - 1];
}
