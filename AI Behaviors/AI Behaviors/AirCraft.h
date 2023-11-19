#pragma once
#include "Building.h"

class AirCraft : public Building
{
public:
	AirCraft();
	~AirCraft();

	void update(sf::Time deltaTime) override;
	void render(sf::RenderWindow& m_window) const override;

private:

	void setupSprite();

};

