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
	void setupEnergyWave();

	void movement(sf::Time t_deltaTime);

	void spawnElectricalParticleEffect(sf::Time t_deltaTime);

	void startEMPCharge();
	void emitEMP();

	void resetEnergyWave();

	sf::CircleShape m_healingAura;
	sf::Shader m_healingAuraShader;
	sf::Clock m_shaderClock;

	sf::Sprite m_energyWaveSprite;
	sf::Texture m_energyWaveTexture;
	sf::Texture m_lightningTexture;

	sf::Vector2f directionToEnemy;

	float m_healingRange = 150.0f;

	// Electicity stuff
	bool isChargingEMPWave = true;
	float empChargeTime = 6.0f;
	float currentChargeTime = 0.0f;
	float empExpandArea = 15.0f;

	// Energy Wave stuff
	sf::Shader waveShader;
	sf::Clock clock;

	bool isFiringEnergyWave = false;
	sf::Vector2f energyWavePosition;
	float energyWaveSpeed = 200.0f;
	float energyWaveSize = 0.4f; 
	float energyWaveMaxSize = 1.5f;
	float energyWaveGrowthRate = 0.5f;
};

