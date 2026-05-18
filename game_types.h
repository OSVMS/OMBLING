#pragma once

#include <vector>

enum class JokerAction {
    None,
    X2,
    Next,
    Mix,
    Vision,
    Tirage,
    Retry,
    Swap,
    Plus5,
    Minus5,
};

enum class SpecialCardType {
    None,
    Grace,
    Maudit,
    Double,
    Nul,
    Plus10,
    Minus10,
};

struct PlayerState {
    int score = 0;
    int successStreak = 0;
    bool immunity = false;
    int multiplier = 1;
    int temporaryModifier = 0;
    int extraDraws = 0;
    bool skipNextTurn = false;
    bool retryAvailable = false;
    int revealedCardId = 0;
    int lastScoreDelta = 0;
    std::vector<JokerAction> jokers;
    std::vector<SpecialCardType> specialCards;
};
