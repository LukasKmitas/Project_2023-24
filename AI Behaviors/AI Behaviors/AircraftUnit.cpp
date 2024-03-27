#include "AircraftUnit.h"

AircraftUnit::AircraftUnit()
{
	m_unitType = UnitType::Air;
}

AircraftUnit::~AircraftUnit()
{
}

void AircraftUnit::update(sf::Time t_deltaTime, std::vector<Unit*>& allyUnits)
{
	Unit::update(t_deltaTime, allyUnits);
}

void AircraftUnit::render(sf::RenderWindow& m_window)
{
	Unit::render(m_window);
}
