#include "ai_hard.h"

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

bool hasJoker(const std::vector<JokerAction>& jokers, JokerAction action) {
    for (JokerAction j : jokers) {
        if (j == action) {
            return true;
        }
    }
    return false;
}
} // namespace

PredictionType AIHard::choose(const std::vector<int>& knownCards, const carte* currentCard, int suiteStreak, std::mt19937& rng) {
    if (!currentCard) {
        std::uniform_int_distribution<int> dist(0, PredictionEngine::kPredictionCount - 1);
        return PredictionEngine::predictionTypes()[dist(rng)];
    }

    PredictionType bestType = PredictionType::Couleur;
    double bestScore = -1.0;
    for (PredictionType type : PredictionEngine::predictionTypes()) {
        double p = successProbability(type, currentCard->getId(), knownCards);
        if (type == PredictionType::Suite && suiteStreak >= 2) {
            p *= 0.8;
        }
        if (p > bestScore) {
            bestScore = p;
            bestType = type;
        }
    }
    return bestType;
}

std::optional<JokerAction> AIHard::chooseJoker(
    const std::vector<int>& knownCards,
    const std::vector<JokerAction>& jokers,
    const carte* currentCard,
    int suiteStreak,
    std::mt19937& rng
) {
    if (jokers.empty() || !currentCard) {
        return std::nullopt;
    }

    PredictionType bestType = choose(knownCards, currentCard, suiteStreak, rng);
    const double bestProba = successProbability(bestType, currentCard->getId(), knownCards);

    if (bestProba >= 0.68 && hasJoker(jokers, JokerAction::X2)) {
        return JokerAction::X2;
    }
    if (bestProba >= 0.58 && hasJoker(jokers, JokerAction::Plus5)) {
        return JokerAction::Plus5;
    }
    if (bestProba <= 0.42 && hasJoker(jokers, JokerAction::Retry)) {
        return JokerAction::Retry;
    }
    if (bestProba <= 0.36 && hasJoker(jokers, JokerAction::Mix)) {
        return JokerAction::Mix;
    }
    if (bestProba <= 0.50 && hasJoker(jokers, JokerAction::Vision)) {
        return JokerAction::Vision;
    }

    return std::nullopt;
}
