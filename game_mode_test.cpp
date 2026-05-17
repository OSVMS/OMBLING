#include "game.h"

void Game::appliquerScoreJoueurSelonMode() {
    if (estModeMultijoueur()) {
        if (joueurCourant == 1) {
            scoreJ1 += derniersPoints;
        } else {
            scoreJ2 += derniersPoints;
        }
        if (derniersPoints >= 0) {
            dernierResultat = "J" + std::to_string(joueurCourant) + " : Reussi +" + std::to_string(derniersPoints) + " pts";
        } else {
            dernierResultat = "J" + std::to_string(joueurCourant) + " : Rate " + std::to_string(derniersPoints) + " pts";
        }
        joueurCourant = (joueurCourant == 1) ? 2 : 1;
        return;
    }

    score += derniersPoints;
}

void Game::configurerTitreSelonMode() {
    if (estModeSolo()) {
        if (modeJeu == ModeJeu::SoloFacile) {
            txtTitre->setString("Solo Facile vs IA");
        } else if (modeJeu == ModeJeu::SoloMoyen) {
            txtTitre->setString("Solo Moyen vs IA");
        } else {
            txtTitre->setString("Solo Difficile vs IA");
        }
        return;
    }

    if (estModeMultijoueurMaison()) {
        txtTitre->setString("Multijoueur Maison");
        return;
    }

    if (estModeMultijoueurOnline()) {
        txtTitre->setString("Multijoueur Online");
        return;
    }

    txtTitre->setString("Partie Test");
}

void Game::configurerFinPartieTest(std::string& titre, std::string& detail, sf::Color& couleur) const {
    if (score > 0) {
        titre = "Victoire";
        couleur = {110, 230, 120};
    } else {
        titre = "Defaite";
        couleur = {240, 110, 110};
    }
    detail = "Score final: " + std::to_string(score);
}
