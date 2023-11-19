#include "Grid.h"

Grid::Grid()
{
    setupSprite();
	createGrid();
}

void Grid::draw(sf::RenderWindow& m_window)
{
    for (const sf::RectangleShape& cell : cells)
    {
        m_window.draw(cell);
    }

    /*for (const Tile& tile : tiles) 
    {
        m_window.draw(tile.m_sprite);
    }*/
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
            cell.setOutlineColor(sf::Color(255,255,255,150));
            cells.push_back(cell);
        }
    }

    //for (int i = 0; i < rows; ++i) 
    //{
    //    for (int j = 0; j < columns; ++j) 
    //    {
    //        Tile tile;
    //        tile.isTraversable = true; // Set this based on your game logic
    //        tile.m_sprite.setTexture(spriteSheetTexture);

    //        // Example: Assuming each tile on the sprite sheet is 32x32 pixels
    //        sf::IntRect textureRect(0, 0, 32, 32);
    //        tile.m_sprite.setTextureRect(textureRect);

    //        // Set the position of the tile
    //        tile.m_sprite.setPosition(j * cellSize, i * cellSize);

    //        tiles.push_back(tile);
    //    }
    //}
}

void Grid::setupSprite()
{
    if (!spriteSheetTexture.loadFromFile("Assets\\Temp\\Labrynna Underwater.png"))
    {
        std::cout << "Error - Loading Vehicle Building Sprite" << std::endl;
    }
}
