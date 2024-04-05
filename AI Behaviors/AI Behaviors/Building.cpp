#include "Building.h"

int Building::buildingCount = 0;

Building::Building()
{
	buildingCount++;
	buildingID = buildingCount;

	healthBarBackground.setSize(sf::Vector2f(50, 5));
	healthBarBackground.setFillColor(sf::Color(0, 0, 0));

	healthBar.setSize(sf::Vector2f(50, 5)); 
	healthBar.setFillColor(sf::Color(0, 255, 0));
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

	m_window.draw(healthBarBackground);
	m_window.draw(healthBar);
}

void Building::takeDamage(float damageAmount)
{
	m_health -= damageAmount;
	if (m_health < 0)
	{
		m_health = 0;
	}

	float healthPercentage = static_cast<float>(m_health) / m_maxHealth;
	healthBar.setSize(sf::Vector2f(healthBarBackground.getSize().x * healthPercentage, healthBar.getSize().y));
}

void Building::setPosition(const sf::Vector2f& m_position)
{
	m_buildingSprite.setPosition(m_position);
	m_placementRadius.setPosition(m_position);

	healthBarBackground.setPosition(m_position.x - healthBarBackground.getSize().x / 2, m_position.y - m_buildingSprite.getGlobalBounds().height / 2 - 10);
	healthBar.setPosition(healthBarBackground.getPosition());
}

sf::Vector2f Building::getPosition()
{
	return m_buildingSprite.getPosition();
}

int Building::getCost() const
{
	return m_cost;
}

int Building::getBuildingID() const
{
	return buildingID;
}

float Building::getHealth() const
{
	return m_health;
}

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

void Building::setPlacementRadiusSize(float radius)
{
	m_placementRadius.setRadius(radius);
	m_placementRadius.setOrigin(radius, radius);
	m_placementRadius.setFillColor(sf::Color(255, 255, 255, 0));

	m_placementRadius.setOutlineColor(sf::Color::Black); // temp
	m_placementRadius.setOutlineThickness(1); // temp
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