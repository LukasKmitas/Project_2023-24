#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include "GameState.h"
#include "Global.h"

class LevelLoader
{
public:
	LevelLoader();
	~LevelLoader();

	void update(sf::Time t_deltaTime);
	void render(sf::RenderWindow& m_window);

	void goToMainMenu(sf::Vector2i mousePosition, GameState& m_gameState);

private:

	void initBackground();
	void initBackButton();

	GameState m_currentState;

	sf::RectangleShape m_toGoBackButton;
	sf::Texture m_buttonTexture;

	sf::Texture m_backgroundTexture;
	sf::Sprite m_backgroundSprite;

	sf::Font m_font;
	sf::Text m_toGoBackText;

};

