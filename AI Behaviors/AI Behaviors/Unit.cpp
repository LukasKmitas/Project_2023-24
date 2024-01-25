#include "Unit.h"

Unit::Unit()
{
}

Unit::~Unit()
{
}

void Unit::update(sf::Time t_deltaTime)
{
}

void Unit::render(sf::RenderWindow& m_window) const
{
    m_window.draw(m_unitSprite);
}

void Unit::move(const sf::Vector2f& direction)
{
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
