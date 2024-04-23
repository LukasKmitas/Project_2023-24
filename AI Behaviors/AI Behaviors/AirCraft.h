#pragma once
#include "Building.h"

class AirCraft : public Building
{
public:
	AirCraft();
	~AirCraft();

	void update(sf::Time t_deltaTime) override;
	void render(sf::RenderWindow& m_window) override;

private:

	void setupSprite();

};

