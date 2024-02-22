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
#include "Tile.h"

#include "ParticleSystem.h"

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
	std::vector<Unit*> units;
	std::vector<Unit*> enemyUnits;
	Unit* m_selectedUnit = nullptr;

	Tile m_tiles;
	ParticleSystem m_particleSystem;

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
	void initParticles();

	void handleLevelSelectionMouseInput(sf::Vector2i mousePosition);
	void loadLevel(const std::string& filename);

	void updateFogOfWarBasedOnBuildings(const std::vector<Building*>& buildings);
	void updateFogOfWarBasedOnUnits(const std::vector<Unit*>& units);

	void createUnit();
	void createEnemyUnit();

	void selectUnitAt(const sf::Vector2f& mousePos);
	void selectUnitsInBox();

	void spawnBubbleParticles();
	void spawnBulletSparkParticles(const sf::Vector2f& position);

	int calculateGridSize(int numberOfUnits);

	std::vector<sf::RectangleShape> levelSelectionButtons;
	std::vector<std::string> levelFilenames;

	sf::RenderWindow m_window;
	sf::View gameView;
	sf::Font m_font;
	sf::Text levelSelectionButtonText;
	sf::Text m_toGoBackText;

	sf::RectangleShape m_toGoBackButton;
	
	sf::Texture m_bubbleTexture;
	sf::Texture m_bulletSparksTexture;

	sf::Texture m_cursorTexture;
	sf::Sprite m_cursorSprite;

	sf::Vector2f cameraVelocity;

	bool isDragging = false;
	sf::Vector2f dragStart;
	sf::Vector2f dragEnd;
	sf::RectangleShape selectionBox;

	int selectedButtonIndex;

	float viewMoveSpeed = 5.0f;
	float minX = 950;
	float minY = 530;
	float maxX = 1550;
	float maxY = 1970;

	float buttonWidth = 200.0f;
	float buttonHeight = 50.0f;

	// Particles variables
	float angleParticle;
	float speedParticle;
	float lifetimeParticle;
	float sizeParticle;
	sf::Color colorParticle;

	bool levelLoaded = false;
	bool once = false; // temp
	bool m_exitGame;

};

#endif