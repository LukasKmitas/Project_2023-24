#include "Building.h"

Building::Building()
{
}

Building::~Building()
{
}

void Building::update(sf::Time deltaTime)
{
}

void Building::render(sf::RenderWindow& m_window) const
{
}

int Building::getHealth() const
{
	return m_health;
}

void Building::setHealth(int health)
{
	m_health = health;
}

void Building::applyDamage(int damage)
{
	m_health -= damage;
	if (m_health < 0) 
	{
		m_health = 0;
	}
}

sf::Vector2f Building::getPosition()
{
	return m_buildingSprite.getPosition();
}