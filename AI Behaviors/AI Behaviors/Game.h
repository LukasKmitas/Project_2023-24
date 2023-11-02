#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>
#include "GUI.h"
#include "Global.h"
#include "Grid.h"
#include "Headquarters.h"

class Game
{
public:
	Game();
	~Game();

	void run();

private:
	GUI m_gui;
	Grid m_grid;
	Headquarters* m_headquaters;

	void processEvents();
	void processKeys(sf::Event t_event);
	void update(sf::Time t_deltaTime);
	void render();

	void updateView();

	sf::RenderWindow m_window;
	sf::View gameView;
	sf::Font m_ArialBlackfont;

	sf::Vector2f cameraVelocity;
	float viewMoveSpeed = 5.0f;
	float minX = 950;
	float minY = 530;
	float maxX = 1550;
	float maxY = 1970;

	bool m_exitGame;

};

#endif