#pragma once
#include "Unit.h"

class Infantry : public Unit
{
public:
    Infantry();
    virtual ~Infantry();

    virtual void move(const sf::Vector2f& direction) override;
    virtual void attack(Unit* target) override;
};
