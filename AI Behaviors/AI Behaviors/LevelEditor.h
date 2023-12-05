#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include "Tile.h"
#include <fstream>
#include "Global.h"

class LevelEditor
{
public:
	LevelEditor();

	void update(sf::Time t_deltaTime);
	void render(sf::RenderWindow& m_window);
	void renderLoadedLevel(sf::RenderWindow& m_window);

	void saveLevelToFile(const std::string& filename);
	void loadLevelFromFile(const std::string& filename);

private:

	sf::View m_levelEditorView;

	void randomGenerateLevel();
	void initBar();

	std::vector<std::vector<Tile>> m_tiles;

	sf::RectangleShape m_backgroundForTiles;

	const int numRows = 50;
	const int numCols = 50;

};

