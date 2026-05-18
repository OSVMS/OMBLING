#include "prediction_engine.h"

#include <algorithm>
#include <cmath>

namespace {
constexpr std::array<PredictionType, PredictionEngine::kPredictionCount> kTypes{
    PredictionType::Couleur,
    PredictionType::Signe,
    PredictionType::Inferieur,
    PredictionType::Superieur,
    PredictionType::Suite,
    PredictionType::Paire
};

constexpr std::array<int, PredictionEngine::kPredictionCount> kPoints{2, 4, 2, 2, 10, 12};

const std::array<std::string, PredictionEngine::kPredictionCount> kLabels{
    "Couleur: meme couleur (+2)",
    "Signe: meme signe (+4)",
    "Inferieur (+2)",
    "Superieur (+2)",
    "Suite (+10)",
    "Paire (+12)"
};
} // namespace

const std::array<PredictionType, PredictionEngine::kPredictionCount>& PredictionEngine::predictionTypes() {
    return kTypes;
}

const std::array<std::string, PredictionEngine::kPredictionCount>& PredictionEngine::predictionLabels() {
    return kLabels;
}

int PredictionEngine::points(PredictionType type) {
    for (int i = 0; i < kPredictionCount; ++i) {
        if (kTypes[i] == type) {
            return kPoints[i];
        }
    }
    return 0;
}

std::string PredictionEngine::name(PredictionType type) {
    switch (type) {
        case PredictionType::Couleur:  return "Couleur";
        case PredictionType::Signe:    return "Signe";
        case PredictionType::Inferieur:return "Inferieur";
        case PredictionType::Superieur:return "Superieur";
        case PredictionType::Suite:    return "Suite";
        case PredictionType::Paire:    return "Paire";
    }
    return "?";
}

bool PredictionEngine::isSuccessful(
    PredictionType type,
    int gaucheValeur,
    int gaucheCouleur,
    int droiteValeur,
    int droiteCouleur
) {
    switch (type) {
        case PredictionType::Couleur: {
            const bool gaucheRouge = (gaucheCouleur == 1 || gaucheCouleur == 2);
            const bool droiteRouge = (droiteCouleur == 1 || droiteCouleur == 2);
            return gaucheRouge == droiteRouge;
        }
        case PredictionType::Signe:
            return gaucheCouleur == droiteCouleur;
        case PredictionType::Inferieur:
            return droiteValeur < gaucheValeur;
        case PredictionType::Superieur:
            return droiteValeur > gaucheValeur;
        case PredictionType::Suite:
            return std::abs(gaucheValeur - droiteValeur) == 1;
        case PredictionType::Paire:
            return gaucheValeur == droiteValeur;
    }
    return false;
}

PredictionType PredictionEngine::chooseForIA(
    const std::vector<int>& knownCards,
    bool weightedMedium,
    bool hardMode,
    int suiteStreak,
    std::mt19937& rng
) {
    constexpr int kDeckSize = 52;
    std::vector<bool> dispo(kDeckSize + 1, true);
    for (int id : knownCards) {
        if (id >= 1 && id <= kDeckSize) {
            dispo[id] = false;
        }
    }

    std::vector<int> inconnues;
    inconnues.reserve(kDeckSize);
    for (int id = 1; id <= kDeckSize; ++id) {
        if (dispo[id]) {
            inconnues.push_back(id);
        }
    }

    if (inconnues.size() < 2) {
        std::uniform_int_distribution<int> dist(0, kPredictionCount - 1);
        return kTypes[dist(rng)];
    }

    struct OptionIA {
        PredictionType type;
        double proba;
        double scoreDecision;
    };
    std::vector<OptionIA> options;
    options.reserve(kPredictionCount);

    auto idVersVC = [](int id, int& v, int& c) {
        v = ((id - 1) % 13) + 1;
        c = ((id - 1) / 13) + 1;
    };

    const double totalPaires = static_cast<double>(inconnues.size()) * static_cast<double>(inconnues.size() - 1);

    for (int i = 0; i < kPredictionCount; ++i) {
        const PredictionType type = kTypes[i];
        long long succes = 0;

        for (size_t a = 0; a < inconnues.size(); ++a) {
            int gv = 0, gc = 0;
            idVersVC(inconnues[a], gv, gc);
            for (size_t b = 0; b < inconnues.size(); ++b) {
                if (a == b) {
                    continue;
                }
                int dv = 0, dc = 0;
                idVersVC(inconnues[b], dv, dc);
                if (isSuccessful(type, gv, gc, dv, dc)) {
                    ++succes;
                }
            }
        }

        const double proba = static_cast<double>(succes) / totalPaires;
        const double esperance = proba * static_cast<double>(kPoints[i]);

        double scoreDecision = esperance;
        if (hardMode && type == PredictionType::Suite && suiteStreak >= 2) {
            scoreDecision -= 0.30 * static_cast<double>(suiteStreak - 1);
        }

        options.push_back({type, proba, scoreDecision});
    }

    auto cmp = [](const OptionIA& a, const OptionIA& b) {
        if (a.scoreDecision != b.scoreDecision) {
            return a.scoreDecision > b.scoreDecision;
        }
        return a.proba > b.proba;
    };
    std::sort(options.begin(), options.end(), cmp);

    if (weightedMedium) {
        const int topK = std::min(3, static_cast<int>(options.size()));
        std::vector<double> poids(topK, 0.0);

        const double minScore = options[topK - 1].scoreDecision;
        for (int i = 0; i < topK; ++i) {
            poids[i] = (options[i].scoreDecision - minScore) + 0.22;
            if (options[i].type == PredictionType::Suite) {
                poids[i] *= 0.72;
            }
        }

        double somme = 0.0;
        for (double p : poids) {
            somme += p;
        }
        std::uniform_real_distribution<double> dist(0.0, somme);
        double tirage = dist(rng);
        double cumul = 0.0;
        for (int i = 0; i < topK; ++i) {
            cumul += poids[i];
            if (tirage <= cumul) {
                return options[i].type;
            }
        }
    }

    return options[0].type;
}
