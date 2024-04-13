#include "Building.h"

//int Building::m_buildingCount = 0;

Building::Building(BuildingType type) 
	:
	m_type(type)
{
	//m_buildingCount++;
	//m_numBuildingCount = m_buildingCount;

	m_healthBarBackground.setSize(sf::Vector2f(50, 5));
	m_healthBarBackground.setFillColor(sf::Color(0, 0, 0));

	m_healthBar.setSize(sf::Vector2f(50, 5)); 
	m_healthBar.setFillColor(sf::Color(0, 255, 0));
}

Building::~Building()
{
}

void Building::update(sf::Time t_deltaTime)
{
}

void Building::render(sf::RenderWindow& m_window) const
{
	m_window.draw(m_buildingSprite);
	m_window.draw(m_placementRadius);

	m_window.draw(m_healthBarBackground);
	m_window.draw(m_healthBar);
}

/// <summary>
/// building to take damage 
/// </summary>
/// <param name="m_damageAmount"></param>
void Building::takeDamage(float m_damageAmount)
{
	m_health -= m_damageAmount;
	if (m_health < 0)
	{
		m_health = 0;
	}

	float healthPercentage = static_cast<float>(m_health) / m_maxHealth;
	m_healthBar.setSize(sf::Vector2f(m_healthBarBackground.getSize().x * healthPercentage, m_healthBar.getSize().y));
}

/// <summary>
/// sets position of the building
/// </summary>
/// <param name="m_position"></param>
void Building::setPosition(const sf::Vector2f& m_position)
{
	m_buildingSprite.setPosition(m_position);
	m_placementRadius.setPosition(m_position);

	m_healthBarBackground.setPosition(m_position.x - m_healthBarBackground.getSize().x / 2, m_position.y - m_buildingSprite.getGlobalBounds().height / 2 - 10);
	m_healthBar.setPosition(m_healthBarBackground.getPosition());
}

sf::Vector2f Building::getPosition()
{
	return m_buildingSprite.getPosition();
}

int Building::getCost() const
{
	return m_cost;
}

//int Building::getBuildingCount() const
//{
//	return m_numBuildingCount;
//}

float Building::getHealth() const
{
	return m_health;
}

BuildingType Building::getType() const
{
	return m_type;
}

/// <summary>
/// checks if it can be bought 
/// </summary>
/// <returns></returns>
bool Building::checkAffordability()
{
	if (Global::playerCurrency >= m_cost) 
	{
		Global::playerCurrency -= m_cost;
		std::cout << m_cost << std::endl;
		return true;
	}
	else 
	{
		std::cout << "Not enough currency to place the building." << std::endl;
		return false;
	}
}

/// <summary>
/// sets the placement radius
/// </summary>
/// <param name="m_radius"></param>
void Building::setPlacementRadiusSize(float m_radius)
{
	m_placementRadius.setRadius(m_radius);
	m_placementRadius.setOrigin(m_radius, m_radius);
	m_placementRadius.setFillColor(sf::Color(255, 255, 255, 0));

	m_placementRadius.setOutlineColor(sf::Color::Black); // temp
	m_placementRadius.setOutlineThickness(1); // temp
}

/// <summary>
/// Sets the health for laoding
/// </summary>
/// <param name="newHealth"></param>
void Building::setHealth(float newHealth)
{
	m_health = newHealth;
	updateHealthBar();
}

const sf::Sprite& Building::getBuildingSprite() const
{
	return m_buildingSprite;
}

const sf::Texture& Building::getBuildingTexture() const
{
	return m_buildingTexture;
}

const sf::CircleShape& Building::getPlacementRadius() const
{
	return m_placementRadius;
}

/// <summary>
/// Updates the health bar when its set for loading the building
/// </summary>
void Building::updateHealthBar() 
{
	if (m_healthBarBackground.getSize().x > 0)
	{ 
		float healthPercentage = static_cast<float>(m_health) / m_maxHealth;
		m_healthBar.setSize(sf::Vector2f(m_healthBarBackground.getSize().x * healthPercentage, m_healthBar.getSize().y));
	}
}
