#pragma once
#include "VehicleUnit.h"

class TankAurora : public VehicleUnit
{
public:
	TankAurora();
	~TankAurora();

	void update(sf::Time t_deltaTime, std::vector<Unit*>& m_allUnits) override;
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

	sf::Vector2f m_directionToEnemy;

	float m_healingRange = 150.0f;

	// Electicity stuff
	bool m_isChargingEMPWave = true;
	float m_empChargeTime = 6.0f;
	float m_currentChargeTime = 0.0f;
	float m_empExpandArea = 15.0f;

	// Energy Wave stuff
	sf::Shader m_waveShader;
	sf::Clock m_clock;

	bool m_isFiringEnergyWave = false;
	sf::Vector2f m_energyWavePosition;
	float m_energyWaveSpeed = 200.0f;
	float m_energyWaveSize = 0.4f; 
	float m_energyWaveMaxSize = 1.5f;
	float m_energyWaveGrowthRate = 0.5f;
};

