#pragma once
#include "VehicleUnit.h"

class Buggy : public VehicleUnit
{
public:
	Buggy();
	~Buggy();

	void update(sf::Time t_deltaTime, std::vector<Unit*>& m_allyUnits) override;

	UnitType getUnitType() const override { return UnitType::Buggy; }

private:

	void setupBuggy();

	void movement(sf::Time t_deltaTime);
	void aimWeapon(const std::vector<Unit*>& m_enemyUnits);

	void shootAtEnemy();

	int m_shotsFired = 0;
	int m_clip = 8;
	float m_reloadTime = 2.0f;
	float m_reloadTimer = 0.0f;
	float m_fireRate = 0.2f;
	float m_fireTimer = 0.0f;
};
