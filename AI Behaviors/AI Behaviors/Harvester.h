#pragma once
#include "VehicleUnit.h"
#include "Refinery.h"
#include "Tile.h"

class Harvester : public VehicleUnit
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

	void setBuildings(const std::vector<Building*>& buildings);
	void setTiles(const std::vector<std::vector<Tile>>& tiles);

	State currentState = Idle;

private:

	Refinery* targetRefinery = nullptr;

	void movement(sf::Time t_deltaTime);

	void findNearestResourceTile();
	void findNearestRefinery();

	void gettingResourcePosition(const sf::Vector2f& resourcePos);
	void moveToRefinery(Refinery* refinery);

	void unloadResources(sf::Time t_deltaTime);

	bool isAtTargetPosition();
	bool isAtRefinery();

	void setupHarvester();

	sf::Vector2f resourcePosition;

	const std::vector<Building*>* allBuildings;
	const std::vector<std::vector<Tile>>* tiles;

	float collectionRate = 4.0f; 
	float maxResourceCapacity = 100.0f; 
	float currentResourceLoad = 0.0f;

	bool foundResource = false;
};

