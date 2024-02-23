#include "LevelLoader.h"

LevelLoader::LevelLoader()
{
	if (!m_font.loadFromFile("Assets\\Fonts\\ManicSea_19.ttf"))
	{
		std::cout << "Error - problem loading font in Level Editor" << std::endl;
	}
	initBackButton();
	initBackground();
}

LevelLoader::~LevelLoader()
{
}

void LevelLoader::update(sf::Time t_deltaTime)
{
}

void LevelLoader::render(sf::RenderWindow& m_window)
{
	m_window.draw(m_backgroundSprite);
	m_window.draw(m_toGoBackButton);
	m_window.draw(m_toGoBackText);
}

void LevelLoader::goToMainMenu(sf::Vector2i mousePosition, GameState& m_gameState)
{
	if (m_toGoBackButton.getGlobalBounds().contains(mousePosition.x, mousePosition.y))
	{
		m_gameState = GameState::MainMenu;
	}
}

void LevelLoader::initBackground()
{
	if (!m_backgroundTexture.loadFromFile("Assets\\Images\\Menu\\Underwater BG Blank.png"))
	{
		std::cout << "Error - Failed to load background image" << std::endl;
	}

	m_backgroundSprite.setTexture(m_backgroundTexture);

	m_backgroundSprite.setScale(static_cast<float>(Global::S_WIDTH) / m_backgroundSprite.getGlobalBounds().width,
		static_cast<float>(Global::S_HEIGHT) / m_backgroundSprite.getGlobalBounds().height);
}

void LevelLoader::initBackButton()
{
	if (!m_buttonTexture.loadFromFile("Assets\\Images\\GUI\\buttonStock2.png"))
	{
		std::cout << "Error - loading button texture" << std::endl;
	}

	m_toGoBackButton.setSize(sf::Vector2f(100, 50));
	m_toGoBackButton.setPosition(80, 50);
	m_toGoBackButton.setOrigin(m_toGoBackButton.getGlobalBounds().width / 2, m_toGoBackButton.getGlobalBounds().height / 2);
	m_toGoBackButton.setTexture(&m_buttonTexture);

	m_toGoBackText.setFont(m_font);
	m_toGoBackText.setString("Back");
	m_toGoBackText.setPosition(m_toGoBackButton.getPosition().x, m_toGoBackButton.getPosition().y - 5);
	m_toGoBackText.setCharacterSize(25U);
	m_toGoBackText.setFillColor(sf::Color(225, 245, 255));
	m_toGoBackText.setOutlineColor(sf::Color::Blue);
	m_toGoBackText.setOutlineThickness(1.0f);
	m_toGoBackText.setOrigin(m_toGoBackText.getGlobalBounds().width / 2, m_toGoBackText.getGlobalBounds().height / 2);
}