#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include "GameState.h"
#include "Global.h"
#include "LevelEditor.h"
#include <filesystem>

namespace fs = std::experimental::filesystem;

class LevelLoader
{
public:

	LevelLoader();
	~LevelLoader();

	void update(sf::Time t_deltaTime);
	void render(sf::RenderWindow& m_window);

	void initLevelSelectionButtons();

	void goToMainMenu(sf::Vector2i mousePosition, GameState& m_gameState);
	void handleLevelSelectionMouseInput(sf::Vector2i mousePosition);

private:

	LevelEditor m_levelEditor;

	void initBackground();
	void initBackButton();

	void loadLevel(const std::string& filename);

	std::vector<sf::RectangleShape> levelSelectionButtons;
	std::vector<std::string> levelFilenames;

	sf::RectangleShape m_toGoBackButton;
	sf::Texture m_buttonTexture;

	sf::Texture m_backgroundTexture;
	sf::Sprite m_backgroundSprite;

	sf::Font m_font;
	sf::Text m_toGoBackText;
	sf::Text levelSelectionButtonText;

	int selectedButtonIndex;

	float buttonWidth = 250.0f;
	float buttonHeight = 80.0f;

	bool levelLoaded = false;
};

