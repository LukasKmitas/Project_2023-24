#include "HammerHead.h"

HammerHead::HammerHead()
{
	setupHammerhead();
	m_cost = 1500;
	m_speed = 110;
	m_health = 100;
	m_slowingRadius = 100.0f;
    m_rotationSpeed = 80;
    m_maxForce = 200;
}

HammerHead::~HammerHead()
{
}

void HammerHead::update(sf::Time t_deltaTime, std::vector<Unit*>& allUnits)
{
    AircraftUnit::update(t_deltaTime, allUnits);

    sf::Vector2f steeringForce = steerTowards(m_targetPosition);

    m_velocity += steeringForce * t_deltaTime.asSeconds();

    float distanceToTarget = magnitude(m_targetPosition - m_position);
    if (distanceToTarget < 1.0f)
    { 
        m_velocity = sf::Vector2f(0, 0);
    }
    else if (magnitude(m_velocity) > m_speed)
    {
        normalize(m_velocity);
        m_velocity *= m_speed;
    }

    m_position += m_velocity * t_deltaTime.asSeconds();
    m_unitSprite.setPosition(m_position);

    if (magnitude(m_velocity) > 0) 
    {
        float targetAngle = angleFromVector(m_velocity) + 90;
        m_unitSprite.setRotation(targetAngle);
    }
}

void HammerHead::setupHammerhead()
{
	if (!m_unitTexture.loadFromFile("Assets\\Images\\HammerHead.png"))
	{
		std::cout << "Error - Loading HammerHead Texture" << std::endl;
	}
	m_unitSprite.setTexture(m_unitTexture);
	m_unitSprite.setPosition(m_position);
	m_unitSprite.setOrigin(m_unitSprite.getLocalBounds().width / 2, m_unitSprite.getLocalBounds().height / 2);
	m_unitSprite.setScale(0.13, 0.13);
}
