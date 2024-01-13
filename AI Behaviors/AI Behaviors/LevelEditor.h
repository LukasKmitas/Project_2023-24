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

	void update(sf::Time t_deltaTime);
	void render(sf::RenderWindow& m_window);
	void renderLoadedLevel(sf::RenderWindow& m_window);

	void handleMouseInput(sf::Vector2i m_mousePosition, GameState& m_gameState, sf::RenderWindow& m_window);
	void handleRotationInput(sf::Event event);

	void saveLevelToFile(const std::string& m_filename);
	void loadLevelFromFile(const std::string& m_filename);

	void animationForResources();

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

	void handleTileButtons(sf::Vector2f guiMousePosition, sf::Vector2f worldMousePosition, sf::Sprite buttons[], int& lastClickedIndex, int& selectedIndex);
	void handleTilePlacement(sf::Vector2f worldMousePosition, int lastClickedIndex, int selectedIndex);

	static const int numButtons = 12;

	int selectedButtonIndex = -1;
	int lastClickedButtonIndex = -1;
	int selectedTileX;
	int selectedTileY;
	float offset = 25.0f;
	bool isTileSelected = false;
	bool isOffsetApplied = false;

	sf::RectangleShape m_backgroundForTilesTools;
	sf::RectangleShape m_toGoBackButton;
	sf::RectangleShape m_backgroundForBTF[4];

	sf::Sprite m_buttons[numButtons];

	sf::Font m_font;

	sf::Text m_toGoBackText;
	sf::Text m_walkableText;
	sf::Text m_WallText;
	sf::Text m_ResourceText;
	sf::Text m_MiscText;

	sf::Texture m_underWaterTexture;

	sf::Clock tileAnimationClock;
	int currentTileAnimationFrame = 0;
	const int numTileFrames = 4;
	sf::IntRect tileAnimationFrames[4] = {
		{971, 1798, 16, 16},
		{988, 1798, 16, 16},
		{1005, 1798, 16, 16},
		{1022, 1798, 16, 16}
	};

};

