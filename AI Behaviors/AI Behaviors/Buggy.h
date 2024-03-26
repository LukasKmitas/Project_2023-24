#pragma once
#include "VehicleUnit.h"

class Buggy : public VehicleUnit
{
public:
	Buggy();
	~Buggy();

	void update(sf::Time t_deltaTime, std::vector<Unit*>& allUnits) override;

private:

	void setupBuggy();

	void movement(sf::Time t_deltaTime);
	void aimWeapon(const std::vector<Unit*>& enemyUnits);

	void shootAtEnemy();

	int shotsFired = 0;
	int clip = 8;
	float reloadTime = 2.0f;
	float reloadTimer = 0.0f;
	float fireRate = 0.2f;
	float fireTimer = 0.0f;
};
