#include "Game.h"
#include <iostream>


/// <summary>
/// default constructor
/// </summary>
Game::Game() :
	m_window{ sf::VideoMode{ Global::S_WIDTH, Global::S_HEIGHT, 32U }, "Gills & Glory" },
	m_exitGame{false}
{
	gameView.setSize(sf::Vector2f(Global::S_WIDTH, Global::S_HEIGHT));
	gameView.setCenter(Global::S_WIDTH / 2, Global::S_HEIGHT / 2);
	m_window.setView(gameView);

	if (!m_font.loadFromFile("Assets\\Fonts\\ManicSea_19.ttf"))
	{
		std::cout << "Error - loading font" << std::endl;
	}

	createBase();
	initBackgroundImage();
	initBackButton();
	initLevelSelectionButtons();
	m_levelEditor.resetFogOfWar();
}

/// <summary>
/// default destructor we didn't dynamically allocate anything
/// so we don't need to free it, but mthod needs to be here
/// </summary>
Game::~Game()
{
}


/// <summary>
/// main game loop
/// update 60 times per second,
/// process update as often as possible and at least 60 times per second
/// </summary>
void Game::run()
{	
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;
	const float fps{ 60.0f };
	sf::Time timePerFrame = sf::seconds(1.0f / fps);
	while (m_window.isOpen())
	{
		processEvents();
		timeSinceLastUpdate += clock.restart();
		while (timeSinceLastUpdate > timePerFrame)
		{
			timeSinceLastUpdate -= timePerFrame;
			processEvents(); // at least 60 fps
			update(timePerFrame); //60 fps
		}
		render();
	}
}
/// <summary>
/// handle user and system events/ input
/// get key presses/ mouse moves
/// </summary>
void Game::processEvents()
{
	sf::Event newEvent;
	while (m_window.pollEvent(newEvent))
	{
		if ( sf::Event::Closed == newEvent.type)
		{
			m_exitGame = true;
		}
		if (sf::Event::KeyPressed == newEvent.type) //user pressed a key
		{
			processKeys(newEvent);
			m_levelEditor.handleRotationInput(newEvent);
		}
		if (sf::Event::MouseButtonPressed == newEvent.type) // Check for mouse button press.
		{
			if (newEvent.mouseButton.button == sf::Mouse::Left) // Check for left mouse button.
			{
				sf::Vector2i mousePosition = sf::Mouse::getPosition(m_window);
				sf::Vector2f mousePos = m_window.mapPixelToCoords(sf::Mouse::getPosition(m_window));
				switch (m_currentState)
				{
				case GameState::MainMenu:
					m_menu.handleButtonClick(m_window.mapPixelToCoords(mousePosition), m_currentState);
					break;
				case GameState::PlayGame:
					m_gui.handleMouseClick(mousePosition, m_window);
					if (m_selectedUnit) 
					{
						m_selectedUnit->moveTo(mousePos);
					}
					else 
					{
						selectUnitAt(mousePos);
					}
					break;
				case GameState::LevelEditor:
					m_levelEditor.handleMouseInput(mousePosition, m_currentState, m_window);
					break;
				case GameState::LevelSelection:
					if (m_toGoBackButton.getGlobalBounds().contains(mousePosition.x, mousePosition.y))
					{
						goToMainMenu();
					}
					handleLevelSelectionMouseInput(mousePosition);
					break;
				case GameState::Exit:
					break;
				default:
					break;
				}
			}
		}
		if (sf::Event::MouseButtonReleased == newEvent.type)
		{
			switch (m_currentState)
			{
			case GameState::MainMenu:
				break;
			case GameState::PlayGame:
				break;
			case GameState::LevelEditor:
				if (newEvent.mouseButton.button == sf::Mouse::Left)
				{
					m_levelEditor.releaseDragRect();
				}
				break;
			case GameState::LevelSelection:
				break;
			case GameState::Exit:
				break;
			default:
				break;
			}
		}
	}
}

