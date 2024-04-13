#include "AircraftUnit.h"

AircraftUnit::AircraftUnit()
{
	m_unitTypeClass = UnitTypeClass::Air;
}

AircraftUnit::~AircraftUnit()
{
}

void AircraftUnit::update(sf::Time t_deltaTime, std::vector<Unit*>& m_allyUnits)
{
	Unit::update(t_deltaTime, m_allyUnits);
}

void AircraftUnit::render(sf::RenderWindow& m_window)
{
	Unit::render(m_window);
}
