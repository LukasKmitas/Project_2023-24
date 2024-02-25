#ifndef GAME_HPP
#define GAME_HPP

#include <iostream>
#include <SFML/Graphics.hpp>
#include <filesystem>
#include "GameState.h"
#include "MainMenu.h"
#include "LevelEditor.h"
#include "LevelLoader.h"
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
	Tile m_tiles;
	LevelEditor m_levelEditor;
	LevelLoader m_levelLoader;
	GUI m_gui{ placedBuildings, m_selectedBuildingType, m_levelEditor.m_tiles };
	BuildingType m_selectedBuildingType = BuildingType::None;
	std::vector<Building*> placedBuildings;
	std::vector<Unit*> units;
	std::vector<Unit*> enemyUnits;
	Unit* m_selectedUnit = nullptr;
	ParticleSystem m_particleSystem;

	void processEvents();
	void processKeys(sf::Event t_event);
	void update(sf::Time t_deltaTime);
	void render();

	void createBuilding(sf::RenderWindow& window);
	void createBase();
	void saveLevel();
	void loadLevel(const std::string& filename);

	void moveCamera(sf::Vector2f mousePosition);
	void updateViewWithMouse();
	void resetView();
	void resetZoom();

	void initLevelSelectionButtons();
	void initParticles();

	void updateFogOfWarBasedOnBuildings(const std::vector<Building*>& buildings);
	void updateFogOfWarBasedOnUnits(const std::vector<Unit*>& units);

	void createUnit();
	void createEnemyUnit();
	void selectUnitAt(const sf::Vector2f& mousePos);
	void selectUnitsInBox();

	void spawnBubbleParticles();
	void spawnBulletSparkParticles(const sf::Vector2f& position);

	int calculateGridSize(int numberOfUnits);

	sf::RenderWindow m_window;
	sf::View gameView;
	sf::Font m_font;

	sf::Texture m_bubbleTexture;
	sf::Texture m_bulletSparksTexture;

	sf::Texture m_cursorTexture;
	sf::Sprite m_cursorSprite;

	sf::Vector2f viewCenter = gameView.getCenter();
	sf::Vector2i guiMousePosition;
	sf::Vector2f worldMousePosition;
	sf::Vector2f cameraVelocity;
	const float minZoomLevel = 0.6f;
	const float maxZoomLevel = 1.5f;
	float currentZoomLevel = 1.0f;
	float viewMoveSpeed = 5.0f;
	float minX = 950;
	float minY = 530;
	float maxX = 1550;
	float maxY = 1970;

	bool isDragging = false;
	sf::Vector2f dragStart;
	sf::Vector2f dragEnd;
	sf::RectangleShape selectionBox;

	// Particles variables
	float angleParticle;
	float speedParticle;
	float lifetimeParticle;
	float sizeParticle;
	sf::Color colorParticle;

	bool once = false; // temp
	bool m_exitGame;

	std::vector<std::string> levelFilenames;
	int selectedButtonIndex;
	sf::Text m_toGoBackText;
	sf::RectangleShape m_toGoBackButton;
	std::vector<sf::RectangleShape> levelSelectionButtons;
	sf::Text levelSelectionButtonText;
	float buttonWidth = 200.0f;
	float buttonHeight = 50.0f;
	bool levelLoaded = false;
	sf::Texture m_buttonTexture;

};

#endif