/// <summary>
/// deal with key presses from the user
/// </summary>
/// <param name="t_event">key press event</param>
void Game::processKeys(sf::Event t_event)
{
	cameraVelocity = sf::Vector2f(0.0f, 0.0f);
	float speed = (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) ? 2.0f * viewMoveSpeed : viewMoveSpeed;

	switch (m_currentState)
	{
	case GameState::MainMenu:
		if (sf::Keyboard::Escape == t_event.key.code)
		{
			m_exitGame = true;
		}
		break;
	case GameState::PlayGame:
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		{
			cameraVelocity.y -= speed;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		{
			cameraVelocity.x -= speed;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		{
			cameraVelocity.y += speed;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		{
			cameraVelocity.x += speed;
		}
		if (sf::Keyboard::Escape == t_event.key.code)
		{
			m_selectedUnit = nullptr;
			for (Unit* unit : units)
			{
				unit->setSelected(false);
			}
		}
		break;
	case GameState::LevelEditor:
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1))
		{
			saveLevel();
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2))
		{
			m_levelEditor.loadLevelForLevelEditor();
		}
		break;
	case GameState::LevelSelection:
		break;
	case GameState::Exit:
		break;
	default:
		break;
	}
}

/// <summary>
/// Update the game world
/// </summary>
/// <param name="t_deltaTime">time interval per frame</param>
void Game::update(sf::Time t_deltaTime)
{
	if (m_exitGame)
	{
		m_window.close();
	}
	sf::Vector2i mousePosition = sf::Mouse::getPosition(m_window);
	switch (m_currentState)
	{
	case GameState::MainMenu:
		if (m_currentState != GameState::LevelEditor && m_previousState == GameState::LevelEditor) 
		{
			resetView();
		}
		break;
	case GameState::PlayGame:
		loadLevel(levelFilenames[selectedButtonIndex]);
		updateView();
		createBuilding(m_window);
		m_gui.update(t_deltaTime);
		for (Building* building : placedBuildings)
		{
			building->update(t_deltaTime);
		}
		m_gui.handleBuildingPlacement(mousePosition, m_window);
		m_levelEditor.animationForResources();
		updateFogOfWarBasedOnBuildings(placedBuildings);
		createUnit(m_window);
		for (Unit* unit : units) 
		{
			unit->update(t_deltaTime);
		}
		break;
	case GameState::LevelEditor:
		m_previousState = GameState::LevelEditor;
		updateView();
		m_levelEditor.update(t_deltaTime, m_window);
		break;
	case GameState::Exit:
		m_exitGame = true;
		break;
	default:
		break;
	}
}

/// <summary>
/// draw the frame and then switch buffers
/// </summary>
void Game::render()
{
	m_window.clear(sf::Color::Black);
	switch (m_currentState)
	{
	case GameState::MainMenu:
		m_window.draw(m_backGroundSprite);
		m_menu.render(m_window);
		break;
	case GameState::PlayGame:
		m_levelEditor.renderLoadedLevel(m_window);
		for (Building* building : placedBuildings)
		{
			building->render(m_window);
		}
		for (Unit* unit : units)
		{
			unit->render(m_window);
		}
		m_gui.render(m_window);
		m_window.setView(gameView);
		break;
	case GameState::LevelEditor:
		m_levelEditor.render(m_window);
		m_window.setView(gameView);
		break;
	case GameState::LevelSelection:
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
		break;
	case GameState::Exit:
		break;
	default:
		break;
	}

	m_window.display();
}

/// <summary>
/// Update the screen view position
/// </summary>
void Game::updateView()
{
	sf::Vector2f viewCenter = gameView.getCenter();
	sf::Vector2f mousePosition = sf::Vector2f(sf::Mouse::getPosition(m_window));
	switch (m_currentState)
	{
	case GameState::PlayGame:
		minX = 950;
		minY = 530;
		maxX = 1550;
		maxY = 1970;
		if (mousePosition.x < Global::S_WIDTH * 0.01 && viewCenter.x - viewMoveSpeed > minX)
		{
			viewCenter.x -= viewMoveSpeed;
		}
		if (mousePosition.x > Global::S_WIDTH * 0.99 && viewCenter.x + viewMoveSpeed < maxX)
		{
			viewCenter.x += viewMoveSpeed;
		}
		if (mousePosition.y < Global::S_HEIGHT * 0.01 && viewCenter.y - viewMoveSpeed > minY)
		{
			viewCenter.y -= viewMoveSpeed;
		}
		if (mousePosition.y > Global::S_HEIGHT * 0.99 && viewCenter.y + viewMoveSpeed < maxY)
		{
			viewCenter.y += viewMoveSpeed;
		}
		break;
	case GameState::LevelEditor:
		minX = 500;
		minY = 200;
		maxX = 1900;
		maxY = 2300;
		if (mousePosition.x < Global::S_WIDTH * 0.01 && viewCenter.x - viewMoveSpeed > minX)
		{
			viewCenter.x -= viewMoveSpeed;
		}
		if (mousePosition.x > Global::S_WIDTH * 0.99 && viewCenter.x + viewMoveSpeed < maxX)
		{
			viewCenter.x += viewMoveSpeed;
		}
		if (mousePosition.y < Global::S_HEIGHT * 0.01 && viewCenter.y - viewMoveSpeed > minY)
		{
			viewCenter.y -= viewMoveSpeed;
		}
		if (mousePosition.y > Global::S_HEIGHT * 0.99 && viewCenter.y + viewMoveSpeed < maxY)
		{
			viewCenter.y += viewMoveSpeed;
		}
		break;
	default:
		break;
	}
	gameView.setCenter(viewCenter);
}

