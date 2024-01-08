#include "MainMenu.h"

MainMenu::MainMenu()
{
	initButtons();
}

void MainMenu::update(sf::Time t_deltaTime)
{
   
}

void MainMenu::render(sf::RenderWindow& m_window)
{
    m_window.draw(m_playButton);
    m_window.draw(m_editorButton);
    m_window.draw(m_exitButton);
    m_window.draw(m_playText);
    m_window.draw(m_editorText);
    m_window.draw(m_exitText);
    m_window.draw(m_welcomeMessage);
}

void MainMenu::handleButtonClick(const sf::Vector2f& m_mousePosition, GameState& m_gameState)
{
    if (m_playButton.getGlobalBounds().contains(m_mousePosition))
    {
        m_gameState = GameState::PlayGame;
    }
    else if (m_editorButton.getGlobalBounds().contains(m_mousePosition))
    {
        m_gameState = GameState::LevelEditor;
    }
    else if (m_exitButton.getGlobalBounds().contains(m_mousePosition))
    {
        m_gameState = GameState::Exit;
    }
}

void MainMenu::initButtons()
{
    if (!m_font.loadFromFile("Assets\\Fonts\\ManicSea_19.ttf"))
    {
        std::cout << "Error - problem loading font in Main Menu" << std::endl;
    }

    // Welcome message
    m_welcomeMessage.setFont(m_font);
    m_welcomeMessage.setString("Gills & Glory");
    m_welcomeMessage.setStyle(sf::Text::Italic | sf::Text::Bold);
    m_welcomeMessage.setPosition(Global::S_WIDTH / 2, Global::S_HEIGHT / 10);
    m_welcomeMessage.setCharacterSize(80U);
    m_welcomeMessage.setOutlineColor(sf::Color::Blue);
    m_welcomeMessage.setFillColor(sf::Color::White);
    m_welcomeMessage.setOutlineThickness(1.0f);
    m_welcomeMessage.setOrigin(m_welcomeMessage.getGlobalBounds().width / 2, m_welcomeMessage.getGlobalBounds().height / 2);
    
    // Play Button
    m_playButton.setSize(sf::Vector2f(120, 50));
    m_playButton.setFillColor(sf::Color::White);
    m_playButton.setOrigin(m_playButton.getSize().x / 2, m_playButton.getSize().y / 2);
    m_playButton.setPosition(Global::S_WIDTH / 2,Global::S_HEIGHT / 2 - 100);
    m_playText.setFont(m_font);
    m_playText.setString("Play");
    m_playText.setCharacterSize(30);
    m_playText.setFillColor(sf::Color::Black);
    m_playText.setPosition(m_playButton.getPosition());
    m_playText.setOrigin(m_playText.getGlobalBounds().width / 2, m_playText.getGlobalBounds().height / 2);

    // Level Editor Button
    m_editorButton.setSize(sf::Vector2f(200, 50));
    m_editorButton.setFillColor(sf::Color::White);
    m_editorButton.setOrigin(m_editorButton.getSize().x / 2, m_editorButton.getSize().y / 2);
    m_editorButton.setPosition(Global::S_WIDTH / 2, Global::S_HEIGHT / 2);
    m_editorText.setFont(m_font);
    m_editorText.setString("Level Editor");
    m_editorText.setCharacterSize(30);
    m_editorText.setFillColor(sf::Color::Black);
    m_editorText.setPosition(m_editorButton.getPosition());
    m_editorText.setOrigin(m_editorText.getGlobalBounds().width / 2, m_editorText.getGlobalBounds().height / 2);

    // Exit Button
    m_exitButton.setSize(sf::Vector2f(100, 50));
    m_exitButton.setFillColor(sf::Color::White);
    m_exitButton.setOrigin(m_exitButton.getSize().x / 2, m_exitButton.getSize().y / 2);
    m_exitButton.setPosition(Global::S_WIDTH / 2, Global::S_HEIGHT / 2 + 100);
    m_exitText.setFont(m_font);
    m_exitText.setString("Exit");
    m_exitText.setCharacterSize(30);
    m_exitText.setFillColor(sf::Color::Black);
    m_exitText.setPosition(m_exitButton.getPosition());
    m_exitText.setOrigin(m_exitText.getGlobalBounds().width / 2, m_exitText.getGlobalBounds().height / 2);
}
