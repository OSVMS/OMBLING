#include "ui_helpers.h"

void centrerTexte(sf::Text& txt, const sf::RectangleShape& rect) {
    sf::FloatRect tb = txt.getLocalBounds();
    txt.setOrigin({tb.position.x + tb.size.x / 2.f, tb.position.y + tb.size.y / 2.f});
    sf::FloatRect rb = rect.getGlobalBounds();
    txt.setPosition({rb.position.x + rb.size.x / 2.f, rb.position.y + rb.size.y / 2.f});
}

void dessinerCarte(sf::RenderWindow& window, const carte& c, float x, float y, const sf::Font& font) {
    const float W = 150.f;
    const float H = 220.f;

    sf::RectangleShape fond({W, H});
    fond.setPosition({x, y});
    fond.setFillColor(sf::Color::White);
    fond.setOutlineThickness(3.f);
    fond.setOutlineColor({80, 80, 80});
    window.draw(fond);

    sf::Color couleur = (c.getCouleur() <= 2) ? sf::Color::Red : sf::Color::Black;

    sf::Text valHaut(font, c.getNomValeur(), 22);
    valHaut.setFillColor(couleur);
    sf::FloatRect vb = valHaut.getLocalBounds();
    valHaut.setOrigin({vb.position.x, vb.position.y});
    valHaut.setPosition({x + 10.f, y + 8.f});
    window.draw(valHaut);

    sf::Text nomCouleur(font, c.getNomCouleur(), 20);
    nomCouleur.setFillColor(couleur);
    sf::FloatRect cb = nomCouleur.getLocalBounds();
    nomCouleur.setOrigin({cb.position.x + cb.size.x / 2.f, cb.position.y + cb.size.y / 2.f});
    nomCouleur.setPosition({x + W / 2.f, y + H / 2.f});
    window.draw(nomCouleur);

    sf::Text valBas(font, c.getNomValeur(), 22);
    valBas.setFillColor(couleur);
    sf::FloatRect vb2 = valBas.getLocalBounds();
    valBas.setOrigin({vb2.position.x + vb2.size.x, vb2.position.y + vb2.size.y});
    valBas.setPosition({x + W - 10.f, y + H - 8.f});
    valBas.setRotation(sf::degrees(180.f));
    window.draw(valBas);
}
