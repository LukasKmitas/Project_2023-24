#include "Unit.h"

Unit::Unit()
{
}

Unit::~Unit()
{
}

void Unit::update(sf::Time t_deltaTime, std::vector<Unit*>& allUnits)
{
    float deltaTimeInSeconds = t_deltaTime.asSeconds();
    sf::Vector2f desired = m_targetPosition - m_position;
    float distance = magnitude(desired);
    normalize(desired);

    float slowingDistance = 150.0f;
    if (distance < slowingDistance)
    {
        desired *= (maxSpeed * distance / slowingDistance);
    }
    else 
    {
        desired *= maxSpeed;
    }

    sf::Vector2f sep = separation(allUnits);
    sep *= separationWeight;

    sf::Vector2f steer = desired + sep;
    m_velocity += steer;
    normalize(m_velocity);
    m_velocity *= maxSpeed;

    if (distance > stoppingDistance) 
    { 
        m_position += m_velocity * deltaTimeInSeconds;
        m_unitSprite.setPosition(m_position);
    }
    else 
    {
        m_velocity = sf::Vector2f(0.0f, 0.0f); 
    }
}

void Unit::render(sf::RenderWindow& m_window) const
{
    m_window.draw(m_unitSprite);
}

void Unit::attack(Unit* target)
{
}

void Unit::setPosition(const sf::Vector2f& position) 
{
    m_unitSprite.setPosition(position);
}

void Unit::moveTo(const sf::Vector2f& targetPos) 
{
    m_targetPosition = targetPos;
   
    float offsetX = static_cast<float>(rand() % static_cast<int>(offsetRange)) - offsetRange / 2.0f;
    float offsetY = static_cast<float>(rand() % static_cast<int>(offsetRange)) - offsetRange / 2.0f;

    m_targetPosition = sf::Vector2f(targetPos.x + offsetX, targetPos.y + offsetY);
}

void Unit::setSelected(bool selected) 
{
    isSelected = selected;
    if (isSelected) 
    {
        m_unitSprite.setColor(sf::Color(0, 255, 0, 128));
    }
    else 
    {
        m_unitSprite.setColor(sf::Color(255, 255, 255, 255));
    }
}

sf::Vector2f Unit::separation(const std::vector<Unit*>& units)
{
    sf::Vector2f steer(0.0f, 0.0f);
    int count = 0;
    float desiredSeparation = 50.0f;

    for (const auto& other : units) 
    {
        float d = distance(m_position, other->getPosition());
        if ((other != this) && (d < desiredSeparation) && (d > 0)) 
        {
            sf::Vector2f diff = m_position - other->getPosition();
            normalize(diff);
            diff /= d; 
            steer += diff;
            count++;
        }
    }

    if (count > 0) 
    {
        steer /= static_cast<float>(count);
        normalize(steer);
        steer *= maxSpeed;
    }

    return steer;
}

float Unit::distance(const sf::Vector2f& a, const sf::Vector2f& b) 
{
    sf::Vector2f diff = a - b;
    return std::sqrt(diff.x * diff.x + diff.y * diff.y);
}

void Unit::normalize(sf::Vector2f& vec) 
{
    float len = std::sqrt(vec.x * vec.x + vec.y * vec.y);
    if (len != 0) 
    {
        vec.x /= len;
        vec.y /= len;
    }
}

float Unit::magnitude(const sf::Vector2f& v) 
{
    return std::sqrt(v.x * v.x + v.y * v.y);
}

sf::Vector2f Unit::getPosition() const 
{
    return m_position;
}
