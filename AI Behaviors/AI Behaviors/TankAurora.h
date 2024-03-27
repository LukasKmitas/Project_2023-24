#pragma once
#include "VehicleUnit.h"

class TankAurora : public VehicleUnit
{
public:
	TankAurora();
	~TankAurora();

	void update(sf::Time t_deltaTime, std::vector<Unit*>& allUnits) override;
	void render(sf::RenderWindow& m_window) override;

private:

	void setupTankAurora();
	void setupHealingAura();

	void movement(sf::Time t_deltaTime);

	void spawnElectricalParticleEffect();

	sf::CircleShape m_healingAura;
	sf::Shader m_healingAuraShader;
	sf::Clock m_shaderClock;

	sf::Texture m_lightningTexture;

	float m_healingRange = 150.0f;

	bool isChargingEMPWave = false;
	
};

