#pragma once
#include "Building.h"

class Vehicle : public Building
{
public:
	Vehicle();
	~Vehicle();

	void update(sf::Time deltaTime) override;
	void render(sf::RenderWindow& m_window) const override;

private:

	void setupSprite();

};

