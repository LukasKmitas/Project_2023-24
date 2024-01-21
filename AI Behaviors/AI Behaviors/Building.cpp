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

void Building::update(sf::Time t_deltaTime)
{
}

void Building::render(sf::RenderWindow& m_window) const
{
}

int Building::getHealth() const
{
	return m_health;
}

void Building::setHealth(int m_health)
{
	m_health = m_health;
}

void Building::applyDamage(int m_damage)
{
	m_health -= m_damage;
	if (m_health < 0) 
	{
		m_health = 0;
	}
}

void Building::setPosition(const sf::Vector2f& m_position)
{
	m_buildingSprite.setPosition(m_position);
	m_placementRadius.setPosition(m_position);
}

sf::Vector2f Building::getPosition()
{
	return m_buildingSprite.getPosition();
}

int Building::getCost() const
{
	return m_cost;
}

bool Building::checkAffordability()
{
	if (Global::currency >= m_cost) 
	{
		Global::currency -= m_cost;
		std::cout << m_cost << std::endl;
		return true;
	}
	else 
	{
		std::cout << "Not enough currency to place the building." << std::endl;
		return false;
	}
}

void Building::togglePlacementRadiusVisibility() 
{
	m_placementRadiusVisible = !m_placementRadiusVisible;
}

void Building::setPlacementRadius(float radius, sf::Color color, float thickness)
{
	m_placementRadius.setRadius(radius);
	m_placementRadius.setFillColor(sf::Color::Transparent);
	m_placementRadius.setOutlineColor(color);
	m_placementRadius.setOutlineThickness(thickness);
	m_placementRadius.setOrigin(radius, radius);
	m_placementRadius.setPosition(m_buildingSprite.getPosition().x, m_buildingSprite.getPosition().y);
}