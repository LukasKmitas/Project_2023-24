#include "AircraftUnit.h"

AircraftUnit::AircraftUnit()
{
}

AircraftUnit::~AircraftUnit()
{
}

void AircraftUnit::update(sf::Time t_deltaTime, std::vector<Unit*>& allUnits)
{
	Unit::update(t_deltaTime, allUnits);
}

sf::Vector2f AircraftUnit::steerTowards(sf::Vector2f target)
{
    sf::Vector2f desiredVelocity = target - m_position; 
    float distance = magnitude(desiredVelocity);

    if (distance < 0.0001) 
    {
        return sf::Vector2f(0, 0);
    }
    if (distance > 0)
    {
        normalize(desiredVelocity);
    }

    if (distance < m_slowingRadius) 
    {
        desiredVelocity *= m_speed * (distance / m_slowingRadius);
    }
    else
    {
        desiredVelocity *= m_speed;
    }

    sf::Vector2f steer = desiredVelocity - m_velocity;
    if (magnitude(steer) > m_maxForce) 
    {
        normalize(steer);
        steer *= m_maxForce; 
    }

    return steer;
}