void Game::createBuilding(sf::RenderWindow& window)
{
	if (m_gui.m_confirmed)
	{
		sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
		sf::Vector2f worldMousePosition = window.mapPixelToCoords(mousePosition);
		if (m_selectedBuildingType == BuildingType::Refinery)
		{
			Refinery* newRefinery = new Refinery();
			newRefinery->setPosition(worldMousePosition);
			placedBuildings.push_back(newRefinery);
			std::cout << "Refinery Created" << std::endl;
		}
		else if (m_selectedBuildingType == BuildingType::Barracks)
		{
			Barracks* newBarracks = new Barracks();
			newBarracks->setPosition(worldMousePosition);
			placedBuildings.push_back(newBarracks);
			std::cout << "Barracks Created" << std::endl;
		}
		else if (m_selectedBuildingType == BuildingType::WarFactory)
		{
			WarFactory* newVehicle = new WarFactory();
			newVehicle->setPosition(worldMousePosition);
			placedBuildings.push_back(newVehicle);
			std::cout << "Vehicle Created" << std::endl;
		}
		else if (m_selectedBuildingType == BuildingType::AirCraft)
		{
			AirCraft* newAirCraft = new AirCraft();
			newAirCraft->setPosition(worldMousePosition);
			placedBuildings.push_back(newAirCraft);
			std::cout << "AirCraft Created" << std::endl;
		}
		m_gui.m_confirmBuildingPlacement = false;
		m_gui.m_confirmed = false;
		m_selectedBuildingType = BuildingType::Headquarters;
	}
}

void Game::createBase()
{
	Headquarters* newHeadquarters = new Headquarters();
	newHeadquarters->setPosition(sf::Vector2f(800.0f,200.0f));
	placedBuildings.push_back(newHeadquarters);
	std::cout << "Base Initilised" << std::endl;
}

void Game::saveLevel()
{
	std::string filename;
	std::cout << "Enter a filename to save the level: ";
	std::cin >> filename;

	if (filename.empty())
	{
		std::cout << "Invalid filename. Level not saved." << std::endl;
		return;
	}

	if (filename.find(".txt") == std::string::npos)
	{
		filename += ".txt";
	}

	m_levelEditor.saveLevelToFile("Assets\\SaveFiles\\" + filename);
}

/// <summary>
/// Resets the view back to normal
/// </summary>
void Game::resetView() 
{
	gameView.setSize(sf::Vector2f(Global::S_WIDTH, Global::S_HEIGHT));
	gameView.setCenter(Global::S_WIDTH / 2, Global::S_HEIGHT / 2);
	m_window.setView(gameView);
	initLevelSelectionButtons();
}

void Game::goToMainMenu()
{
	m_currentState = GameState::MainMenu;
}

