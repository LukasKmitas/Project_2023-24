#pragma once
#include "AircraftUnit.h"

class HammerHead : public AircraftUnit
{
public:
	HammerHead();
	~HammerHead();

	void update(sf::Time t_deltaTime, std::vector<Unit*>& m_allyUnits) override;
	void render(sf::RenderWindow& m_window) override;

private:

	void initHammerhead();
	
	void spawnParticleExhaustEffect();

	void movement(sf::Time t_deltaTime);
	void aimWeapons(const std::vector<Unit*>& m_enemyUnits);
	void shootAtEnemy();

	sf::Sprite m_leftGunSprite;
	sf::Sprite m_rightGunSprite;

	float m_fireRate = 0.2f; 
	float m_fireTimer = 0.0f; 
	float m_bulletSpeed = 300.0f;
};
