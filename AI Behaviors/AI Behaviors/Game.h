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
	GUI m_gui{ m_placedPlayerBuildings, m_selectedBuildingType, m_levelEditor.m_tiles };
	BuildingType m_selectedBuildingType = BuildingType::None;
	ParticleSystem m_particleSystem;
	NeuralNetworks m_neural_network;
	Unit* m_selectedUnit = nullptr;
	std::vector<Building*> m_placedPlayerBuildings;
	std::vector<Building*> m_placedEnemyBuildings;
	std::vector<Unit*> m_playerUnits;
	std::vector<Unit*> m_enemyUnits;
	std::map<BuildingType, int> m_enemyBuildingCounts;

	void processEvents();
	void processKeys(sf::Event t_event);
	void update(sf::Time t_deltaTime);
	void render();

	// Player stuff 
	void createBuilding(sf::RenderWindow& m_window);
	void createBase();
	void updatePlayerAssets(sf::Time t_deltaTime);
	void createUnit();
	void selectUnitAt(const sf::Vector2f& m_mousePos);
	void selectUnitsInBox();

	void saveLevel();
	void loadLevel(const std::string& m_filename);
	void checkVictoryConditions();

	// Enemy AI stuff
	EnemyAIState m_enemyAIState = EnemyAIState::Exploring;
	void createEnemyStarterBase();
	void updateEnemyAIDecisionOnCreating(sf::Time t_deltaTime);
	void updateEnemyAIUnitDecisionState(sf::Time t_deltaTime);
	void updateEnemyAssets(sf::Time t_deltaTime);
	void createEnemyUnit(const std::string& m_unitType);
	void createEnemyHarvesterUnit();
	void updateBuildingCounts();
	void decideNextEnemyBuilding();
	void placeEnemyBuilding(BuildingType m_type);
	void moveEnemyUnits();
	void enemyExploring(sf::Time t_deltaTime);
	void enemyGroupUnits(sf::Time t_deltaTime);
	void enemyAttacking(sf::Time t_deltaTime);
	bool checkPositionWithinMap(const sf::Vector2f& m_position);
	float distanceBetweenPoints(const sf::Vector2f& p1, const sf::Vector2f& p2);
	sf::Vector2f findNearestPlayerObjectPosition(const sf::Vector2f& m_enemyPosition);
	sf::Vector2f findPlacementPositionNearExistingBuilding();
	std::vector<sf::Vector2f> getValidExplorationTargets();
	sf::Vector2f findEnemyHeadquartersPosition();
	Building* newEnemyBuilding = nullptr;
	sf::Vector2f enemyRefineryBuildingPosition;
	std::vector<sf::Vector2f> m_validTargets;
	sf::Time m_enemyStateTimer = sf::seconds(0);

	// m_windows/camera/view stuff
	void moveCamera(sf::Vector2f m_mousePosition);
	void updateViewWithMouse();
	void resetView();
	void resetZoom();

	//Neural networks stuff
	void initNeuralNetwork();
	void updateNeuralNetwork();
	void drawNeuralNetwork(sf::RenderWindow& m_window);

	// Updates the fog of war 
	void updateFogOfWarBasedOnBuildings(const std::vector<Building*>& m_buildings);
	void updateFogOfWarBasedOnUnits(const std::vector<Unit*>& m_units);

	// Particle stuff
	void initParticles(); 
	void spawnBubbleParticles();
	void spawnBulletSparkParticles(const sf::Vector2f& m_position);
	void spawnExplosionParticle(const sf::Vector2f& m_position);

	void initWinLosePanel();
	void renderWinLosePanel(sf::RenderWindow& m_window);
	void handleWinLosePanel(const sf::Vector2f& m_mousePosition);
	
	void gameReset();
	void clearGameEntities();

	int calculateGridSize(int m_numberOfUnits);

	sf::RenderWindow m_window;
	sf::View m_gameView;
	sf::Font m_font;
	sf::Font m_normalfont;

	sf::Texture m_bubbleTexture;
	sf::Texture m_bulletSparksTexture;

	sf::Texture m_cursorTexture;
	sf::Sprite m_cursorSprite;

	sf::Vector2f m_viewCenter = m_gameView.getCenter();
	sf::Vector2i m_guiMousePosition;
	sf::Vector2f m_worldMousePosition;
	sf::Vector2f m_cameraVelocity;
	const float m_minZoomLevel = 0.5f;
	const float m_maxZoomLevel = 1.5f;
	float m_currentZoomLevel = 1.0f;
	float m_viewMoveSpeed = 5.0f;
	float m_minX = 950;
	float m_minY = 530;
	float m_maxX = 1550;
	float m_maxY = 1970;
	int m_mapWidth = 2400;
	int m_mapHeight = 2400;

	bool m_isDragging = false;
	sf::Vector2f m_dragStart;
	sf::Vector2f m_dragEnd;
	sf::RectangleShape m_selectionBox;

	// Particles variables
	float m_angleParticle;
	float m_speedParticle;
	float m_lifetimeParticle;
	float m_sizeParticle;
	sf::Color m_colorParticle;

	bool m_levelLoaded = false;
	bool m_exitGame;

	// Neural Network stuff
	int m_mousePosX;
	int m_mousePosY;
	int m_outputHeight = 64;
	int m_outputWidth = 64;
	float m_totalErrors = 0;
	float m_dotX;
	float m_dotY;
	const int TRAININGS_PER_FRAME = 1000;
	bool m_train = false;
	vector_2d m_errors;
	std::vector<std::vector<float>> m_neuralNetwork;
	std::vector<std::vector<std::vector<float>>> m_weights;
	std::array<int, 2> m_hiddenNeurons = m_neural_network.getHiddenNeurons();
	std::array<int, 3> m_biasNeurons = m_neural_network.getBiasNeurons();
	std::mt19937_64 m_random;
	sf::Text m_errorText;
	sf::Image m_outputImage;
	sf::Sprite m_outputSprite;
	sf::Texture m_outputTexture;

	// Win/Lose stuff
	bool m_showWinLosePanel = false;
	sf::Sprite m_panelBackgroundSprite;
	sf::Texture m_panelBackgroundTexture;
	sf::Text m_winLoseText;
	sf::Text m_playAgainText;
	sf::Text m_exitText;
	sf::RectangleShape m_playAgainButton;
	sf::RectangleShape m_exitButton;
	sf::Text m_playerStatsText;
	sf::Text m_enemyStatsText;
	sf::Text m_gameDurationText;
	int m_playerUnitStatCount = 0;
	int m_playerBuildingStatCount = 0;
	int m_enemyUnitStatCount = 0;
	int m_enemyBuildingStatCount = 0;
	sf::Clock m_gameDurationClock;
	sf::Time m_gameDurationTime;
	int m_minutes = 0;
	int m_seconds = 0;

};

#endif