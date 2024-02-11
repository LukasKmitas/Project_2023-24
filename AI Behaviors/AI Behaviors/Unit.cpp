#include "Unit.h"

Unit::Unit()
{
    initView();
}

Unit::~Unit()
{
}

void Unit::update(sf::Time t_deltaTime, std::vector<Unit*>& allUnits)
{ 
    m_viewCircleShape.setPosition(m_unitSprite.getPosition().x, m_unitSprite.getPosition().y);
    avoidCollisions(allUnits);
}

void Unit::render(sf::RenderWindow& m_window) const
{
    m_window.draw(m_unitSprite);
    m_window.draw(m_viewCircleShape);
}

void Unit::attack(Unit* target)
{
}

void Unit::avoidCollisions(std::vector<Unit*>& allUnits)
{
    sf::Vector2f separationForce(0.0f, 0.0f);
    int closeUnits = 0;

    for (auto& other : allUnits)
    {
        if (other == this) continue;

        float dist = distance(this->getPosition(), other->getPosition());
        if (dist < m_viewRadius - 150)
        { 
            sf::Vector2f pushAway = this->getPosition() - other->getPosition();
            normalize(pushAway); 
            separationForce += pushAway / dist;
            closeUnits++;
        }
    }

    if (closeUnits > 0) 
    {
        separationForce /= static_cast<float>(closeUnits); 
        normalize(separationForce);
        separationForce *= m_speed;

        m_targetPosition += separationForce;
    }
}

void Unit::setTargetPosition(const sf::Vector2f& targetPos)
{
    m_targetPosition = targetPos;
    isOrbiting = false;
}

void Unit::setPosition(const sf::Vector2f& position) 
{
    m_position = position;
    m_unitSprite.setPosition(position);
}

void Unit::moveTo(const sf::Vector2f& targetPos)
{
    m_targetPosition = targetPos;
    isOrbiting = false;
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

void Unit::initView()
{
    m_viewCircleShape.setRadius(m_viewRadius);
    m_viewCircleShape.setFillColor(sf::Color(255, 255, 255, 0));
    m_viewCircleShape.setOrigin(m_viewRadius, m_viewRadius);
    m_viewCircleShape.setPosition(m_position);
}

bool Unit::checkAffordability()
{
    if (Global::currency >= m_cost)
    {
        Global::currency -= m_cost;
        std::cout << m_cost << std::endl;
        return true;
    }
    else
    {
        std::cout << "Not enough currency to create unit." << std::endl;
        return false;
    }
}

sf::Vector2f Unit::getPosition() const 
{
    return m_unitSprite.getPosition();
}

float Unit::angleFromVector(const sf::Vector2f& vector) 
{
    return std::atan2(vector.y, vector.x) * 180 / PI;
}

float Unit::getViewRadius() const 
{
    return m_viewRadius;
}