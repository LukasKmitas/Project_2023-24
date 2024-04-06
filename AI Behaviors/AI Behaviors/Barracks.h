#pragma once
#include "Building.h"

class Barracks : public Building
{
public:
	Barracks();
	~Barracks();

	void update(sf::Time t_deltaTime) override;
	void render(sf::RenderWindow& m_window) const override;

private:

	void setupSprite();

};
