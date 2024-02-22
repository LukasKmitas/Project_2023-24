#include "Refinery.h"

Refinery::Refinery()
{
	setupSprite();
	m_cost = 1500;
	setPlacementRadiusSize(150.0f);
}

Refinery::~Refinery()
{
}

void Refinery::update(sf::Time deltaTime)
{
}

void Refinery::render(sf::RenderWindow& m_window) const
{
	m_window.draw(m_buildingSprite);
	m_window.draw(m_placementRadius);
}

void Refinery::setupSprite()
{
	if (!m_buildingTexture.loadFromFile("Assets\\Images\\GUI\\Refinery.png"))
	{
		std::cout << "Error - Loading Refinery Building Sprite" << std::endl;
	}
	m_buildingSprite.setTexture(m_buildingTexture);
	m_buildingSprite.setPosition(m_position);
	m_buildingSprite.setTextureRect(sf::IntRect(395, 460, 55, 60));
	m_buildingSprite.setOrigin(m_buildingSprite.getLocalBounds().width / 2, m_buildingSprite.getLocalBounds().height / 2);
	m_buildingSprite.setScale(1, 1);
}
