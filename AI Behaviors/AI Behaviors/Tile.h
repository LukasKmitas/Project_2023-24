#pragma once
#include "SFML/Graphics.hpp"

class Tile
{
public:
    Tile();

    sf::RectangleShape m_tile;
    int tileSize = 50;
    bool isWall;
    bool isResource;

private:

    void initTile();

};