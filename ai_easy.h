#pragma once

#include "prediction_engine.h"

#include <random>

class AIEasy {
public:
    static PredictionType choose(std::mt19937& rng);
};
