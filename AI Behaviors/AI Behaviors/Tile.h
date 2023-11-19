#pragma once
#include "SFML/Graphics.hpp"

class Tile
{
public:

    sf::RectangleShape m_tile;
    sf::Sprite m_sprite;
    bool isTraversable;
};