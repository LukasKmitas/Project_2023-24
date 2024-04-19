#include "InfantryUnit.h"

InfantryUnit::InfantryUnit()
{
}

InfantryUnit::~InfantryUnit()
{
}

void InfantryUnit::update(sf::Time t_deltaTime, std::vector<Unit*>& m_allyUnits)
{
	Unit::update(t_deltaTime, m_allyUnits);
}

void InfantryUnit::render(sf::RenderWindow& m_window)
{
	Unit::render(m_window);
}
