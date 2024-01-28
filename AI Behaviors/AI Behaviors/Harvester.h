#pragma once
#include "Vehicle.h"
#include "Refinery.h"

class Harvester : public Vehicle
{
public:
	enum State
	{
		Idle,
		MovingToResource,
		CollectingResource,
		MovingToRefinery,
		UnloadingResource
	};

	Harvester();
	~Harvester();

	void update(sf::Time t_deltaTime, std::vector<Unit*>& allUnits) override;
	void render(sf::RenderWindow& m_window);

	void setBuildings(const std::vector<Building*>& buildings);

	State currentState = Idle;

private:

	void movement(sf::Time t_deltaTime);
	void findNearestRefinery();
	void moveToRefinery(Refinery* refinery);

	void setupHarvester();

	sf::Vector2f resourcePosition;

	const std::vector<Building*>* allBuildings;

	float const RESOURCE_CAPACITY = 100;
	float currentResourceAmount;

};

