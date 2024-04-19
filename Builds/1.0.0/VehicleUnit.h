#pragma once
#include "Unit.h"

class VehicleUnit : public Unit
{
public:
    VehicleUnit();
    virtual ~VehicleUnit();

    void update(sf::Time t_deltaTime, std::vector<Unit*>& m_allyUnits) override;
    void render(sf::RenderWindow& m_window) override;

};
