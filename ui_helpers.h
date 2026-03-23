#pragma once

#include "carte.h"
#include <SFML/Graphics.hpp>

void centrerTexte(sf::Text& txt, const sf::RectangleShape& rect);
void dessinerCarte(sf::RenderWindow& window, const carte& c, float x, float y, const sf::Font& font);
