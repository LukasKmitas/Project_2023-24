#include "Building.h"

int Building::buildingCount = 0;

Building::Building()
{
	buildingCount++;
	buildingID = buildingCount;
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

void Building::setPosition(const sf::Vector2f& position)
{
	m_position = position;
	m_buildingSprite.setPosition(position);
}

sf::Vector2f Building::getPosition()
{
	return m_buildingSprite.getPosition();
}

int Building::getCost() const
{
	return m_cost;
}

bool Building::canAfford() const
{
	return Global::currency >= m_cost;
}

void Building::checkAffordability()
{
	if (canAfford())
	{
		Global::currency -= m_cost;
	}
	else
	{
		std::cout << "Not enough currency to place the building." << std::endl;
	}
}
