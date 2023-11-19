#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include "Tile.h"

class Grid
{
public:
    Grid();

    void draw(sf::RenderWindow& m_window);

private:

    void createGrid();
    void setupSprite();

    int rows = 100;
    int columns = 100;
    float cellSize = 25;
    std::vector<sf::RectangleShape> cells;
    std::vector<Tile> tiles;

    sf::Texture spriteSheetTexture;

};