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
	void setupParticleExhaustEffect();

	void movement(sf::Time t_deltaTime);
	void aimWeapons(const std::vector<Unit*>& enemyUnits);
	void shootAtEnemy();

	float fireRate = 0.2f; 
	float fireTimer = 0.0f; 
	float bulletSpeed = 300.0f;
};
