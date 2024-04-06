#include "VehicleUnit.h"

VehicleUnit::VehicleUnit()
{
    m_unitType = UnitType::Vehicle;
}

VehicleUnit::~VehicleUnit()
{
}

void VehicleUnit::update(sf::Time t_deltaTime, std::vector<Unit*>& m_allyUnits)
{
    Unit::update(t_deltaTime, m_allyUnits);
}

void VehicleUnit::render(sf::RenderWindow& m_window)
{
    Unit::render(m_window);
}

