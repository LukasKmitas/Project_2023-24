#pragma once
#include "Unit.h"

class VehicleUnit : public Unit
{
public:
    VehicleUnit();
    virtual ~VehicleUnit();

    void update(sf::Time t_deltaTime, std::vector<Unit*>& allUnits) override;

protected:

    void steerTowards(const sf::Vector2f& target);

private:

};
