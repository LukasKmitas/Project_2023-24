#include "Unit.h"

Unit::Unit()
{
}

Unit::~Unit()
{
}

void Unit::update(float deltaTime)
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
    m_position = position;
    m_unitSprite.setPosition(position);
}