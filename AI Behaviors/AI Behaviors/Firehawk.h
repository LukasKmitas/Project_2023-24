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

	float m_orbitRadius = 150.0f;
	float m_orbitSpeed = 0.0f;
	float m_currentOrbitAngle = 0.0f;
};

