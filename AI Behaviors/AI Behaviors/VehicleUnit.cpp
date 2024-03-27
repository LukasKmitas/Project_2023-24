#include "VehicleUnit.h"

VehicleUnit::VehicleUnit()
{
    m_unitType = UnitType::Vehicle;
}

VehicleUnit::~VehicleUnit()
{
}

void VehicleUnit::update(sf::Time t_deltaTime, std::vector<Unit*>& allyUnits)
{
    Unit::update(t_deltaTime, allyUnits);
}

void VehicleUnit::render(sf::RenderWindow& m_window)
{
    Unit::render(m_window);
}

