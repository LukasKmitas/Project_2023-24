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
}

void Unit::render(sf::RenderWindow& m_window) const
{
    m_window.draw(m_unitSprite);
    m_window.draw(m_viewCircleShape);
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
    m_viewCircleShape.setRadius(viewRadius);
    m_viewCircleShape.setFillColor(sf::Color(255, 255, 255, 0));
    m_viewCircleShape.setOrigin(viewRadius, viewRadius);
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
    return viewRadius;
}