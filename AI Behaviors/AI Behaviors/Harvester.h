#pragma once
#include "Vehicle.h"

class Harvester : public Vehicle
{
public:
	Harvester();
	~Harvester();

	void update(sf::Time t_deltaTime);
	void render(sf::RenderWindow& m_window);

private:

	void movement(sf::Time t_deltaTime);
	void setupHarvester();

	float const RESOURCE_CAPACITY = 100;
	float currentResourceAmount;

};

