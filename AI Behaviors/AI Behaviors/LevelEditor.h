#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include "Tile.h"
#include <fstream>
#include "Global.h"
#include "GameState.h"

class LevelEditor
{
public:
	LevelEditor();

	void update(sf::Time t_deltaTime, sf::RenderWindow& m_window);
	void render(sf::RenderWindow& m_window);
	void renderLoadedLevel(sf::RenderWindow& m_window);

	void handleMouseInput(sf::Vector2i m_mousePosition, GameState& m_gameState, sf::RenderWindow& m_window);
	void handleRotationInput(sf::Event event);

	void saveLevelToFile(const std::string& m_filename);
	void loadLevelFromFile(const std::string& m_filename);
	void loadLevelForLevelEditor();

	void animationForResources();
	void animationForWeed();

	void releaseDragRect();
	void resetFogOfWar();

	std::vector<std::vector<Tile>> m_tiles;

	const int numRows = 50;
	const int numCols = 50;

private:

	sf::View m_levelEditorView;

	void initGrid();
	void randomGenerateLevel();
	void initBar();
	void initbuttonsForToolEditor();
	void initButtonsForToolSet();
	void initBackButton();
	void initDragRectangle();

	void handleTileButtons(sf::Vector2f guiMousePosition, sf::Vector2f worldMousePosition, sf::Sprite buttons[], int& lastClickedIndex, int& selectedIndex);
	void handleTilePlacement(sf::Vector2f worldMousePosition, int lastClickedIndex, int selectedIndex);

	void placeTilesInDragRectangle();

	static const int numButtons = 12;

	int selectedButtonIndex = -1;
	int lastClickedButtonIndex = -1;
	int selectedTileX;
	int selectedTileY;
	float offset = 25.0f;
	bool isTileSelected = false;
	bool isButtonClicked = false;

	sf::Vector2f newMousePos;
	sf::Vector2f startMousePos;

	sf::RectangleShape m_backgroundForTilesTools;
	sf::RectangleShape m_toGoBackButton;
	sf::RectangleShape m_backgroundForBTF[4];
	sf::RectangleShape dragRectangle;

	sf::Sprite m_buttons[numButtons];

	sf::Font m_font;

	sf::Text m_toGoBackText;
	sf::Text m_walkableText;
	sf::Text m_WallText;
	sf::Text m_ResourceText;
	sf::Text m_MiscText;

	sf::Texture m_underWaterTexture;
	sf::Texture m_buttonTexture;
	sf::Texture m_buttonTexture1;

	sf::IntRect buttonAreaForWalkable[4] = 
	{
		{17, 1808, 16, 16},
		{33, 1808, 16, 16},
		{49, 1808, 16, 16},
		{49, 1792, 16, 16}
	};
	sf::IntRect buttonAreaForWalls[5] = 
	{
		{49, 1695, 16, 16},
		{242, 1808, 16, 16},
		{257, 1808, 16, 16},
		{226, 1776, 16, 16},
		{258, 1792, 16, 16}
	};
	sf::IntRect buttonAreaForResources = { 971, 1798, 16, 16 };
	sf::IntRect buttonAreaForMiscs[2] =
	{
		{971, 1764, 16, 16},
		{854, 1776, 16, 16}
	};

	//animation stuff
	const int numTileFrames = 4;
	int currentResourceTileAnimationFrame = 0;
	sf::Clock tileResourceAnimationClock;
	sf::IntRect tileResourceAnimationFrames[4] = 
	{
		{971, 1798, 16, 16},
		{988, 1798, 16, 16},
		{1005, 1798, 16, 16},
		{1022, 1798, 16, 16}
	};
	sf::Clock tileAnimationClock;
	int currentTileWeedAnimationFrame = 0;
	sf::IntRect tileWeedAnimationFrames[4] =
	{
		{971, 1764, 16, 16},
		{988, 1764, 16, 16},
		{1005, 1764, 16, 16},
		{1022, 1764, 16, 16}
	};

	sf::Sprite buttonSelectedSprite;
	sf::Texture buttonSelectedTexture;
	sf::Clock buttonSelectedanimationClock;
	float animationSpeed = 0.2f;
	int currentAnimationFrame = 0;
};

