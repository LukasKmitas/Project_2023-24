#pragma once
#include "VehicleUnit.h"

class Buggy : public VehicleUnit
{
public:
	Buggy();
	~Buggy();

	void update(sf::Time t_deltaTime, std::vector<Unit*>& allUnits) override;
	void setEnemyUnits(std::vector<Unit*>& enemyUnits);

private:

	void setupBuggy();

	void movement(sf::Time t_deltaTime);
	void aimWeapon(sf::Time t_deltaTime, const std::vector<Unit*>& enemyUnits);

	void shootAtEnemy();

	std::vector<Unit*>* enemyUnits = nullptr;
	Unit* closestEnemy = nullptr;

	int shotsFired = 0;
	int clip = 8;
	bool isReloading = false;
	float reloadTime = 2.0f;
	float reloadTimer = 0.0f;
	float closestDistance;
	float fireRate = 0.2f;
	float fireTimer = 0.0f;
};
