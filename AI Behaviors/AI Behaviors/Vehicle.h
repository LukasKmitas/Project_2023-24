#pragma once
#include "Unit.h"

class Vehicle : public Unit
{
public:
    Vehicle();
    virtual ~Vehicle();

    void update(sf::Time t_deltaTime, std::vector<Unit*>& allUnits) override;

protected:

    void steerTowards(const sf::Vector2f& target);

private:

    float maxForce = 1.0f;
};
