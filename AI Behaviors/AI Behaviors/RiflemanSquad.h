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
	void updateFormation();

	std::vector<sf::Sprite> m_entities;
	std::vector<sf::Vector2f> m_offsets;

	sf::Vector2f previousPosition;

	int numberOfSoldiers = 4;

	float scatterRadius = 30.0f;

};

