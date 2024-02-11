#include "HammerHead.h"

HammerHead::HammerHead()
{
	setupHammerhead();
	m_cost = 1500;
	m_speed = 1010;
	m_health = 100;
	m_maxForce = 0.1f;
	m_slowingRadius = 50.0f;
    m_rotationSpeed = 180.0f;
}

HammerHead::~HammerHead()
{
}

void HammerHead::update(sf::Time t_deltaTime, std::vector<Unit*>& allUnits)
{
    steerTowards(m_targetPosition, t_deltaTime);

    normalize(m_velocity);
    sf::Vector2f direction = m_velocity;
    float desiredAngle = atan2(direction.y, direction.x) * (180 / PI);
    float currentAngle = m_unitSprite.getRotation() - 90;
    float angleDiff = fmod(desiredAngle - currentAngle + 540, 360) - 180;

    float newAngle = currentAngle + std::min(std::max(angleDiff, -m_rotationSpeed * t_deltaTime.asSeconds()), m_rotationSpeed * t_deltaTime.asSeconds());
    m_unitSprite.setRotation(newAngle + 90);

    m_unitSprite.setPosition(m_position);
}

void HammerHead::steerTowards(const sf::Vector2f& target, sf::Time t_deltaTime)
{
    sf::Vector2f desired = target - m_position;
    float distance = magnitude(desired);
    normalize(desired);

    if (distance < m_slowingRadius) 
    {
        desired *= m_speed * (distance / m_slowingRadius);
    }
    else
    {
        desired *= m_speed;
    }

    sf::Vector2f steer = desired - m_velocity;
    if (magnitude(steer) > m_maxForce)
    {
        normalize(steer);
        steer *= m_maxForce;
    }

    m_velocity += steer;
    if (magnitude(m_velocity) > m_speed)
    {
        normalize(m_velocity);
        m_velocity *= m_speed;
    }

    m_position += m_velocity * t_deltaTime.asSeconds();
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
