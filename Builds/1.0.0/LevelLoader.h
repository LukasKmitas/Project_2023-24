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

	void goToMainMenu(sf::Vector2i m_mousePosition, GameState& m_gameState);
	void handleLevelSelectionMouseInput(sf::Vector2i m_mousePosition);

	std::vector<std::string> m_levelFilenames;

	int m_selectedButtonIndex;

private:

	LevelEditor m_levelEditor;

	void initBackground();
	void initBackButton();

	void loadLevel(const std::string& m_filename);

	std::vector<sf::RectangleShape> m_levelSelectionButtons;

	sf::RectangleShape m_toGoBackButton;
	sf::Texture m_buttonTexture;

	sf::Texture m_backgroundTexture;
	sf::Sprite m_backgroundSprite;

	sf::Font m_font;
	sf::Text m_toGoBackText;
	sf::Text m_levelSelectionButtonText;

	float m_buttonWidth = 250.0f;
	float m_buttonHeight = 80.0f;

	bool m_levelLoaded = false;
};

