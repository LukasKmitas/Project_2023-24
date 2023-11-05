#pragma once
#include "Building.h"

class Refinery : public Building
{
public:
	Refinery();
	~Refinery();

	void update(sf::Time deltaTime) override;
	void render(sf::RenderWindow& m_window) const override;

private:

	void setupSprite();

};

