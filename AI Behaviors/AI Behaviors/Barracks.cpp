#include "Barracks.h"

Barracks::Barracks()
{
	setupSprite();
	m_cost = 500;
}

Barracks::~Barracks()
{
}

void Barracks::update(sf::Time deltaTime)
{
}

void Barracks::render(sf::RenderWindow& m_window) const
{
	m_window.draw(m_buildingSprite);
}

void Barracks::setupSprite()
{
	if (!m_buildingTexture.loadFromFile("Assets\\Images\\Factory.png"))
	{
		std::cout << "Error - Loading Building Sprite" << std::endl;
	}
	m_buildingSprite.setTexture(m_buildingTexture);
	m_buildingSprite.setPosition(m_position);
	m_buildingSprite.setTextureRect(sf::IntRect(1, 131, 56, 64));
	m_buildingSprite.setOrigin(m_buildingSprite.getLocalBounds().width / 2, m_buildingSprite.getLocalBounds().height / 2);
	m_buildingSprite.setScale(1, 1);
}
