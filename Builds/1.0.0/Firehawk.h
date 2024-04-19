#pragma once
#include "AircraftUnit.h"

class Firehawk : public AircraftUnit
{
public:
	Firehawk();
	~Firehawk();

	void update(sf::Time t_deltaTime, std::vector<Unit*>& m_allyUnits) override;

	UnitType getUnitType() const override { return UnitType::Firehawk; }

private:

	void approachTarget(const sf::Vector2f& m_targetPos, sf::Time t_deltaTime);
	void orbitTarget(sf::Time t_deltaTime);

	void initFirehawk();

	void fireMissileAtTarget(const sf::Vector2f& m_targetPos);

	bool isTargetWithinRange(const sf::Vector2f& m_targetPos);

	void launchMissile(const sf::Vector2f& m_targetPos);

	sf::Texture m_missileTexture;

	float m_orbitRadius = 150.0f;
	float m_orbitSpeed = 0.0f;
	float m_currentOrbitAngle = 0.0f;

	// Missile stuff
	float m_reloadCooldown = 0.0f; 
	const float m_reloadCooldownTime = 5.0f;
	int m_missilesFiredInBurst = 0;
	const int m_burstSize = 4;
	int m_phase = 0;
	float m_phaseTimer = 0.0f;
	const float m_phaseDelay = 1.0f;

};

