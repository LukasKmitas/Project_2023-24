#pragma once
#include "Unit.h"

class Vehicle : public Unit
{
public:
    Vehicle();
    virtual ~Vehicle();

    virtual void move(const sf::Vector2f& direction) override;
    virtual void attack(Unit* target) override;
};
