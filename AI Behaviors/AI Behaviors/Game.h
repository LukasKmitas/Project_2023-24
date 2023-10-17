#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>
#include "GUI.h"
#include "Global.h"
//#include "BuildingType.h"

#include "Headquarters.h"

class Game
{
public:
	Game();
	~Game();

	void run();

private:
	GUI m_gui;
	Headquarters m_headquaters;
	//BuildingType m_selectedBuildingType = BuildingType::Headquarters;

	void processEvents();
	void processKeys(sf::Event t_event);
	void update(sf::Time t_deltaTime);
	void render();

	void updateView();

	sf::RenderWindow m_window;
	sf::View gameView;
	sf::Font m_ArialBlackfont;

	float viewMoveSpeed = 5.0f;
	float minX = -500;
	float minY = -500;
	float maxX = 500;
	float maxY = 500;

	bool m_exitGame;

};

#endif