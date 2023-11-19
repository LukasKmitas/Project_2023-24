#include "AirCraft.h"

AirCraft::AirCraft()
{
	setupSprite();
	m_cost = 1000;
}

AirCraft::~AirCraft()
{
}

void AirCraft::update(sf::Time deltaTime)
{
}

void AirCraft::render(sf::RenderWindow& m_window) const
{
	m_window.draw(m_buildingSprite);
}

void AirCraft::setupSprite()
{
	if (!m_buildingTexture.loadFromFile("Assets\\Images\\Air Control.png"))
	{
		std::cout << "Error - Loading AirCraft Building Sprite" << std::endl;
	}
	m_buildingSprite.setTexture(m_buildingTexture);
	m_buildingSprite.setPosition(m_position);
	m_buildingSprite.setTextureRect(sf::IntRect(1, 199, 56, 65));
	m_buildingSprite.setOrigin(m_buildingSprite.getLocalBounds().width / 2, m_buildingSprite.getLocalBounds().height / 2);
	m_buildingSprite.setScale(1, 1);
}
