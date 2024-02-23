#pragma once
#include "SFML/Graphics.hpp"

class Tile
{
public:
    Tile();

    enum class FogStatus { Unexplored, Explored, Visible };
    FogStatus fogStatus = FogStatus::Unexplored;

    sf::RectangleShape m_tile;
    int tileSize = 50;
    bool isWall;
    bool isResource;
    bool animationWeed;

private:

    void initTile();

};