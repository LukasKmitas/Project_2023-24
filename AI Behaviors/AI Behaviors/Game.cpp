#include "Game.h"

/// <summary>
/// default constructor
/// </summary>
Game::Game() :
	m_window{ sf::VideoMode{ Global::S_WIDTH, Global::S_HEIGHT, 32U }, "Gills & Glory" },
	m_exitGame{ false }
{
	gameView.setSize(sf::Vector2f(Global::S_WIDTH, Global::S_HEIGHT));
	gameView.setCenter(Global::S_WIDTH / 2, Global::S_HEIGHT / 2);
	m_window.setView(gameView);
	m_window.setMouseCursorVisible(false);

	if (!m_font.loadFromFile("Assets\\Fonts\\ManicSea_19.ttf"))
	{
		std::cout << "Error - loading font" << std::endl;
	}

	if (!m_cursorTexture.loadFromFile("Assets\\Images\\GUI\\pointer1.png"))
	{
		std::cout << "Error - loading cursor texture" << std::endl;
	}
	m_cursorSprite.setTexture(m_cursorTexture);

	createBase();
	initParticles();
	initLevelSelectionButtons();

	m_levelEditor.resetFogOfWar();

	selectionBox.setFillColor(sf::Color(0, 255, 0, 50));
	selectionBox.setOutlineColor(sf::Color(0, 255, 0));
	selectionBox.setOutlineThickness(1.0f);
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
		if (sf::Event::KeyPressed == newEvent.type) //user pressed a key
		{
			processKeys(newEvent);
		}
		if (sf::Event::Closed == newEvent.type)
		{
			m_exitGame = true;
		}
		if (newEvent.type == sf::Event::MouseWheelScrolled)
		{
			if (m_currentState == GameState::PlayGame || m_currentState == GameState::LevelEditor)
			{
				float zoomFactor = (newEvent.mouseWheelScroll.delta > 0) ? 0.9f : 1.1f;
				float newZoomLevel = currentZoomLevel * zoomFactor;

				if (newZoomLevel >= minZoomLevel && newZoomLevel <= maxZoomLevel)
				{
					currentZoomLevel = newZoomLevel;
					gameView.setSize(Global::S_WIDTH * currentZoomLevel, Global::S_HEIGHT * currentZoomLevel);
					updateViewWithMouse();
				}
			}
		}
		if (sf::Event::MouseButtonPressed == newEvent.type) // Check for mouse button press.
		{
			guiMousePosition = sf::Mouse::getPosition(m_window); // GUI
			worldMousePosition = m_window.mapPixelToCoords(sf::Mouse::getPosition(m_window)); // World
		}
		switch (m_currentState)
		{
		case GameState::MainMenu:
			if (newEvent.mouseButton.button == sf::Mouse::Left) // Check for left mouse button.
			{
				m_menu.handleButtonClick(m_window.mapPixelToCoords(guiMousePosition), m_currentState);
			}
			break;
		case GameState::PlayGame:
			if (sf::Event::MouseButtonPressed == newEvent.type)
			{
				if (newEvent.mouseButton.button == sf::Mouse::Left) // Check for left mouse button
				{
					m_gui.handleMouseClick(guiMousePosition, m_window);
					if (m_selectedUnit)
					{
						std::vector<Unit*> selectedUnits;
						for (Unit* unit : units)
						{
							if (unit->isSelected)
							{
								selectedUnits.push_back(unit);
							}
						}

						int gridSize = calculateGridSize(selectedUnits.size());
						float spacing = 80.0f;

						for (int i = 0; i < selectedUnits.size(); ++i)
						{
							int row = i / gridSize;
							int col = i % gridSize;

							sf::Vector2f gridPosition = sf::Vector2f(worldMousePosition.x + col * spacing, worldMousePosition.y + row * spacing);
							selectedUnits[i]->moveTo(gridPosition);
						}
					}
					else
					{
						selectUnitAt(worldMousePosition);
					}
					isDragging = true;
					dragStart = m_window.mapPixelToCoords(sf::Mouse::getPosition(m_window));
				}
				if (newEvent.mouseButton.button == sf::Mouse::Right) // Check for Right mouse button
				{
					m_selectedUnit = nullptr;
					for (Unit* unit : units)
					{
						unit->setSelected(false);
					}
				}
			}
			if (sf::Event::MouseButtonReleased == newEvent.type) // Check for release mouse button
			{
				if (newEvent.mouseButton.button == sf::Mouse::Left)
				{
					isDragging = false;
					selectUnitsInBox();
				}
				selectionBox.setSize(sf::Vector2f(0, 0));
			}
			if (sf::Event::MouseMoved == newEvent.type)
			{
				if (isDragging)
				{
					dragEnd = m_window.mapPixelToCoords(sf::Mouse::getPosition(m_window));
					selectionBox.setPosition(std::min(dragStart.x, dragEnd.x), std::min(dragStart.y, dragEnd.y));
					selectionBox.setSize(sf::Vector2f(std::abs(dragEnd.x - dragStart.x), std::abs(dragEnd.y - dragStart.y)));
				}
			}
			break;
		case GameState::LevelEditor:
			if (sf::Event::MouseButtonPressed == newEvent.type)
			{
				if (newEvent.mouseButton.button == sf::Mouse::Left) // Check for Left mouse button
				{
					m_levelEditor.handleMouseInput(guiMousePosition, m_currentState, m_window);
				}
			}
			if (sf::Event::MouseButtonReleased == newEvent.type) // Check for release mouse button
			{
				if (newEvent.mouseButton.button == sf::Mouse::Left)
				{
					m_levelEditor.releaseDragRect();
				}
			}
			break;
		case GameState::LevelLoad:
			if (sf::Event::MouseButtonPressed == newEvent.type)
			{
				if (newEvent.mouseButton.button == sf::Mouse::Left)
				{
					m_levelLoader.goToMainMenu(guiMousePosition, m_currentState);
					m_levelLoader.handleLevelSelectionMouseInput(guiMousePosition);
				}
			}
			break;
		case GameState::Exit:
			break;
		default:
			break;
		}
	}
}

