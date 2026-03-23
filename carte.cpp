#include "carte.h"
#include <iostream>

carte::carte() {
	this->id = 0;
	this->valeur = 0;
	this->couleur = 0;
}

carte::carte(int id, int valeur, int couleur) {
	this->id = id;
	this->valeur = valeur;
	this->couleur = couleur;
}

std::string carte::getNomValeur() const {
	switch (this->valeur) {
		case 1:  return "As";
		case 11: return "Valet";
		case 12: return "Reine";
		case 13: return "Roi";
		default: return std::to_string(this->valeur);
	}
}

std::string carte::getNomCouleur() const {
	switch (this->couleur) {
		case 1: return "Coeur";
		case 2: return "Carreau";
		case 3: return "Trefle";
		case 4: return "Pique";
		default: return "Inconnue";
	}
}

int carte::getCouleur() const {
	return this->couleur;
}

void carte::jouer() {
	std::cout << "La carte tirée est : " << getNomValeur() << " de " << getNomCouleur() << std::endl;
}
