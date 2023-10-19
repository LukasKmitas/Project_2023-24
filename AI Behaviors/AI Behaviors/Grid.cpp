#include "Grid.h"

Grid::Grid(int rows, int columns, float cellSize)
    : rows(rows), columns(columns), cellSize(cellSize)
{
	createGrid();
}

void Grid::draw(sf::RenderWindow& m_window)
{
    for (const sf::RectangleShape& cell : cells)
    {
        m_window.draw(cell);
    }
}

void Grid::createGrid()
{
    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < columns; ++j)
        {
            sf::RectangleShape cell(sf::Vector2f(cellSize, cellSize));
            cell.setPosition(j * cellSize, i * cellSize);
            cell.setOutlineThickness(1.0f);
            cell.setFillColor(sf::Color::Black);
            cell.setOutlineColor(sf::Color::White);
            cells.push_back(cell);
        }
    }
}
