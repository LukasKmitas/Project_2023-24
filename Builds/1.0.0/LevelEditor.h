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
	void handleRotationInput(sf::Event m_event);

	void saveLevelToFile(const std::string& m_filename);
	void loadLevelFromFile(const std::string& m_filename);
	void loadLevelForLevelEditor();

	void animationForResources();
	void animationForWeed();

	void releaseDragRect();
	void resetFogOfWar();

	std::vector<std::vector<Tile>> m_tiles;

	const int m_numRows = 50;
	const int m_numCols = 50;

private:

	sf::View m_levelEditorView;

	void initGrid();
	void randomGenerateLevel();
	void initBar();
	void initbuttonsForToolEditor();
	void initButtonsForToolSet();
	void initBackButton();
	void initDragRectangle();

	void handleTileButtons(sf::Vector2f m_guiMousePosition, sf::Vector2f m_worldMousePosition, sf::Sprite m_buttons[], int& m_lastClickedIndex, int& m_selectedIndex);
	void handleTilePlacement(sf::Vector2f m_worldMousePosition, int m_selectedIndex);

	void placeTilesInDragRectangle();

	static const int m_numButtons = 12;

	int m_selectedButtonIndex = -1;
	int m_lastClickedButtonIndex = -1;
	int m_selectedTileX;
	int m_selectedTileY;
	float m_offset = 25.0f;
	bool m_isTileSelected = false;
	bool m_isButtonClicked = false;

	sf::Vector2f m_newMousePos;
	sf::Vector2f m_startMousePos;

	sf::RectangleShape m_backgroundForTilesTools;
	sf::RectangleShape m_toGoBackButton;
	sf::RectangleShape m_backgroundForBTF[4];
	sf::RectangleShape m_dragRectangle;

	sf::Sprite m_buttons[m_numButtons];

	sf::Font m_font;

	sf::Text m_toGoBackText;
	sf::Text m_walkableText;
	sf::Text m_WallText;
	sf::Text m_ResourceText;
	sf::Text m_MiscText;

	sf::Texture m_underWaterTexture;
	sf::Texture m_buttonTexture;
	sf::Texture m_buttonTexture1;

	sf::IntRect m_buttonAreaForWalkable[4] = 
	{
		{17, 1808, 16, 16},
		{33, 1808, 16, 16},
		{49, 1808, 16, 16},
		{49, 1792, 16, 16}
	};
	sf::IntRect m_buttonAreaForWalls[5] = 
	{
		{49, 1695, 16, 16},
		{242, 1808, 16, 16},
		{257, 1808, 16, 16},
		{226, 1776, 16, 16},
		{258, 1792, 16, 16}
	};
	sf::IntRect m_buttonAreaForResources = { 971, 1798, 16, 16 };
	sf::IntRect m_buttonAreaForMiscs[2] =
	{
		{971, 1764, 16, 16},
		{854, 1776, 16, 16}
	};

	//animation stuff
	const int m_numTileFrames = 4;
	int m_currentResourceTileAnimationFrame = 0;
	sf::Clock m_tileResourceAnimationClock;
	sf::IntRect m_tileResourceAnimationFrames[4] = 
	{
		{971, 1798, 16, 16},
		{988, 1798, 16, 16},
		{1005, 1798, 16, 16},
		{1022, 1798, 16, 16}
	};
	sf::Clock m_tileAnimationClock;
	int m_currentTileWeedAnimationFrame = 0;
	sf::IntRect m_tileWeedAnimationFrames[4] =
	{
		{971, 1764, 16, 16},
		{988, 1764, 16, 16},
		{1005, 1764, 16, 16},
		{1022, 1764, 16, 16}
	};

	sf::Sprite m_buttonSelectedSprite;
	sf::Texture m_buttonSelectedTexture;
	sf::Clock m_buttonSelectedAnimationClock;
	int m_currentAnimationFrame = 0;
	float m_animationSpeed = 0.2f;
};