void Game::initLevelSelectionButtons()
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

			if (i == 0)
			{
				button.setFillColor(sf::Color::Red);
				button.setOutlineColor(sf::Color::White);
				button.setOutlineThickness(2.0f);
			}
			else
			{
				button.setFillColor(sf::Color::Blue);
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

void Game::initBackButton()
{
	m_toGoBackButton.setFillColor(sf::Color(0, 200, 200));
	m_toGoBackButton.setSize(sf::Vector2f(100, 50));
	m_toGoBackButton.setPosition(80, 50);
	m_toGoBackButton.setOrigin(m_toGoBackButton.getGlobalBounds().width / 2, m_toGoBackButton.getGlobalBounds().height / 2);

	m_toGoBackText.setFont(m_font);
	m_toGoBackText.setString("Back");
	m_toGoBackText.setPosition(m_toGoBackButton.getPosition().x, m_toGoBackButton.getPosition().y - 5);
	m_toGoBackText.setCharacterSize(25U);
	m_toGoBackText.setOutlineColor(sf::Color::Black);
	m_toGoBackText.setFillColor(sf::Color::White);
	m_toGoBackText.setOutlineThickness(1.0f);
	m_toGoBackText.setOrigin(m_toGoBackText.getGlobalBounds().width / 2, m_toGoBackText.getGlobalBounds().height / 2);
}

void Game::initBackgroundImage()
{
	if (!m_backgroundTexture.loadFromFile("Assets\\Temp\\Down the Tubes.png"))
	{
		std::cout << "Error - Failed to load background image" << std::endl;
	}

	m_backGroundSprite.setTexture(m_backgroundTexture);

	m_backGroundSprite.setScale(static_cast<float>(Global::S_WIDTH) / m_backGroundSprite.getGlobalBounds().width,
		static_cast<float>(Global::S_HEIGHT) / m_backGroundSprite.getGlobalBounds().height);
}

void Game::handleLevelSelectionMouseInput(sf::Vector2i mousePosition)
{
	for (size_t i = 0; i < levelSelectionButtons.size(); ++i)
	{
		if (levelSelectionButtons[i].getGlobalBounds().contains(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y)))
		{
			levelSelectionButtons[selectedButtonIndex].setFillColor(sf::Color::Blue);
			selectedButtonIndex = static_cast<int>(i);
			levelSelectionButtons[selectedButtonIndex].setFillColor(sf::Color::Red);
			levelLoaded = false;
			loadLevel(levelFilenames[selectedButtonIndex]);
			break;
		}
	}
}

void Game::loadLevel(const std::string& filename)
{
	if (!levelLoaded)
	{
		m_levelEditor.loadLevelFromFile("Assets\\SaveFiles\\" + filename + ".txt");
		levelLoaded = true;
	}
}

void Game::updateFogOfWarBasedOnBuildings(const std::vector<Building*>& buildings)
{
	for (auto& building : buildings)
	{
		sf::Vector2f buildingCenter = building->getPosition();
		float radius = 300;

		// Determine the range of tiles affected by this building
		int minX = std::max(static_cast<int>((buildingCenter.x - radius) / m_tiles.tileSize), 0);
		int maxX = std::min(static_cast<int>((buildingCenter.x + radius) / m_tiles.tileSize), m_levelEditor.numCols - 1);
		int minY = std::max(static_cast<int>((buildingCenter.y - radius) / m_tiles.tileSize), 0);
		int maxY = std::min(static_cast<int>((buildingCenter.y + radius) / m_tiles.tileSize), m_levelEditor.numRows - 1);

		for (int i = minY; i <= maxY; ++i)
		{
			for (int j = minX; j <= maxX; ++j)
			{
				// Check if the tile is within the circle's radius
				sf::Vector2f tileCenter(j * m_tiles.tileSize + m_tiles.tileSize / 2.0f, i * m_tiles.tileSize + m_tiles.tileSize / 2.0f);
				float distance = std::sqrt(std::pow(tileCenter.x - buildingCenter.x, 2) + std::pow(tileCenter.y - buildingCenter.y, 2));

				if (distance <= radius)
				{
					m_levelEditor.m_tiles[i][j].fogStatus = Tile::FogStatus::Visible;
				}
			}
		}
	}
}

void Game::createUnit(sf::RenderWindow& window)
{
	if (m_gui.m_unitConfirmed && m_gui.m_selectedBuilding) 
	{
		sf::Vector2f buildingPosition = m_gui.m_selectedBuilding->getPosition();

		sf::Vector2f spawnPosition = buildingPosition + sf::Vector2f(0.0f, 60.0f);

		Harvester* newHarvester = new Harvester();
		newHarvester->setPosition(spawnPosition);

		units.push_back(newHarvester);

		m_gui.m_unitConfirmed = false;
		m_gui.m_selectedBuilding = nullptr;
	}
}

void Game::selectUnitAt(const sf::Vector2f& mousePos) 
{
	for (Unit* unit : units) 
	{
		if (unit->getSprite().getGlobalBounds().contains(mousePos)) 
		{
			m_selectedUnit = unit;
			break;
		}
	}
}
