#include "Barracks.h"

Barracks::Barracks()
{
	setupSprite();
	setPlacementRadiusSize(100.0f);
	m_cost = 500;
}

Barracks::~Barracks()
{
}

void Barracks::update(sf::Time t_deltaTime)
{
}

void Barracks::render(sf::RenderWindow& m_window) const
{
	Building::render(m_window);
}

/// <summary>
/// initializes the barracks building
/// </summary>
void Barracks::setupSprite()
{
	if (!m_buildingTexture.loadFromFile("Assets\\Images\\GUI\\Factory.png"))
	{
		std::cout << "Error - Loading Building Sprite" << std::endl;
	}
	m_buildingSprite.setTexture(m_buildingTexture);
	m_buildingSprite.setPosition(m_position);
	m_buildingSprite.setTextureRect(sf::IntRect(1, 131, 56, 64));
	m_buildingSprite.setOrigin(m_buildingSprite.getLocalBounds().width / 2, m_buildingSprite.getLocalBounds().height / 2);
	m_buildingSprite.setScale(1, 1);
}
