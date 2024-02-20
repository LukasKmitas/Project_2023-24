#pragma once
#include "Unit.h"

class AircraftUnit : public Unit
{
public:
	AircraftUnit();
	~AircraftUnit();

	void update(sf::Time t_deltaTime, std::vector<Unit*>& allUnits) override;

};
