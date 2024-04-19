#include "LevelLoader.h"

LevelLoader::LevelLoader()
{
	if (!m_font.loadFromFile("Assets\\Fonts\\ManicSea_19.ttf"))
	{
		std::cout << "Error - problem loading font in Level Editor" << std::endl;
	}
	initBackButton();
	initBackground();
	initLevelSelectionButtons();
}

LevelLoader::~LevelLoader()
{
}

/// <summary>
/// Updates the level loader to load the level
/// </summary>
/// <param name="t_deltaTime"></param>
void LevelLoader::update(sf::Time t_deltaTime)
{
	loadLevel(m_levelFilenames[m_selectedButtonIndex]);
}

/// <summary>
/// Renders everything in level loader 
/// </summary>
/// <param name="m_window"></param>
void LevelLoader::render(sf::RenderWindow& m_window)
{
	m_window.draw(m_backgroundSprite);
	m_window.draw(m_toGoBackButton);
	m_window.draw(m_toGoBackText);

	for (const auto& button : m_levelSelectionButtons)
	{
		m_window.draw(button);
	}
	for (size_t i = 0; i < m_levelSelectionButtons.size(); ++i)
	{
		m_window.draw(m_levelSelectionButtons[i]);
		m_levelSelectionButtonText.setString(m_levelFilenames[i]);
		float textX = m_levelSelectionButtons[i].getPosition().x + (m_buttonWidth - m_levelSelectionButtonText.getGlobalBounds().width) / 2.0f;
		float textY = m_levelSelectionButtons[i].getPosition().y + (m_buttonHeight - m_levelSelectionButtonText.getGlobalBounds().height) / 2.0f;
		m_levelSelectionButtonText.setPosition(textX, textY);
		m_window.draw(m_levelSelectionButtonText);
	}
}

/// <summary>
/// Initializes buttons to select what level
/// </summary>
void LevelLoader::initLevelSelectionButtons()
{
	m_levelSelectionButtons.clear();
	m_levelFilenames.clear();

	const std::string saveFilesPath = "Assets\\SaveFiles\\";

	float buttonSpacing = 30.0f;
	float startX = (Global::S_WIDTH - m_buttonWidth) / 2.0f;
	float startY = (Global::S_HEIGHT - (m_buttonHeight + buttonSpacing) * m_levelSelectionButtons.size()) / 2.0f - 200;

	size_t i = 0;
	for (const auto& entry : std::filesystem::directory_iterator(saveFilesPath))
	{
		if (entry.is_regular_file() && entry.path().extension() == ".txt")
		{
			// Extract filename (without extension)
			std::string filename = entry.path().filename().stem().string();
			m_levelFilenames.push_back(filename);

			sf::RectangleShape button(sf::Vector2f(m_buttonWidth, m_buttonHeight));
			button.setTexture(&m_buttonTexture);
			if (i == 0)
			{
				button.setFillColor(sf::Color(255, 80, 80, 255));
			}
			else
			{
				button.setFillColor(sf::Color(100, 100, 150, 255));
			}

			button.setPosition(startX, startY + static_cast<float>(i) * (m_buttonHeight + buttonSpacing));

			m_levelSelectionButtonText.setFont(m_font);
			m_levelSelectionButtonText.setString(filename);
			m_levelSelectionButtonText.setCharacterSize(20);
			m_levelSelectionButtonText.setFillColor(sf::Color::White);

			float textX = startX + (m_buttonWidth - m_levelSelectionButtonText.getGlobalBounds().width) / 2.0f;
			float textY = startY + static_cast<float>(i) * (m_buttonHeight + buttonSpacing) + (m_buttonHeight - m_levelSelectionButtonText.getGlobalBounds().height) / 2.0f;
			m_levelSelectionButtonText.setPosition(textX, textY);
			m_levelSelectionButtons.push_back(button);
			++i;
		}
	}
	m_selectedButtonIndex = 0;
}

/// <summary>
/// To go back to main menu screen
/// </summary>
/// <param name="mousePosition"></param>
/// <param name="m_gameState"></param>
void LevelLoader::goToMainMenu(sf::Vector2i m_mousePosition, GameState& m_gameState)
{
	if (m_toGoBackButton.getGlobalBounds().contains(m_mousePosition.x, m_mousePosition.y))
	{
		m_gameState = GameState::MainMenu;
	}
}

/// <summary>
/// Initializes background
/// </summary>
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

/// <summary>
/// Initializes back button 
/// </summary>
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

/// <summary>
/// handles the mouse press for the buttons to load the level
/// </summary>
/// <param name="mousePosition"></param>
void LevelLoader::handleLevelSelectionMouseInput(sf::Vector2i m_mousePosition)
{
	for (size_t i = 0; i < m_levelSelectionButtons.size(); ++i)
	{
		if (m_levelSelectionButtons[i].getGlobalBounds().contains(static_cast<float>(m_mousePosition.x), static_cast<float>(m_mousePosition.y)))
		{
			m_levelSelectionButtons[m_selectedButtonIndex].setFillColor(sf::Color(100, 100, 150, 255));
			m_selectedButtonIndex = static_cast<int>(i);
			m_levelSelectionButtons[m_selectedButtonIndex].setFillColor(sf::Color(255, 80, 80, 255));
			m_levelSelectionButtons[i].setTexture(&m_buttonTexture);
			m_levelLoaded = false;
			loadLevel(m_levelFilenames[m_selectedButtonIndex]);
			break;
		}
	}
}

/// <summary>
/// Loads the level
/// </summary>
/// <param name="filename"></param>
void LevelLoader::loadLevel(const std::string& m_filename)
{
	if (!m_levelLoaded)
	{
		m_levelEditor.loadLevelFromFile("Assets\\SaveFiles\\" + m_filename + ".txt");
		m_levelLoaded = true;
	}
}
