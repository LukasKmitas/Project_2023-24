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

	void handleMouseInput(sf::Vector2i mousePosition, GameState& gameState, sf::RenderWindow& m_window);

	void saveLevelToFile(const std::string& filename);
	void loadLevelFromFile(const std::string& filename);

	std::vector<std::vector<Tile>> m_tiles;

	const int numRows = 50;
	const int numCols = 50;
private:

	sf::View m_levelEditorView;

	void randomGenerateLevel();
	void initBar();
	void initButtons();

	//std::vector<std::vector<Tile>> m_tiles;

	sf::RectangleShape m_backgroundForTiles;
	sf::RectangleShape m_toGoBackButton;

	sf::Font m_font;
	sf::Text m_toGoBackText;

};

