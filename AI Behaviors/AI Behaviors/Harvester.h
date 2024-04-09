#pragma once
#include "VehicleUnit.h"
#include "Refinery.h"

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

	void update(sf::Time t_deltaTime, std::vector<Unit*>& m_allyUnits) override;

	void setBuildings(const std::vector<Building*>& m_buildings);

	State m_currentState = Idle;

private:

	Refinery* m_targetRefinery = nullptr;

	void movement(sf::Time t_deltaTime);

	void findNearestResourceTile();
	void findNearestRefinery();

	void gettingResourcePosition(const sf::Vector2f& m_resourcePos);
	void moveToRefinery(Refinery* m_refinery);

	void unloadResources(sf::Time t_deltaTime);

	bool isAtTargetPosition();
	bool isAtRefinery();

	void initHarvester();

	sf::Vector2f m_resourcePosition;

	const std::vector<Building*>* m_allBuildings;

	float m_collectionRate = 4.0f; 
	float m_maxResourceCapacity = 100.0f; 
	float m_currentResourceLoad = 0.0f;

	bool m_foundResource = false;
};

