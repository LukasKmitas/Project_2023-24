#include "VehicleUnit.h"

VehicleUnit::VehicleUnit()
{
    m_unitType = UnitType::Vehicle;
}

VehicleUnit::~VehicleUnit()
{
}

void VehicleUnit::update(sf::Time t_deltaTime, std::vector<Unit*>& allUnits)
{
    Unit::update(t_deltaTime, allUnits);
}

