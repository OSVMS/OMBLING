#pragma once

#include "carte.h"
#include "game_types.h"
#include <random>
#include <string>
#include <vector>

class IJokerEffect {
public:
    virtual ~IJokerEffect() = default;
    virtual void apply(
        PlayerState* players,
        int joueurCourant,
        carte paquet[52],
        int& index,
        std::mt19937& rng,
        bool estModeMultijoueur,
        bool estModeMultijoueurOnline,
        std::string& message
    ) = 0;
    virtual std::string label() const = 0;
};

class JokerX2 final : public IJokerEffect {
public:
    void apply(PlayerState* players, int joueurCourant, carte paquet[52], int& index, std::mt19937& rng, bool estModeMultijoueur, bool estModeMultijoueurOnline, std::string& message) override;
    std::string label() const override { return "x2 Points"; }
};

class JokerNext final : public IJokerEffect {
public:
    void apply(PlayerState* players, int joueurCourant, carte paquet[52], int& index, std::mt19937& rng, bool estModeMultijoueur, bool estModeMultijoueurOnline, std::string& message) override;
    std::string label() const override { return "Passer tour"; }
};

class JokerMix final : public IJokerEffect {
public:
    void apply(PlayerState* players, int joueurCourant, carte paquet[52], int& index, std::mt19937& rng, bool estModeMultijoueur, bool estModeMultijoueurOnline, std::string& message) override;
    std::string label() const override { return "Melanger"; }
};

class JokerVision final : public IJokerEffect {
public:
    void apply(PlayerState* players, int joueurCourant, carte paquet[52], int& index, std::mt19937& rng, bool estModeMultijoueur, bool estModeMultijoueurOnline, std::string& message) override;
    std::string label() const override { return "Voir suivante"; }
};

class JokerTirage final : public IJokerEffect {
public:
    void apply(PlayerState* players, int joueurCourant, carte paquet[52], int& index, std::mt19937& rng, bool estModeMultijoueur, bool estModeMultijoueurOnline, std::string& message) override;
    std::string label() const override { return "Tirage bonus"; }
};

class JokerRetry final : public IJokerEffect {
public:
    void apply(PlayerState* players, int joueurCourant, carte paquet[52], int& index, std::mt19937& rng, bool estModeMultijoueur, bool estModeMultijoueurOnline, std::string& message) override;
    std::string label() const override { return "Immunite"; }
};

class JokerSwap final : public IJokerEffect {
public:
    void apply(PlayerState* players, int joueurCourant, carte paquet[52], int& index, std::mt19937& rng, bool estModeMultijoueur, bool estModeMultijoueurOnline, std::string& message) override;
    std::string label() const override { return "Echanger"; }
};

class JokerPlus5 final : public IJokerEffect {
public:
    void apply(PlayerState* players, int joueurCourant, carte paquet[52], int& index, std::mt19937& rng, bool estModeMultijoueur, bool estModeMultijoueurOnline, std::string& message) override;
    std::string label() const override { return "+5 pts"; }
};

class JokerMinus5 final : public IJokerEffect {
public:
    void apply(PlayerState* players, int joueurCourant, carte paquet[52], int& index, std::mt19937& rng, bool estModeMultijoueur, bool estModeMultijoueurOnline, std::string& message) override;
    std::string label() const override { return "-5 pts"; }
};
