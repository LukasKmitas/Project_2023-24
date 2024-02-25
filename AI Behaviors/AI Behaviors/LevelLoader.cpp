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

void LevelLoader::update(sf::Time t_deltaTime)
{
	loadLevel(levelFilenames[selectedButtonIndex]);
}

void LevelLoader::render(sf::RenderWindow& m_window)
{
	m_window.draw(m_backgroundSprite);
	m_window.draw(m_toGoBackButton);
	m_window.draw(m_toGoBackText);

	for (const auto& button : levelSelectionButtons)
	{
		m_window.draw(button);
	}
	for (size_t i = 0; i < levelSelectionButtons.size(); ++i)
	{
		m_window.draw(levelSelectionButtons[i]);
		levelSelectionButtonText.setString(levelFilenames[i]);
		float textX = levelSelectionButtons[i].getPosition().x + (buttonWidth - levelSelectionButtonText.getGlobalBounds().width) / 2.0f;
		float textY = levelSelectionButtons[i].getPosition().y + (buttonHeight - levelSelectionButtonText.getGlobalBounds().height) / 2.0f;
		levelSelectionButtonText.setPosition(textX, textY);
		m_window.draw(levelSelectionButtonText);
	}
}

void LevelLoader::initLevelSelectionButtons()
{
	levelSelectionButtons.clear();
	levelFilenames.clear();

	const std::string saveFilesPath = "Assets\\SaveFiles\\";

	float buttonSpacing = 30.0f;

	float startX = (Global::S_WIDTH - buttonWidth) / 2.0f;
	float startY = (Global::S_HEIGHT - (buttonHeight + buttonSpacing) * levelSelectionButtons.size()) / 2.0f - 200;

	size_t i = 0;
	for (const auto& entry : std::filesystem::directory_iterator(saveFilesPath))
	{
		if (entry.is_regular_file() && entry.path().extension() == ".txt")
		{
			// Extract filename (without extension)
			std::string filename = entry.path().filename().stem().string();
			levelFilenames.push_back(filename);

			sf::RectangleShape button(sf::Vector2f(buttonWidth, buttonHeight));
			button.setTexture(&m_buttonTexture);
			if (i == 0)
			{
				button.setFillColor(sf::Color(255, 80, 80, 255));
			}
			else
			{
				button.setFillColor(sf::Color(100, 100, 150, 255));
			}

			button.setPosition(startX, startY + static_cast<float>(i) * (buttonHeight + buttonSpacing));

			levelSelectionButtonText.setFont(m_font);
			levelSelectionButtonText.setString(filename);
			levelSelectionButtonText.setCharacterSize(20);
			levelSelectionButtonText.setFillColor(sf::Color::White);

			float textX = startX + (buttonWidth - levelSelectionButtonText.getGlobalBounds().width) / 2.0f;
			float textY = startY + static_cast<float>(i) * (buttonHeight + buttonSpacing) + (buttonHeight - levelSelectionButtonText.getGlobalBounds().height) / 2.0f;
			levelSelectionButtonText.setPosition(textX, textY);
			levelSelectionButtons.push_back(button);
			++i;
		}
	}
	selectedButtonIndex = 0;
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

void LevelLoader::handleLevelSelectionMouseInput(sf::Vector2i mousePosition)
{
	for (size_t i = 0; i < levelSelectionButtons.size(); ++i)
	{
		if (levelSelectionButtons[i].getGlobalBounds().contains(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y)))
		{
			levelSelectionButtons[selectedButtonIndex].setFillColor(sf::Color(100, 100, 150, 255));
			selectedButtonIndex = static_cast<int>(i);
			levelSelectionButtons[selectedButtonIndex].setFillColor(sf::Color(255, 80, 80, 255));
			levelSelectionButtons[i].setTexture(&m_buttonTexture);
			levelLoaded = false;
			loadLevel(levelFilenames[selectedButtonIndex]);
			break;
		}
	}
}

void LevelLoader::loadLevel(const std::string& filename)
{
	if (!levelLoaded)
	{
		m_levelEditor.loadLevelFromFile("Assets\\SaveFiles\\" + filename + ".txt");
		levelLoaded = true;
	}
}
