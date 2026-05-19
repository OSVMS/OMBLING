#include "ai_medium.h"

#include <array>

namespace {
void decodeId(int id, int& value, int& suit) {
    value = ((id - 1) % 13) + 1;
    suit = ((id - 1) / 13) + 1;
}

double successProbability(PredictionType type, int currentCardId, const std::vector<int>& knownCards) {
    if (currentCardId < 1 || currentCardId > 52) {
        return 0.0;
    }

    std::array<bool, 53> seen{};
    for (int id : knownCards) {
        if (id >= 1 && id <= 52) {
            seen[id] = true;
        }
    }

    int gv = 0;
    int gc = 0;
    decodeId(currentCardId, gv, gc);

    int remaining = 0;
    int success = 0;
    for (int id = 1; id <= 52; ++id) {
        if (seen[id]) {
            continue;
        }
        int dv = 0;
        int dc = 0;
        decodeId(id, dv, dc);
        ++remaining;
        if (PredictionEngine::isSuccessful(type, gv, gc, dv, dc)) {
            ++success;
        }
    }

    if (remaining == 0) {
        return 0.0;
    }
    return static_cast<double>(success) / static_cast<double>(remaining);
}
} // namespace

PredictionType AIMedium::choose(const std::vector<int>& knownCards, const carte* currentCard, std::mt19937& rng) {
    if (!currentCard) {
        std::uniform_int_distribution<int> dist(0, PredictionEngine::kPredictionCount - 1);
        return PredictionEngine::predictionTypes()[dist(rng)];
    }

    PredictionType bestType = PredictionType::Couleur;
    double bestProba = -1.0;
    for (PredictionType type : PredictionEngine::predictionTypes()) {
        const double p = successProbability(type, currentCard->getId(), knownCards);
        if (p > bestProba) {
            bestProba = p;
            bestType = type;
        }
    }
    return bestType;
}

std::optional<JokerAction> AIMedium::chooseJoker(const std::vector<JokerAction>& jokers, std::mt19937& rng) {
    if (jokers.empty()) {
        return std::nullopt;
    }

    // Medium garde un comportement joker aleatoire.
    std::uniform_int_distribution<int> dist(0, static_cast<int>(jokers.size()));
    const int pick = dist(rng);
    if (pick == 0) {
        return std::nullopt;
    }
    return jokers[pick - 1];
}
