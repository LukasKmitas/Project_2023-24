#include "Headquarters.h"

Headquarters::Headquarters()
{
	setupSprite();
	m_position = sf::Vector2f(0.0f, 0.0f);
}

Headquarters::~Headquarters()
{
}

void Headquarters::update(sf::Time deltaTime)
{
	m_position += sf::Vector2f(0.1, 0);
	m_buildingSprite.setPosition(m_position);
	//std::cout << m_position.x << " " << m_position.y << std::endl;
	//std::cout << getPosition().x << " " << getPosition().y << std::endl;
}

void Headquarters::render(sf::RenderWindow& m_window) const
{
	m_window.draw(m_buildingSprite);
}

void Headquarters::setupSprite()
{
	if (!m_buildingTexture.loadFromFile("Assets\\Temp\\Command Center.png"))
	{
		std::cout << "Error - Loading Building Sprite" << std::endl;
	}
	m_buildingSprite.setTexture(m_buildingTexture);
	m_buildingSprite.setPosition(m_position);
	m_buildingSprite.setTextureRect(sf::IntRect(1, 229, 56, 75));
	m_buildingSprite.setOrigin(m_buildingSprite.getLocalBounds().width / 2, m_buildingSprite.getLocalBounds().height / 2);
	m_buildingSprite.setScale(1, 1);
}


