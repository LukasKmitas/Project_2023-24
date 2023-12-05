#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include "Global.h"
#include "GameState.h"

class MainMenu
{
public:
    MainMenu();

	void update(sf::Time t_deltaTime);
	void render(sf::RenderWindow& m_window);
    void handleButtonClick(const sf::Vector2f& mousePosition, GameState& gameState);

private:

    void initButtons();

    GameState m_currentState = GameState::MainMenu;

    sf::Font m_font;
    sf::Text m_welcomeMessage;
    sf::Text m_playText;
    sf::Text m_editorText;
    sf::Text m_exitText;
    sf::RectangleShape m_playButton;
    sf::RectangleShape m_editorButton;
    sf::RectangleShape m_exitButton;

};
