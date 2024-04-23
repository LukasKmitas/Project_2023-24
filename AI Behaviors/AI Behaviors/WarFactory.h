#pragma once
#include "Building.h"

class WarFactory : public Building
{
public:
	WarFactory();
	~WarFactory();

	void update(sf::Time t_deltaTime) override;
	void render(sf::RenderWindow& m_window) override;

private:

	void setupSprite();

};

