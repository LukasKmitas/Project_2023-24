#include "WarFactory.h"

WarFactory::WarFactory()
{
	setupSprite();
	setPlacementRadiusSize(150.0f);
	m_cost = 2000;
}

WarFactory::~WarFactory()
{
}

void WarFactory::update(sf::Time t_deltaTime)
{
}

void WarFactory::render(sf::RenderWindow& m_window) const
{
	Building::render(m_window);
}

/// <summary>
/// initializes the war factory building 
/// </summary>
void WarFactory::setupSprite()
{
	if (!m_buildingTexture.loadFromFile("Assets\\Images\\GUI\\Ground Control.png"))
	{
		std::cout << "Error - Loading Vehicle Building Sprite" << std::endl;
	}
	m_buildingSprite.setTexture(m_buildingTexture);
	m_buildingSprite.setPosition(m_position);
	m_buildingSprite.setTextureRect(sf::IntRect(1, 197, 56, 63));
	m_buildingSprite.setOrigin(m_buildingSprite.getLocalBounds().width / 2, m_buildingSprite.getLocalBounds().height / 2);
	m_buildingSprite.setScale(1, 1);
}
