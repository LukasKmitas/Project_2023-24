#pragma once
#include <iostream>
#include "SFML/Graphics.hpp"

class Unit
{
public:

    Unit();
    virtual ~Unit();

    virtual void move(const sf::Vector2f& direction);
    virtual void attack(Unit* target);
    virtual void update(float deltaTime);

protected:

    sf::Texture m_unitTexture;
    sf::Sprite m_unitSprite;

    int m_cost = 100;

    float m_health = 100;

    sf::Vector2f m_position;

};

