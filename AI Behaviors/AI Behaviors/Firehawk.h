#pragma once
#include "AircraftUnit.h"

class Firehawk : public AircraftUnit
{
public:
	Firehawk();
	~Firehawk();

	void update(sf::Time t_deltaTime, std::vector<Unit*>& allyUnits) override;

private:

	void approachTarget(const sf::Vector2f& targetPos, sf::Time t_deltaTime);
	void orbitTarget(sf::Time t_deltaTime);

	void setupFirehawk();

	void fireMissileAtTarget(const sf::Vector2f& targetPos);

	bool isTargetWithinRange(const sf::Vector2f& targetPos);

	sf::Texture missileTexture;

	float m_orbitRadius = 150.0f;
	float m_orbitSpeed = 0.0f;
	float m_currentOrbitAngle = 0.0f;

	// Missile stuff
	float reloadCooldown = 0.0f; 
	const float reloadCooldownTime = 5.0f;
	int missilesFiredInBurst = 0;
	const int burstSize = 4;
};

