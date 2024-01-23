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

    sf::Vector2f position;

    float health;

};

