#pragma once

#include <array>
#include <random>
#include <string>
#include <vector>

enum class PredictionType {
    Couleur,
    Signe,
    Inferieur,
    Superieur,
    Suite,
    Paire
};

class PredictionEngine {
public:
    static constexpr int kPredictionCount = 6;

    static const std::array<PredictionType, kPredictionCount>& predictionTypes();
    static const std::array<std::string, kPredictionCount>& predictionLabels();

    static int points(PredictionType type);
    static std::string name(PredictionType type);

    static bool isSuccessful(
        PredictionType type,
        int gaucheValeur,
        int gaucheCouleur,
        int droiteValeur,
        int droiteCouleur
    );

    static PredictionType chooseForIA(
        const std::vector<int>& knownCards,
        bool weightedMedium,
        bool hardMode,
        int suiteStreak,
        std::mt19937& rng
    );
};
