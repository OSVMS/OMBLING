#pragma once

#include "prediction_engine.h"

#include <random>
#include <vector>

class AIMedium {
public:
    static PredictionType choose(const std::vector<int>& knownCards, std::mt19937& rng);
};
