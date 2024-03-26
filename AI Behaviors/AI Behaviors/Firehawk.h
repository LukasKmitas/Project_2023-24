#pragma once
#include "AircraftUnit.h"

class Firehawk : public AircraftUnit
{
public:
	Firehawk();
	~Firehawk();

	void update(sf::Time t_deltaTime, std::vector<Unit*>& allUnits) override;

private:

	void approachTarget(const sf::Vector2f& targetPos, sf::Time t_deltaTime);
	void orbitTarget(sf::Time t_deltaTime);

	void setupFirehawk();

	void fireMissileAtTarget(const sf::Vector2f& targetPos);

	sf::Texture missileTexture;

	float m_orbitRadius = 150.0f;
	float m_orbitSpeed = 0.0f;
	float m_currentOrbitAngle = 0.0f;

	// Missile stuff
	float reloadCooldown = 0.0f; // Time until the Firehawk can fire again after a burst
	const float reloadCooldownTime = 5.0f; // Cooldown time for reloading missiles
	int missilesFiredInBurst = 0; // Count of missiles fired in the current burst
	const int burstSize = 4;
};

