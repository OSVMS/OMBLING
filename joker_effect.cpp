#include "joker_effect.h"
#include <algorithm>

void JokerX2::apply(PlayerState* players, int joueurCourant, carte paquet[52], int& index, std::mt19937& rng, bool estModeMultijoueur, bool estModeMultijoueurOnline, std::string& message) {
    (void)paquet; (void)index; (void)rng; (void)estModeMultijoueur; (void)estModeMultijoueurOnline;
    if (joueurCourant < 1 || joueurCourant > 2) return;
    players[joueurCourant - 1].multiplier *= 2;
    message = "Joker X2 active : prochain tour double";
}

void JokerNext::apply(PlayerState* players, int joueurCourant, carte paquet[52], int& index, std::mt19937& rng, bool estModeMultijoueur, bool estModeMultijoueurOnline, std::string& message) {
    (void)paquet; (void)index; (void)rng; (void)estModeMultijoueurOnline;
    if (joueurCourant < 1 || joueurCourant > 2) return;
    if (estModeMultijoueur) {
        players[joueurCourant - 1].skipNextTurn = true;
        message = "Joker NEXT active : tour suivant passe";
    } else {
        message = "NEXT uniquement multijoueur";
    }
}

void JokerMix::apply(PlayerState* players, int joueurCourant, carte paquet[52], int& index, std::mt19937& rng, bool estModeMultijoueur, bool estModeMultijoueurOnline, std::string& message) {
    (void)players; (void)joueurCourant; (void)estModeMultijoueur; (void)estModeMultijoueurOnline;
    constexpr int kDeckSize = 52;
    if (index < kDeckSize) {
        std::shuffle(paquet + index, paquet + kDeckSize, rng);
        message = "Joker MIX active : paquet remelange";
    } else {
        message = "Rien a melanger";
    }
}

void JokerVision::apply(PlayerState* players, int joueurCourant, carte paquet[52], int& index, std::mt19937& rng, bool estModeMultijoueur, bool estModeMultijoueurOnline, std::string& message) {
    (void)rng; (void)estModeMultijoueur; (void)estModeMultijoueurOnline;
    constexpr int kDeckSize = 52;
    if (joueurCourant < 1 || joueurCourant > 2) return;
    if (index < kDeckSize) {
        players[joueurCourant - 1].revealedCardId = paquet[index].getId();
        message = "Joker VISION active : prochaine carte vue";
    } else {
        message = "Plus de carte a voir";
    }
}

void JokerTirage::apply(PlayerState* players, int joueurCourant, carte paquet[52], int& index, std::mt19937& rng, bool estModeMultijoueur, bool estModeMultijoueurOnline, std::string& message) {
    (void)paquet; (void)index; (void)rng; (void)estModeMultijoueur; (void)estModeMultijoueurOnline;
    if (joueurCourant < 1 || joueurCourant > 2) return;
    players[joueurCourant - 1].extraDraws += 1;
    message = "Joker TIRAGE active : deux tirages successifs";
}

void JokerRetry::apply(PlayerState* players, int joueurCourant, carte paquet[52], int& index, std::mt19937& rng, bool estModeMultijoueur, bool estModeMultijoueurOnline, std::string& message) {
    (void)paquet; (void)index; (void)rng; (void)estModeMultijoueur; (void)estModeMultijoueurOnline;
    if (joueurCourant < 1 || joueurCourant > 2) return;
    players[joueurCourant - 1].immunity = true;
    message = "Joker RETRY active : prochaine perte annulee";
}

void JokerSwap::apply(PlayerState* players, int joueurCourant, carte paquet[52], int& index, std::mt19937& rng, bool estModeMultijoueur, bool estModeMultijoueurOnline, std::string& message) {
    (void)paquet; (void)index; (void)rng;
    if (joueurCourant < 1 || joueurCourant > 2) return;
    if (estModeMultijoueur && !estModeMultijoueurOnline) {
        auto& opp = players[(joueurCourant == 1) ? 1 : 0];
        std::swap(players[joueurCourant - 1].jokers, opp.jokers);
        message = "Joker SWAP active : effets echanges";
    } else {
        message = "SWAP uniquement multijoueur local";
    }
}

void JokerPlus5::apply(PlayerState* players, int joueurCourant, carte paquet[52], int& index, std::mt19937& rng, bool estModeMultijoueur, bool estModeMultijoueurOnline, std::string& message) {
    (void)paquet; (void)index; (void)rng; (void)estModeMultijoueur; (void)estModeMultijoueurOnline;
    if (joueurCourant < 1 || joueurCourant > 2) return;
    players[joueurCourant - 1].temporaryModifier += 5;
    message = "Joker +5 active : prochain score +5";
}

void JokerMinus5::apply(PlayerState* players, int joueurCourant, carte paquet[52], int& index, std::mt19937& rng, bool estModeMultijoueur, bool estModeMultijoueurOnline, std::string& message) {
    (void)paquet; (void)index; (void)rng; (void)estModeMultijoueur; (void)estModeMultijoueurOnline;
    if (joueurCourant < 1 || joueurCourant > 2) return;
    players[joueurCourant - 1].temporaryModifier -= 5;
    message = "Joker -5 active : prochain score -5";
}
