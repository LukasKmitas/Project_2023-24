#pragma once
#include "InfantryUnit.h"

class RiflemanSquad : public InfantryUnit
{
public:

	RiflemanSquad();
	~RiflemanSquad();

	void update(sf::Time t_deltaTime, std::vector<Unit*>& allyUnits) override;
	void render(sf::RenderWindow& m_window) override;

	void initSquad();

private:

	void initRifleman();
	void movement(sf::Time t_deltaTime);
	void updateFormation();

	void takeDamageSquad();
	void squadEntityRemoval() override;
	void squadEntityRegain() override;

	void shootAt(const std::vector<Unit*>& enemyUnits);

	std::vector<sf::Sprite> m_entities;
	std::vector<sf::Vector2f> m_offsets;

	sf::Vector2f previousPosition;

	int numberOfSoldiers = 4;

	float scatterRadius = 30.0f;

	int shotsFired = 0;
	int clipSize = 4; 
	float reloadTime = 2.0f;
	float fireRate = 0.2f;
	float fireTimer = 0.0f;
	float reloadTimer = 0.0f;
	bool isReloading = false;
};

