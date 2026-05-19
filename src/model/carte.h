#ifndef CARTE_H
#define CARTE_H

#include <string>

class carte{
    private:
        int id;         // 1-13 = coeur, 14-26 = carreau, 27-39 = trefle, 40-52 = pique
        int valeur;     // 11 = valet, 12 = dame, 13 = roi
        int couleur;    //1 = coeur, 2 = carreau, 3 = trefle, 4 = pique

    public:
        carte();        
        carte(int id, int valeur, int couleur);
        void jouer();
        std::string getNomValeur() const;
        std::string getNomCouleur() const;
        int getValeur() const;
        int getCouleur() const;
        int getId() const;
};

#endif