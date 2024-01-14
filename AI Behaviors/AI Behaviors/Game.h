#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>
#include <filesystem>

#include "GameState.h"
#include "MainMenu.h"
#include "LevelEditor.h"
#include "GUI.h"
#include "Global.h"
#include "BuildingType.h"

namespace fs = std::experimental::filesystem;

class Game
{
public:
	Game();
	~Game();

	void run();

private:
	GameState m_currentState = GameState::MainMenu;
	GameState m_previousState;
	MainMenu m_menu;
	LevelEditor m_levelEditor;
	GUI m_gui{ placedBuildings, m_selectedBuildingType, m_levelEditor.m_tiles };
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

	void resetView();

	void goToMainMenu();
	void initLevelSelectionButtons();
	void initBackButton();
	void initBackgroundImage();

	void handleLevelSelectionMouseInput(sf::Vector2i mousePosition);
	void loadLevel(const std::string& filename);

	std::vector<sf::RectangleShape> levelSelectionButtons;
	std::vector<std::string> levelFilenames;

	sf::RenderWindow m_window;
	sf::View gameView;
	sf::Font m_font;
	sf::Text levelSelectionButtonText;
	sf::Text m_toGoBackText;

	sf::RectangleShape m_toGoBackButton;
	
	sf::Texture m_backgroundTexture;
	sf::Sprite m_backGroundSprite;

	sf::Vector2f cameraVelocity;

	int selectedButtonIndex;

	float viewMoveSpeed = 5.0f;
	float minX = 950;
	float minY = 530;
	float maxX = 1550;
	float maxY = 1970;

	float buttonWidth = 200.0f;
	float buttonHeight = 50.0f;

	bool levelLoaded = false;
	bool m_exitGame;

};

#endif