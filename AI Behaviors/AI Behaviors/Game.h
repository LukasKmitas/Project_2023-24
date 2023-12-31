#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>
#include "GameState.h"
#include "MainMenu.h"
#include "LevelEditor.h"
#include "GUI.h"
#include "Global.h"
#include "BuildingType.h"

class Game
{
public:
	Game();
	~Game();

	void run();

private:
	GameState m_currentState = GameState::MainMenu;
	MainMenu m_menu;
	LevelEditor m_levelEditor;
	GUI m_gui{ placedBuildings, m_selectedBuildingType };
	BuildingType m_selectedBuildingType = BuildingType::None;
	std::vector<Building*> placedBuildings;

	void processEvents();
	void processKeys(sf::Event t_event);
	void update(sf::Time t_deltaTime);
	void render();

	void updateView();
	void createBuilding(sf::RenderWindow& window);
	void createBase();
	void saveLevel();
	void loadLevelForPlay();

	sf::RenderWindow m_window;
	sf::View gameView;
	sf::Font m_ArialBlackfont;

	sf::Vector2f cameraVelocity;
	float viewMoveSpeed = 5.0f;
	float minX = 950;
	float minY = 530;
	float maxX = 1550;
	float maxY = 1970;

	bool levelLoaded = false;
	bool m_exitGame;

};

#endif