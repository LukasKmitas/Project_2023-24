#include "Tile.h"

Tile::Tile()
{
	initTile();
}

void Tile::initTile()
{
	m_tile.setSize(sf::Vector2f(tileSize, tileSize));
}
