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
    void handleButtonClick(const sf::Vector2f& m_mousePosition, GameState& m_gameState);

private:

    void initButtons();
    void initBackgroundImage();

    void buttonTabAnimation(sf::RenderWindow& m_window, sf::Time t_deltaTime);
    void titleAnimation(sf::Time t_deltaTime);

    GameState m_currentState = GameState::MainMenu;

    sf::Font m_font;

    sf::Text m_titleText;
    sf::Text outlineText;
    sf::Text m_playText;
    sf::Text m_editorText;
    sf::Text m_loadLevelText;
    sf::Text m_exitText;

    sf::RectangleShape m_playButton;
    sf::RectangleShape m_editorButton;
    sf::RectangleShape m_loadLevelButton;
    sf::RectangleShape m_exitButton;
    sf::Texture m_buttonTexture;
    sf::Sprite m_buttonTabSprite;
    sf::Texture m_buttonTabTexture;
 
    sf::Texture m_backgroundTexture;
    sf::Sprite m_backgroundSprite;
    
    sf::Shader m_titleShader;
    sf::Shader m_backgroundShader;

    sf::Vector2f m_mousePos;
    sf::Time m_deltaTime;

    float m_titleAnimationTime = 0.0f;
    float m_titleMaxScale = 1.0f;
    float m_titleMinScale = 0.8f;
    float m_titleAnimationSpeed = 1.0f;

};
