#pragma once
#include <SFML/Graphics.hpp>

class Grid
{
public:
    Grid(int rows, int columns, float cellSize);

    void draw(sf::RenderWindow& m_window);

private:
    int rows;
    int columns;
    float cellSize;
    std::vector<sf::RectangleShape> cells;

    void createGrid();
};