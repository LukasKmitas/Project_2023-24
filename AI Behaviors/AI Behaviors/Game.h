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
#include "NeuralNetworks.h"
#include "EnemyAIState.h"
#include "WinLoseState.h"

namespace fs = std::experimental::filesystem;

class Game
{
public:
	Game();
	~Game();

	void run();

private:

	GameState m_currentState = GameState::MainMenu;
	WinLoseState m_gameWinLose = WinLoseState::NONE;
	GameState m_previousState;
	MainMenu m_menu;
	Tile m_tiles;
	LevelEditor m_levelEditor;
	LevelLoader m_levelLoader;
	GUI m_gui{ placedPlayerBuildings, m_selectedBuildingType, m_levelEditor.m_tiles };
	BuildingType m_selectedBuildingType = BuildingType::None;
	ParticleSystem m_particleSystem;
	NeuralNetworks m_neural_network;
	Unit* m_selectedUnit = nullptr;
	std::vector<Building*> placedPlayerBuildings;
	std::vector<Building*> placedEnemyBuildings;
	std::vector<Unit*> playerUnits;
	std::vector<Unit*> enemyUnits;
	std::map<BuildingType, int> enemyBuildingCounts;

	void processEvents();
	void processKeys(sf::Event t_event);
	void update(sf::Time t_deltaTime);
	void render();

	// Player stuff 
	void createBuilding(sf::RenderWindow& window);
	void createBase();
	void updatePlayerAssets(sf::Time t_deltaTime);

	void saveLevel();
	void loadLevel(const std::string& filename);
	void checkVictoryConditions();

	// Enemy AI stuff
	EnemyAIState enemyAIState = EnemyAIState::Exploring;
	sf::Time enemyStateTimer = sf::seconds(0);
	void createEnemyStarterBase();
	void updateEnemyAIDecisionOnCreating(sf::Time t_deltaTime);
	void updateEnemyAIUnitDecisionState(sf::Time deltaTime);
	void updateEnemyAssets(sf::Time t_deltaTime);
	void createEnemyUnit(const std::string& unitType);
	void createEnemyHarvesterUnit();
	void updateBuildingCounts();
	void decideNextEnemyBuilding();
	void placeEnemyBuilding(BuildingType type);
	sf::Vector2f findPlacementPositionNearExistingBuilding();
	void moveEnemyUnits();
	sf::Vector2f findNearestPlayerObjectPosition(const sf::Vector2f& enemyPosition);
	float distanceSquaredBetweenPoints(const sf::Vector2f& p1, const sf::Vector2f& p2);
	sf::Vector2f enemyRefineryBuildingPosition;
	Building* newEnemyBuilding = nullptr;
	void enemyExploring(sf::Time deltaTime);
	void enemyGroupUnits(sf::Time deltaTime);
	void enemyAttacking(sf::Time deltaTime);
	std::vector<sf::Vector2f> getValidExplorationTargets();
	sf::Vector2f findEnemyHeadquartersPosition();
	bool isPositionWithinMap(const sf::Vector2f& position);
	std::vector<sf::Vector2f> validTargets;

	void moveCamera(sf::Vector2f mousePosition);
	void updateViewWithMouse();
	void resetView();
	void resetZoom();

	void initParticles();
	void initializeNeuralNetwork();

	void updateNeuralNetwork();
	void drawNeuralNetwork(sf::RenderWindow& window);

	void updateFogOfWarBasedOnBuildings(const std::vector<Building*>& buildings);
	void updateFogOfWarBasedOnUnits(const std::vector<Unit*>& units);

	void createUnit();
	void selectUnitAt(const sf::Vector2f& mousePos);
	void selectUnitsInBox();

	void spawnBubbleParticles();
	void spawnBulletSparkParticles(const sf::Vector2f& position);
	void spawnExplosionParticle(const sf::Vector2f& position);

	void initWinLosePanel();
	void renderWinLosePanel(sf::RenderWindow& window);
	void handleVictoryPanelInput(const sf::Vector2f& m_mousePosition);
	
	void gameReset();
	void clearGameEntities();

	int calculateGridSize(int numberOfUnits);

	sf::RenderWindow m_window;
	sf::View gameView;
	sf::Font m_font;
	sf::Font m_normalfont;

	sf::Texture m_bubbleTexture;
	sf::Texture m_bulletSparksTexture;

	sf::Texture m_cursorTexture;
	sf::Sprite m_cursorSprite;

	sf::Vector2f viewCenter = gameView.getCenter();
	sf::Vector2i guiMousePosition;
	sf::Vector2f worldMousePosition;
	sf::Vector2f cameraVelocity;
	const float minZoomLevel = 0.5f;
	const float maxZoomLevel = 1.5f;
	float currentZoomLevel = 1.0f;
	float viewMoveSpeed = 5.0f;
	float minX = 950;
	float minY = 530;
	float maxX = 1550;
	float maxY = 1970;
	int mapWidth = 2400;
	int mapHeight = 2400;

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

	bool levelLoaded = false;
	bool m_exitGame;

	// Neural Network stuff
	int mouse_x;
	int mouse_y;
	int outputHeight = 64;
	int outputWidth = 64;
	float total_error = 0;
	float dot_x;
	float dot_y;
	const int TRAININGS_PER_FRAME = 1000;
	bool train = false;

	vector_2d errors;
	std::vector<std::vector<float>> neural_network;
	std::vector<std::vector<std::vector<float>>> weights;
	std::array<int, 2> hiddenNeurons = m_neural_network.getHiddenNeurons();
	std::array<int, 3> biasNeurons = m_neural_network.getBiasNeurons();
	std::mt19937_64 random_engine;
	sf::Text errorText;
	sf::Image outputImage;
	sf::Sprite outputSprite;
	sf::Texture outputTexture;

	// Win/Lose stuff
	bool showWinLosePanel = false;
	sf::Sprite PanelBackgroundSprite;
	sf::Texture PanelBackgroundTexture;
	sf::Text winLoseText;
	sf::Text playAgainText;
	sf::Text exitText;
	sf::RectangleShape playAgainButton;
	sf::RectangleShape exitButton;

	sf::Text playerStatsText;
	sf::Text enemyStatsText;
	int playerUnitStatCount = 0;
	int playerBuildingStatCount = 0;
	int enemyUnitStatCount = 0;
	int enemyBuildingStatCount = 0;

};

#endif