/// <summary>
/// deal with key presses from the user
/// </summary>
/// <param name="t_event">key press event</param>
void Game::processKeys(sf::Event t_event)
{
	switch (m_currentState)
	{
	case GameState::MainMenu:
		if (sf::Keyboard::Escape == t_event.key.code)
		{
			m_exitGame = true;
		}
		break;
	case GameState::PlayGame:
		if (t_event.type == sf::Event::KeyPressed && t_event.key.code == sf::Keyboard::R)
		{
			resetZoom();
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
		if (t_event.type == sf::Event::KeyPressed && t_event.key.code == sf::Keyboard::R)
		{
			resetZoom();
		}
		m_levelEditor.handleRotationInput(t_event);
		break;
	case GameState::LevelLoad:
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
	sf::Vector2i mouseGUIPosition = sf::Mouse::getPosition(m_window); // GUI
	sf::Vector2f mouseWorldPos = m_window.mapPixelToCoords(sf::Mouse::getPosition(m_window)); // World
	m_cursorSprite.setPosition(static_cast<float>(mouseWorldPos.x), static_cast<float>(mouseWorldPos.y));
	switch (m_currentState)
	{
	case GameState::MainMenu:
		if (m_currentState != GameState::LevelEditor && m_previousState == GameState::LevelEditor)
		{
			resetView();
		}
		m_menu.update(t_deltaTime);
		break;
	case GameState::PlayGame:
		//m_levelLoader.update(t_deltaTime);
		loadLevel(levelFilenames[selectedButtonIndex]);
		updateViewWithMouse();
		createBuilding(m_window);
		m_gui.update(t_deltaTime);
		for (Building* building : placedBuildings)
		{
			building->update(t_deltaTime);
		}
		m_gui.handleBuildingPlacement(mouseGUIPosition, m_window);
		m_levelEditor.animationForResources();
		m_levelEditor.animationForWeed();
		createUnit();
		createEnemyUnit();
		for (auto& unit : units)
		{
			unit->update(t_deltaTime, enemyUnits);

			for (auto& bullet : unit->bullets)
			{
				for (auto& enemyUnit : enemyUnits)
				{
					if (bullet.bulletShape.getGlobalBounds().intersects(enemyUnit->getSprite().getGlobalBounds()))
					{
						bullet.active = false;
						enemyUnit->takeDamage(bullet.damage);
						if (enemyUnit->getHealth() <= 0)
						{
							enemyUnit->m_active = false;
						}
						spawnBulletSparkParticles(bullet.position);
					}
				}
			}
			// Remove inactive bullets
			unit->bullets.erase(std::remove_if(unit->bullets.begin(), unit->bullets.end(),
				[](const Bullet& bullet) { return !bullet.active; }), unit->bullets.end());
			// Remove dead enemy units
			enemyUnits.erase(std::remove_if(enemyUnits.begin(), enemyUnits.end(),
				[](Unit* enemyUnit) { return !enemyUnit->isActive(); }), enemyUnits.end());
		}
		for (Unit* eUnits : enemyUnits)
		{
			eUnits->update(t_deltaTime, enemyUnits);
		}
		m_particleSystem.update(t_deltaTime);
		updateFogOfWarBasedOnUnits(units);
		updateFogOfWarBasedOnBuildings(placedBuildings);
		spawnBubbleParticles();
		break;
	case GameState::LevelEditor:
		m_previousState = GameState::LevelEditor;
		updateViewWithMouse();
		m_levelEditor.update(t_deltaTime, m_window);
		break;
	case GameState::LevelLoad:
		//m_levelLoader.update(t_deltaTime);
		loadLevel(levelFilenames[selectedButtonIndex]);
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
		for (Unit* eUnit : enemyUnits)
		{
			eUnit->render(m_window);
		}
		m_gui.render(m_window);
		m_window.setView(gameView);
		if (isDragging)
		{
			m_window.draw(selectionBox);
		}
		m_particleSystem.render(m_window);
		break;
	case GameState::LevelEditor:
		m_levelEditor.render(m_window);
		m_window.setView(gameView);
		break;
	case GameState::LevelLoad:
		m_levelLoader.render(m_window);
		break;
	case GameState::Exit:
		break;
	default:
		break;
	}
	m_window.draw(m_cursorSprite);
	m_window.display();
}

/// <summary>
/// Update the screen view position
/// </summary>
void Game::updateViewWithMouse()
{
	sf::Vector2f mousePosition = sf::Vector2f(sf::Mouse::getPosition(m_window));

	switch (m_currentState)
	{
	case GameState::PlayGame:
		minX = 950;
		minY = 530;
		maxX = 1550;
		maxY = 1970;
		moveCamera(mousePosition);
		break;
	case GameState::LevelEditor:
		minX = 500;
		minY = 200;
		maxX = 1900;
		maxY = 2300;
		moveCamera(mousePosition);
		break;
	default:
		break;
	}
	gameView.setCenter(viewCenter);
}

void Game::moveCamera(sf::Vector2f mousePosition)
{
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
			sf::Vector2f buildingPosition = m_gui.m_selectedBuilding->getPosition();

			sf::Vector2f spawnPosition = worldMousePosition + sf::Vector2f(0.0f, 60.0f);
			Harvester* newHarvester = new Harvester();
			newHarvester->setPosition(spawnPosition);
			newHarvester->setTargetPosition(spawnPosition);
			newHarvester->setBuildings(placedBuildings);
			newHarvester->setTiles(m_levelEditor.m_tiles);
			newHarvester->currentState = newHarvester->MovingToResource;
			units.push_back(newHarvester);

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
	newHeadquarters->setPosition(sf::Vector2f(500.0f, 350.0f));
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

void Game::initParticles()
{
	if (!m_bubbleTexture.loadFromFile("Assets\\Images\\Particles\\46.png"))
	{
		std::cout << "Error - loading bubble texture" << std::endl;
	}
	if (!m_bulletSparksTexture.loadFromFile("Assets\\Images\\Particles\\20.png"))
	{
		std::cout << "Error - loading bubble texture" << std::endl;
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

void Game::updateFogOfWarBasedOnUnits(const std::vector<Unit*>& units)
{

	for (int i = 0; i < m_levelEditor.numRows; ++i)
	{
		for (int j = 0; j < m_levelEditor.numCols; ++j)
		{
			if (m_levelEditor.m_tiles[i][j].fogStatus == Tile::FogStatus::Visible)
			{
				m_levelEditor.m_tiles[i][j].fogStatus = Tile::FogStatus::Explored;
			}
		}
	}

	for (auto& unit : units)
	{
		sf::Vector2f unitCenter = unit->getPosition();
		float radius = unit->getViewRadius();

		int minX = std::max(static_cast<int>((unitCenter.x - radius) / m_tiles.tileSize), 0);
		int maxX = std::min(static_cast<int>((unitCenter.x + radius) / m_tiles.tileSize), m_levelEditor.numCols - 1);
		int minY = std::max(static_cast<int>((unitCenter.y - radius) / m_tiles.tileSize), 0);
		int maxY = std::min(static_cast<int>((unitCenter.y + radius) / m_tiles.tileSize), m_levelEditor.numRows - 1);

		for (int i = minY; i <= maxY; ++i)
		{
			for (int j = minX; j <= maxX; ++j)
			{
				sf::Vector2f tileCenter(j * m_tiles.tileSize + m_tiles.tileSize / 2.0f, i * m_tiles.tileSize + m_tiles.tileSize / 2.0f);
				float distance = std::sqrt(std::pow(tileCenter.x - unitCenter.x, 2) + std::pow(tileCenter.y - unitCenter.y, 2));

				if (distance <= radius)
				{
					m_levelEditor.m_tiles[i][j].fogStatus = Tile::FogStatus::Visible;
				}
			}
		}
	}
}

void Game::createUnit()
{
	if (m_gui.m_unitConfirmed)
	{
		// Vehicles
		if (m_gui.m_selectedVehicleType == VehicleType::Harvester)
		{
			if (m_gui.m_selectedBuilding)
			{
				sf::Vector2f buildingPosition = m_gui.m_selectedBuilding->getPosition();
				sf::Vector2f spawnPosition = buildingPosition + sf::Vector2f(0.0f, 60.0f);

				Harvester* newHarvester = new Harvester();
				newHarvester->setPosition(spawnPosition);
				newHarvester->setTargetPosition(spawnPosition);
				newHarvester->setBuildings(placedBuildings);
				newHarvester->setTiles(m_levelEditor.m_tiles);
				newHarvester->currentState = newHarvester->MovingToResource;

				units.push_back(newHarvester);

				m_gui.m_unitConfirmed = false;
				m_gui.m_selectedBuilding = nullptr;
			}
		}
		if (m_gui.m_selectedVehicleType == VehicleType::Buggy)
		{
			if (m_gui.m_selectedBuilding)
			{
				sf::Vector2f buildingPosition = m_gui.m_selectedBuilding->getPosition();
				sf::Vector2f spawnPosition = buildingPosition + sf::Vector2f(0.0f, 60.0f);

				Buggy* newBuggy = new Buggy();
				newBuggy->setPosition(spawnPosition);
				newBuggy->setTargetPosition(spawnPosition);
				newBuggy->setEnemyUnits(enemyUnits);
				units.push_back(newBuggy);

				m_gui.m_unitConfirmed = false;
				m_gui.m_selectedBuilding = nullptr;
			}
		}
		// Air
		if (m_gui.m_selectedAircraftType == AirCraftType::HammerHead)
		{
			if (m_gui.m_selectedBuilding)
			{
				sf::Vector2f buildingPosition = m_gui.m_selectedBuilding->getPosition();
				sf::Vector2f spawnPosition = buildingPosition + sf::Vector2f(0.0f, 60.0f);

				HammerHead* newHammerHead = new HammerHead();
				newHammerHead->setPosition(spawnPosition);
				newHammerHead->setTargetPosition(spawnPosition);
				units.push_back(newHammerHead);

				m_gui.m_unitConfirmed = false;
				m_gui.m_selectedBuilding = nullptr;
			}
		}
		if (m_gui.m_selectedAircraftType == AirCraftType::Firehawk)
		{
			if (m_gui.m_selectedBuilding)
			{
				sf::Vector2f buildingPosition = m_gui.m_selectedBuilding->getPosition();
				sf::Vector2f spawnPosition = buildingPosition + sf::Vector2f(0.0f, 60.0f);

				Firehawk* newFirehawk = new Firehawk();
				newFirehawk->setPosition(spawnPosition);
				newFirehawk->setTargetPosition(spawnPosition);
				units.push_back(newFirehawk);

				m_gui.m_unitConfirmed = false;
				m_gui.m_selectedBuilding = nullptr;
			}
		}
	}
}

void Game::createEnemyUnit()
{
	if (!once)
	{
		sf::Vector2f spawnPosition = sf::Vector2f(700.0f, 800.0f);

		Buggy* newBuggy = new Buggy();
		newBuggy->setPosition(spawnPosition);
		newBuggy->setTargetPosition(spawnPosition);

		enemyUnits.push_back(newBuggy);
		once = true;
	}
}

void Game::selectUnitAt(const sf::Vector2f& mousePos)
{
	for (Unit* unit : units)
	{
		if (unit->getSprite().getGlobalBounds().contains(mousePos))
		{
			m_selectedUnit = unit;
			unit->setSelected(true);
			break;
		}
	}
}

void Game::selectUnitsInBox()
{
	sf::FloatRect selectionRect = selectionBox.getGlobalBounds();

	for (Unit* unit : units)
	{
		if (selectionRect.intersects(unit->getSprite().getGlobalBounds()))
		{
			m_selectedUnit = unit;
			unit->setSelected(true);
		}
	}
}

void Game::spawnBubbleParticles()
{
	static sf::Clock spawnTimer;
	if (spawnTimer.getElapsedTime().asSeconds() > 2.0f)
	{
		spawnTimer.restart();

		int bubblesToSpawn = std::rand() % 100 + 20;

		for (int i = 0; i < bubblesToSpawn; ++i)
		{
			int randomX = std::rand() % m_levelEditor.numCols;
			int randomY = std::rand() % m_levelEditor.numRows;
			if (m_levelEditor.m_tiles[randomY][randomX].fogStatus != Tile::FogStatus::Unexplored)
			{
				sf::Vector2f position(randomX * m_tiles.tileSize + m_tiles.tileSize / 2.0f,
					randomY * m_tiles.tileSize + m_tiles.tileSize / 2.0f);

				angleParticle = static_cast<float>((std::rand() % 41) - 20);	// angle between -20 and 20
				speedParticle = 20.0f + static_cast<float>(std::rand() % 6);	// Speed Between 20 and 25

				sf::Vector2f velocity(std::sin(angleParticle * 3.14f / 180.0f) * speedParticle, -(speedParticle + static_cast<float>(std::rand() % 10)));

				lifetimeParticle = static_cast<float>(std::rand() % 4 + 3);	// lifetime between 3 and 6
				sizeParticle = static_cast<float>(std::rand() % 3 + 2);		// size between 2 and 4

				colorParticle = sf::Color(255, 255, 255, 120 + std::rand() % 135);

				m_particleSystem.addParticle(Particle(position, velocity, colorParticle, lifetimeParticle, sizeParticle, &m_bubbleTexture));
			}
		}
	}
}

void Game::spawnBulletSparkParticles(const sf::Vector2f& position)
{
	for (int i = 0; i < 20; ++i)
	{
		angleParticle = (std::rand() % 360) * 3.14159f / 180.0f;
		speedParticle = (std::rand() % 100) / 10.0f + 50.0f; // Random speed between 50 and 60
		sf::Vector2f velocity(std::cos(angleParticle) * speedParticle, std::sin(angleParticle) * speedParticle);

		lifetimeParticle = static_cast<float>(std::rand() % 40) / 100.0f + 0.1f;
		sizeParticle = static_cast<float>(std::rand() % 3 + 2);

		colorParticle = sf::Color(255, 255, 255, 160 + std::rand() % 95);
		m_particleSystem.addParticle(Particle(position, velocity, colorParticle, lifetimeParticle, sizeParticle, &m_bulletSparksTexture));
	}
}

/// <summary>
/// Resets the view back to normal
/// </summary>
void Game::resetView()
{
	gameView.setSize(sf::Vector2f(Global::S_WIDTH, Global::S_HEIGHT));
	gameView.setCenter(Global::S_WIDTH / 2, Global::S_HEIGHT / 2);
	m_window.setView(gameView);
	//m_levelLoader.initLevelSelectionButtons();
	initLevelSelectionButtons();
}

void Game::resetZoom()
{
	float initialWidth = Global::S_WIDTH;
	float initialHeight = Global::S_HEIGHT;

	gameView.setSize(initialWidth, initialHeight);
	updateViewWithMouse();
}

int Game::calculateGridSize(int numberOfUnits)
{
	return std::ceil(std::sqrt(numberOfUnits));
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

void Game::loadLevel(const std::string& filename)
{
	if (!levelLoaded)
	{
		m_levelEditor.loadLevelFromFile("Assets\\SaveFiles\\" + filename + ".txt");
		levelLoaded = true;
	}
}