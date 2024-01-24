#pragma once
#include "Vehicle.h"

class Harvester : public Vehicle
{
public:
	Harvester();
	~Harvester();

	void update(sf::Time t_deltaTime);
	void render(sf::RenderWindow& m_window);

	void setPosition(const sf::Vector2f& position);

private:

	void setupHarvester();

	float const RESOURCE_CAPACITY = 100;
	float currentResourceAmount;

};

