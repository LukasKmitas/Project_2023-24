#pragma once
#include "InfantryUnit.h"

class RiflemanSquad : public InfantryUnit
{
public:

	RiflemanSquad();
	~RiflemanSquad();

	void update(sf::Time t_deltaTime, std::vector<Unit*>& m_allyUnits) override;
	void render(sf::RenderWindow& m_window) override;

	void initSquad();

private:

	void initRifleman();
	void movement(sf::Time t_deltaTime);
	void updateFormation();

	void takeDamageSquad();
	void squadEntityRemoval() override;
	void squadEntityRegain() override;

	void aimAt(const std::vector<Unit*>& m_enemyUnits);

	std::vector<sf::Sprite> m_entities;
	std::vector<sf::Vector2f> m_offsets;

	std::vector<sf::Vector2f> m_targetPositions;

	sf::Vector2f m_previousPosition;

	int m_numberOfSoldiers = 4;

	float m_scatterRadius = 30.0f;

	int m_shotsFired = 0;
	int m_clipSize = 4; 
	float m_reloadTime = 2.0f;
	float m_fireRate = 0.2f;
	float m_fireTimer = 0.0f;
	float m_reloadTimer = 0.0f;
	bool m_isReloading = false;
};

