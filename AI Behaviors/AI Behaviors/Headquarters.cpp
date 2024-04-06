#include "Headquarters.h"

Headquarters::Headquarters()
{
	setupSprite();
	setPlacementRadiusSize(250);
}

Headquarters::~Headquarters()
{
}

void Headquarters::update(sf::Time t_deltaTime)
{
}

void Headquarters::render(sf::RenderWindow& m_window) const
{
	Building::render(m_window);
}

/// <summary>
/// initializes the headquaters
/// </summary>
void Headquarters::setupSprite()
{
	if (!m_buildingTexture.loadFromFile("Assets\\Images\\GUI\\Command Center.png"))
	{
		std::cout << "Error - Loading Command Center Building Sprite" << std::endl;
	}
	m_buildingSprite.setTexture(m_buildingTexture);
	m_buildingSprite.setPosition(m_position);
	m_buildingSprite.setTextureRect(sf::IntRect(1, 229, 56, 75));
	m_buildingSprite.setOrigin(m_buildingSprite.getLocalBounds().width / 2, m_buildingSprite.getLocalBounds().height / 2);
	m_buildingSprite.setScale(1, 1);
}