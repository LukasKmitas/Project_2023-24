#include "Vehicle.h"

Vehicle::Vehicle()
{
}

Vehicle::~Vehicle()
{
}

void Vehicle::update(sf::Time t_deltaTime, std::vector<Unit*>& allUnits)
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

void Vehicle::steerTowards(const sf::Vector2f& target)
{
    sf::Vector2f desired = target - m_position;
    float distance = magnitude(desired);
    normalize(desired);

    // Slow down as the vehicle approaches the target
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
    if (magnitude(steer) > maxForce)
    {
        normalize(steer);
        steer *= maxForce;
    }

    m_velocity += steer;
}
