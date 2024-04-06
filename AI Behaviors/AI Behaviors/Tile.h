#pragma once
#include "SFML/Graphics.hpp"

class Tile
{
public:
    Tile();

    enum class FogStatus { Unexplored, Explored, Visible };
    FogStatus m_fogStatus = FogStatus::Unexplored;

    sf::RectangleShape m_tile;
    int m_tileSize = 50;
    bool m_isWall;
    bool m_isResource;
    bool m_animationWeed;

private:

    void initTile();

};