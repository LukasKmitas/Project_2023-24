#pragma once
#include "Building.h"

class Refinery : public Building
{
public:
	Refinery();
	~Refinery();

	void update(sf::Time t_deltaTime) override;
	void render(sf::RenderWindow& m_window) override;

private:

	void setupSprite();

};

