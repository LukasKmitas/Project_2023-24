#pragma once
#include "AircraftUnit.h"

class HammerHead : public AircraftUnit
{
public:
	HammerHead();
	~HammerHead();

	void update(sf::Time t_deltaTime, std::vector<Unit*>& allUnits) override;

private:

	void setupHammerhead();

};
