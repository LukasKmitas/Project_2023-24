#include "VehicleUnit.h"

VehicleUnit::VehicleUnit()
{
}

VehicleUnit::~VehicleUnit()
{
}

void VehicleUnit::update(sf::Time t_deltaTime, std::vector<Unit*>& allUnits)
{
    Unit::update(t_deltaTime, allUnits);

    steerTowards(m_targetPosition);

    if (distance(m_position, m_targetPosition) < 10.0f) 
    { 
        m_velocity = sf::Vector2f(0, 0);
    }
    else 
    {
        sf::Vector2f deltaPosition = m_velocity * t_deltaTime.asSeconds();
        m_unitSprite.move(deltaPosition);
        m_position = m_unitSprite.getPosition();
    }
}

void VehicleUnit::steerTowards(const sf::Vector2f& target)
{
    sf::Vector2f desired = target - m_position;
    float distance = magnitude(desired);
    normalize(desired);

    // Slow down as the unit approaches the target
    float slowingDistance = 120.0f;
    if (distance < slowingDistance) 
    {
        desired *= (m_speed * distance / slowingDistance);
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
}
