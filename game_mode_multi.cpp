#include "game.h"

bool Game::estModeMultiMaison() const {
    return modeJeu == ModeJeu::MultiMaison;
}

void Game::configurerFinPartieMulti(std::string& titre, std::string& detail, sf::Color& couleur) const {
    if (scoreJ1 > scoreJ2) {
        titre = "Victoire Joueur 1";
        couleur = {110, 230, 120};
    } else if (scoreJ2 > scoreJ1) {
        titre = "Victoire Joueur 2";
        couleur = {110, 230, 120};
    } else {
        titre = "Egalite";
        couleur = {230, 230, 120};
    }
    detail = "J1: " + std::to_string(scoreJ1) + "  |  J2: " + std::to_string(scoreJ2);
}

void Game::renderScoreSelonMode() {
    if (estModeMultijoueur()) {
        sf::Text txtScoreJ1(font, "J1: " + std::to_string(scoreJ1), 22);
        txtScoreJ1.setFillColor({255, 230, 160});
        txtScoreJ1.setPosition({610.f, 40.f});
        window.draw(txtScoreJ1);

        sf::Text txtScoreJ2(font, "J2: " + std::to_string(scoreJ2), 22);
        txtScoreJ2.setFillColor({190, 235, 255});
        txtScoreJ2.setPosition({610.f, 68.f});
        window.draw(txtScoreJ2);

        sf::Text txtTour(font, "Tour: J" + std::to_string(joueurCourant), 20);
        txtTour.setFillColor(sf::Color::White);
        txtTour.setPosition({610.f, 96.f});
        window.draw(txtTour);
        return;
    }

    sf::Text txtScore(font, "Score: " + std::to_string(score), 24);
    txtScore.setFillColor({255, 240, 130});
    txtScore.setPosition({610.f, 42.f});
    window.draw(txtScore);

    if (estModeSolo()) {
        sf::Text txtScoreIA(font, "Score IA: " + std::to_string(scoreIA), 21);
        txtScoreIA.setFillColor({200, 230, 255});
        txtScoreIA.setPosition({598.f, 74.f});
        window.draw(txtScoreIA);
    }
}
