#include "AircraftUnit.h"

AircraftUnit::AircraftUnit()
{
	m_unitType = UnitType::Air;
}

AircraftUnit::~AircraftUnit()
{
}

void AircraftUnit::update(sf::Time t_deltaTime, std::vector<Unit*>& allUnits)
{
	Unit::update(t_deltaTime, allUnits);
}
