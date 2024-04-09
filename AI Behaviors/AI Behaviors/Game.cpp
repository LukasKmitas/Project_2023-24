#include "Game.h"

/// <summary>
/// default constructor
/// </summary>
Game::Game() :
	m_window{ sf::VideoMode{ Global::S_WIDTH, Global::S_HEIGHT, 32U }, "Gills & Glory" },
	m_exitGame{ false },
	m_neural_network(NeuralNetworks())
{
	m_gameView.setSize(sf::Vector2f(Global::S_WIDTH, Global::S_HEIGHT));
	m_gameView.setCenter(Global::S_WIDTH / 2, Global::S_HEIGHT / 2);
	m_window.setView(m_gameView);
	m_window.setMouseCursorVisible(false);
	m_viewCenter = sf::Vector2f(Global::S_WIDTH / 2, Global::S_HEIGHT / 2);

	if (!m_font.loadFromFile("Assets\\Fonts\\ManicSea_19.ttf"))
	{
		std::cout << "Error - loading font" << std::endl;
	}
	if (!m_normalfont.loadFromFile("Assets\\Fonts\\ariblk.ttf"))
	{
		std::cout << "Error - loading font" << std::endl;
	}
	if (!m_cursorTexture.loadFromFile("Assets\\Images\\GUI\\pointer1.png"))
	{
		std::cout << "Error - loading cursor texture" << std::endl;
	}
	m_cursorSprite.setTexture(m_cursorTexture);

	initParticles();
	initWinLosePanel();

	createBase();
	createEnemyStarterBase();

	m_levelEditor.resetFogOfWar();

	m_selectionBox.setFillColor(sf::Color(0, 255, 0, 50));
	m_selectionBox.setOutlineColor(sf::Color(0, 255, 0));
	m_selectionBox.setOutlineThickness(1.0f);

	m_hiddenNeurons = m_neural_network.getHiddenNeurons();
	m_biasNeurons = m_neural_network.getBiasNeurons();
	initNeuralNetwork();
}

/// <summary>
/// default destructor
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
			if (newEvent.key.code == sf::Keyboard::Enter)
			{
				m_train = !m_train;
			}
		}
		if (m_showWinLosePanel && newEvent.type == sf::Event::MouseButtonPressed && newEvent.mouseButton.button == sf::Mouse::Left)
		{
			sf::Vector2f mousePosition = m_window.mapPixelToCoords(sf::Mouse::getPosition(m_window));
			m_guiMousePosition = sf::Mouse::getPosition(m_window);
			handleWinLosePanel(static_cast<sf::Vector2f>(m_guiMousePosition));
			continue;
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
				float zoomDistance = m_currentZoomLevel * zoomFactor;

				if (zoomDistance >= m_minZoomLevel && zoomDistance <= m_maxZoomLevel)
				{
					m_currentZoomLevel = zoomDistance;
					m_gameView.setSize(Global::S_WIDTH * m_currentZoomLevel, Global::S_HEIGHT * m_currentZoomLevel);
					updateViewWithMouse();
				}
			}
		}
		if (sf::Event::MouseButtonPressed == newEvent.type) // Check for mouse button press.
		{
			m_guiMousePosition = sf::Mouse::getPosition(m_window); // GUI
			m_worldMousePosition = m_window.mapPixelToCoords(sf::Mouse::getPosition(m_window)); // World
		}
		switch (m_currentState)
		{
		case GameState::MainMenu:
			if (newEvent.mouseButton.button == sf::Mouse::Left) // Check for left mouse button.
			{
				m_menu.handleButtonClick(m_window.mapPixelToCoords(m_guiMousePosition), m_currentState);
			}
			break;
		case GameState::PlayGame:
			if (sf::Event::MouseButtonPressed == newEvent.type)
			{
				if (newEvent.mouseButton.button == sf::Mouse::Left) // Check for left mouse button
				{
					m_gui.handleMouseClick(m_guiMousePosition, m_window);
					if (m_selectedUnit)
					{
						std::vector<Unit*> selectedUnits;
						for (Unit* unit : m_playerUnits)
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

							sf::Vector2f gridPosition = sf::Vector2f(m_worldMousePosition.x + col * spacing, m_worldMousePosition.y + row * spacing);
							selectedUnits[i]->moveTo(gridPosition);
						}
					}
					else
					{
						selectUnitAt(m_worldMousePosition);
					}
					m_isDragging = true;
					m_dragStart = m_window.mapPixelToCoords(sf::Mouse::getPosition(m_window));
				}
				if (newEvent.mouseButton.button == sf::Mouse::Right) // Check for Right mouse button
				{
					m_selectedUnit = nullptr;
					for (Unit* unit : m_playerUnits)
					{
						unit->setSelected(false);
					}
				}
			}
			if (sf::Event::MouseButtonReleased == newEvent.type) // Check for release mouse button
			{
				if (newEvent.mouseButton.button == sf::Mouse::Left)
				{
					m_isDragging = false;
					selectUnitsInBox();
				}
				m_selectionBox.setSize(sf::Vector2f(0, 0));
			}
			if (sf::Event::MouseMoved == newEvent.type)
			{
				if (m_isDragging)
				{
					m_dragEnd = m_window.mapPixelToCoords(sf::Mouse::getPosition(m_window));
					m_selectionBox.setPosition(std::min(m_dragStart.x, m_dragEnd.x), std::min(m_dragStart.y, m_dragEnd.y));
					m_selectionBox.setSize(sf::Vector2f(std::abs(m_dragEnd.x - m_dragStart.x), std::abs(m_dragEnd.y - m_dragStart.y)));
				}
			}
			break;
		case GameState::LevelEditor:
			if (sf::Event::MouseButtonPressed == newEvent.type)
			{
				if (newEvent.mouseButton.button == sf::Mouse::Left) // Check for Left mouse button
				{
					m_levelEditor.handleMouseInput(m_guiMousePosition, m_currentState, m_window);
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
					m_levelLoader.goToMainMenu(m_guiMousePosition, m_currentState);
					m_levelLoader.handleLevelSelectionMouseInput(m_guiMousePosition);
				}
			}
			break;
		case GameState::NeuralNetworks:
			if (sf::Event::MouseButtonPressed == newEvent.type)
			{
				if (newEvent.mouseButton.button == sf::Mouse::Left)
				{
					m_neural_network.goToMainMenu(m_guiMousePosition, m_currentState);

					int mouse_x = sf::Mouse::getPosition(m_window).x;
					int mouse_y = sf::Mouse::getPosition(m_window).y;

					if (mouse_x >= Global::S_WIDTH / 2 + 120 && mouse_x < m_window.getSize().x)
					{
						if (0 <= mouse_y && mouse_y < m_window.getSize().y)
						{
							float dot_x = (mouse_x - SCREEN_HEIGHT) / static_cast<float>(SCREEN_HEIGHT);
							float dot_y = mouse_y / static_cast<float>(SCREEN_WITDH);

							if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1))
							{
								m_neural_network.addInput(dot_x, dot_y);
								m_neural_network.addTargetOutput(1, 0, 0);
							}
							else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2))
							{
								m_neural_network.addInput(dot_x, dot_y);
								m_neural_network.addTargetOutput(0, 1, 0);
							}
							else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3))
							{
								m_neural_network.addInput(dot_x, dot_y);
								m_neural_network.addTargetOutput(0, 0, 1);
							}
						}
					}
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
			for (Unit* unit : m_playerUnits)
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
	//std::cout << mouseWorldPos.x << " " << mouseWorldPos.y << std::endl;
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
		if (m_gameWinLose == WinLoseState::NONE)
		{
			loadLevel(m_levelLoader.m_levelFilenames[m_levelLoader.m_selectedButtonIndex]);
			updateViewWithMouse();
			m_gui.update(t_deltaTime);
			m_gui.handleBuildingPlacement(mouseGUIPosition, m_window);
			createBuilding(m_window);
			createUnit();
			updatePlayerAssets(t_deltaTime);
			updateEnemyAssets(t_deltaTime);
			updateFogOfWarBasedOnUnits(m_playerUnits);
			updateFogOfWarBasedOnBuildings(m_placedPlayerBuildings);
			spawnBubbleParticles();
			updateEnemyAIDecisionOnCreating(t_deltaTime);
			updateEnemyAIUnitDecisionState(t_deltaTime);
			m_particleSystem.update(t_deltaTime);
		}
		m_levelEditor.animationForResources();
		m_levelEditor.animationForWeed();
		checkVictoryConditions();
		break;
	case GameState::LevelEditor:
		m_previousState = GameState::LevelEditor;
		updateViewWithMouse();
		m_levelEditor.update(t_deltaTime, m_window);
		break;
	case GameState::LevelLoad:
		loadLevel(m_levelLoader.m_levelFilenames[m_levelLoader.m_selectedButtonIndex]);
		break;
	case GameState::NeuralNetworks:
		updateNeuralNetwork();
		m_errorText.setString("Total Error: " + std::to_string(m_totalErrors));
		break;
	case GameState::Exit:
		m_exitGame = true;
		break;
	default:
		break;
	}
}

/// <summary>
/// Draws all the sprites, shapes and texts
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
		for (Building* playerBuilding : m_placedPlayerBuildings)
		{
			playerBuilding->render(m_window);
		}
		for (Building* enemyBuilding : m_placedEnemyBuildings)
		{
			sf::Vector2f buildingPosition = enemyBuilding->getPosition();
			if (checkPositionWithinMap(buildingPosition))
			{
				int tileX = static_cast<int>(buildingPosition.x / m_tiles.m_tileSize);
				int tileY = static_cast<int>(buildingPosition.y / m_tiles.m_tileSize);

				if (m_levelEditor.m_tiles[tileY][tileX].m_fogStatus == Tile::FogStatus::Visible)
				{
					enemyBuilding->render(m_window);
				}
			}
		}
		for (Unit* unit : m_playerUnits)
		{
			unit->render(m_window);
		}
		for (Unit* eUnit : m_enemyUnits)
		{
			sf::Vector2f unitPosition = eUnit->getPosition();
			if (checkPositionWithinMap(unitPosition))
			{
				int tileX = static_cast<int>(unitPosition.x / m_tiles.m_tileSize);
				int tileY = static_cast<int>(unitPosition.y / m_tiles.m_tileSize);

				if (m_levelEditor.m_tiles[tileY][tileX].m_fogStatus == Tile::FogStatus::Visible)
				{
					eUnit->render(m_window);
				}
			}
		}
		m_gui.render(m_window);
		if (m_showWinLosePanel)
		{
			renderWinLosePanel(m_window);
		}
		m_window.setView(m_gameView);
		if (m_gameWinLose == WinLoseState::NONE)
		{
			if (m_isDragging)
			{
				m_window.draw(m_selectionBox);
			}
			m_particleSystem.render(m_window);
		}
		break;
	case GameState::LevelEditor:
		m_levelEditor.render(m_window);
		m_window.setView(m_gameView);
		break;
	case GameState::LevelLoad:
		m_levelLoader.render(m_window);
		break;
	case GameState::NeuralNetworks:
		drawNeuralNetwork(m_window);
		m_neural_network.render(m_window);
		m_window.draw(m_errorText);
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
	sf::Vector2f viewSize = m_gameView.getSize();
	float viewHalfWidth = viewSize.x / 2.0f;
	float viewHalfHeight = viewSize.y / 2.0f;

	switch (m_currentState)
	{
	case GameState::PlayGame:
		m_minX = viewHalfWidth;
		m_minY = viewHalfHeight;
		m_maxX = 2500 - viewHalfWidth;
		m_maxY = 2500 - viewHalfHeight;
		moveCamera(mousePosition);
		break;
	case GameState::LevelEditor:
		m_minX = viewHalfWidth - 500;
		m_minY = viewHalfHeight - 500;
		m_maxX = 3000 - viewHalfWidth; 
		m_maxY = 3000 - viewHalfHeight;
		moveCamera(mousePosition);
		break;
	default:
		break;
	}

	// Clamp the viewCenter to be within the boundaries
	m_viewCenter.x = std::max(m_minX, std::min(m_viewCenter.x, m_maxX));
	m_viewCenter.y = std::max(m_minY, std::min(m_viewCenter.y, m_maxY));

	m_gameView.setCenter(m_viewCenter);
	m_window.setView(m_gameView);
}

void Game::moveCamera(sf::Vector2f m_mousePosition)
{
	if (m_mousePosition.x < Global::S_WIDTH * 0.01 && m_viewCenter.x - m_viewMoveSpeed > m_minX)
	{
		m_viewCenter.x -= m_viewMoveSpeed;
	}
	if (m_mousePosition.x > Global::S_WIDTH * 0.99 && m_viewCenter.x + m_viewMoveSpeed < m_maxX)
	{
		m_viewCenter.x += m_viewMoveSpeed;
	}
	if (m_mousePosition.y < Global::S_HEIGHT * 0.01 && m_viewCenter.y - m_viewMoveSpeed > m_minY)
	{
		m_viewCenter.y -= m_viewMoveSpeed;
	}
	if (m_mousePosition.y > Global::S_HEIGHT * 0.99 && m_viewCenter.y + m_viewMoveSpeed < m_maxY)
	{
		m_viewCenter.y += m_viewMoveSpeed;
	}
}

/// <summary>
/// creates the buildings for players side
/// </summary>
/// <param name="m_window"></param>
void Game::createBuilding(sf::RenderWindow& m_window)
{
	if (m_gui.m_confirmed)
	{
		sf::Vector2i mousePosition = sf::Mouse::getPosition(m_window);
		sf::Vector2f worldMousePosition = m_window.mapPixelToCoords(mousePosition);
		if (m_selectedBuildingType == BuildingType::Refinery)
		{
			Refinery* newRefinery = new Refinery();
			newRefinery->setPosition(worldMousePosition);
			m_placedPlayerBuildings.push_back(newRefinery);
			sf::Vector2f buildingPosition = m_gui.m_selectedBuilding->getPosition();

			sf::Vector2f spawnPosition = worldMousePosition + sf::Vector2f(0.0f, 60.0f);
			Harvester* newHarvester = new Harvester();
			newHarvester->setPosition(spawnPosition);
			newHarvester->setTargetPosition(spawnPosition);
			newHarvester->setBuildings(m_placedPlayerBuildings);
			newHarvester->setTiles(m_levelEditor.m_tiles);
			newHarvester->m_currentState = newHarvester->MovingToResource;
			m_playerUnits.push_back(newHarvester);

			std::cout << "Refinery Created" << std::endl;
		}
		else if (m_selectedBuildingType == BuildingType::Barracks)
		{
			Barracks* newBarracks = new Barracks();
			newBarracks->setPosition(worldMousePosition);
			m_placedPlayerBuildings.push_back(newBarracks);
			std::cout << "Barracks Created" << std::endl;
		}
		else if (m_selectedBuildingType == BuildingType::WarFactory)
		{
			WarFactory* newVehicle = new WarFactory();
			newVehicle->setPosition(worldMousePosition);
			m_placedPlayerBuildings.push_back(newVehicle);
			std::cout << "Vehicle Created" << std::endl;
		}
		else if (m_selectedBuildingType == BuildingType::AirCraft)
		{
			AirCraft* newAirCraft = new AirCraft();
			newAirCraft->setPosition(worldMousePosition);
			m_placedPlayerBuildings.push_back(newAirCraft);
			std::cout << "AirCraft Created" << std::endl;
		}
		m_playerBuildingStatCount++;
		m_gui.m_confirmBuildingPlacement = false;
		m_gui.m_confirmed = false;
		m_selectedBuildingType = BuildingType::Headquarters;
	}
}

/// <summary>
/// creates the players base
/// </summary>
void Game::createBase()
{
	Headquarters* newHeadquarters = new Headquarters();
	newHeadquarters->setPosition(sf::Vector2f(500.0f, 350.0f));
	m_placedPlayerBuildings.push_back(newHeadquarters);
	std::cout << "Base Initilised" << std::endl;
}

/// <summary>
/// Updates the players buildings, units and the weapons for the unit.
/// </summary>
/// <param name="t_deltaTime"></param>
void Game::updatePlayerAssets(sf::Time t_deltaTime)
{
	for (Building* playerBuilding : m_placedPlayerBuildings)
	{
		playerBuilding->update(t_deltaTime);
	}

	for (auto& unit : m_playerUnits)
	{
		unit->update(t_deltaTime, m_playerUnits);

		for (auto& bullet : unit->m_bullets) // BULLETS
		{
			bool hit = false;

			for (auto& enemyUnit : m_enemyUnits)
			{
				if (bullet.m_bulletShape.getGlobalBounds().intersects(enemyUnit->getSprite().getGlobalBounds()))
				{
					bullet.m_active = false;
					enemyUnit->takeDamage(unit->getDamage());
					if (enemyUnit->getHealth() <= 0)
					{
						enemyUnit->m_active = false;
					}
					spawnBulletSparkParticles(bullet.m_position);
				}
			}

			for (auto& enemyBuilding : m_placedEnemyBuildings)
			{
				if (bullet.m_bulletShape.getGlobalBounds().intersects(enemyBuilding->getBuildingSprite().getGlobalBounds()))
				{
					bullet.m_active = false;
					enemyBuilding->takeDamage(unit->getDamage());
					hit = true;
					spawnBulletSparkParticles(bullet.m_position);
					break;
				}
			}
			if (hit)
			{
				continue;
			}
		}
		for (auto& missile : unit->m_missiles) // MISSILES
		{
			bool hit = false;

			for (auto& enemyUnit : m_enemyUnits)
			{
				if (missile.m_missileSprite.getGlobalBounds().intersects(enemyUnit->getSprite().getGlobalBounds()))
				{
					missile.m_active = false;
					enemyUnit->takeDamage(missile.m_damage);
					if (enemyUnit->getHealth() <= 0)
					{
						enemyUnit->m_active = false;
					}
					spawnExplosionParticle(missile.m_position);
				}
			}
			for (auto& enemyBuilding : m_placedEnemyBuildings)
			{
				if (missile.m_missileSprite.getGlobalBounds().intersects(enemyBuilding->getBuildingSprite().getGlobalBounds()))
				{
					missile.m_active = false;
					enemyBuilding->takeDamage(unit->getDamage());
					hit = true;
					spawnExplosionParticle(missile.m_position);
					break;
				}
			}
			if (hit)
			{
				continue;
			}
		}

		// Remove inactive bullets
		unit->m_bullets.erase(std::remove_if(unit->m_bullets.begin(), unit->m_bullets.end(),
			[](const Bullet& bullet) { return !bullet.m_active; }), unit->m_bullets.end());
		// Remove inactive missiles
		unit->m_missiles.erase(std::remove_if(unit->m_missiles.begin(), unit->m_missiles.end(),
			[](const Missile& missile) { return !missile.m_active && missile.m_trail.empty(); }), unit->m_missiles.end());

		// Remove dead enemy units
		m_enemyUnits.erase(std::remove_if(m_enemyUnits.begin(), m_enemyUnits.end(),
			[](Unit* enemyUnit) { return !enemyUnit->isActive(); }), m_enemyUnits.end());
		// Remove buildings
		m_placedEnemyBuildings.erase(std::remove_if(m_placedEnemyBuildings.begin(), m_placedEnemyBuildings.end(),
			[](Building* building) { return building->getHealth() <= 0; }), m_placedEnemyBuildings.end());
	}
}

/// <summary>
/// Saves the level file name
/// </summary>
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

/// <summary>
/// initializes the neural networks
/// </summary>
void Game::initNeuralNetwork()
{
	m_errorText.setFont(m_normalfont);
	m_errorText.setCharacterSize(20);
	m_errorText.setFillColor(sf::Color::White);
	m_errorText.setPosition(400, 10);

	m_neuralNetwork.resize(2 + m_hiddenNeurons.size());
	m_neuralNetwork[0].resize(m_neural_network.INPUT_NEURONS + m_biasNeurons[0], 0);
	m_neuralNetwork.back().resize(m_neural_network.OUTPUT_NEURONS, 0);

	m_random.seed(std::chrono::system_clock::now().time_since_epoch().count());
	std::uniform_real_distribution<float> m_distribution(-1, 1); // its to distribute random weights

	m_weights.resize(m_neuralNetwork.size() - 1);

	m_outputImage.create(m_outputWidth, m_outputHeight);
	m_outputTexture.loadFromImage(m_outputImage);
	m_outputSprite.setPosition(SCREEN_HEIGHT, 0);
	//outputs_sprite.setScale(15, 17);
	m_outputSprite.setScale(SCREEN_HEIGHT / m_outputWidth, SCREEN_WITDH / m_outputHeight);
	m_outputSprite.setOrigin(m_outputSprite.getGlobalBounds().width / 2, m_outputSprite.getGlobalBounds().height / 2);
	m_outputSprite.setTexture(m_outputTexture);

	for (size_t i = 1; i < m_neuralNetwork.size() - 1; ++i)
	{
		m_neuralNetwork[i].resize(m_biasNeurons[i] + m_hiddenNeurons[i - 1], 0);
	}

	for (size_t i = 0; i < m_weights.size(); ++i)
	{
		size_t m_layerSize = m_neuralNetwork[i + 1].size() - (i < m_weights.size() - 1 ? m_biasNeurons[i + 1] : 0);
		m_weights[i].resize(m_layerSize, vector_1d(m_neuralNetwork[i].size()));

		for (auto& m_weightLayer : m_weights[i])
		{
			for (float& m_weight : m_weightLayer)
			{
				m_weight = m_distribution(m_random); // Gives random weights at the beginning
			}
		}
	}

	m_errors.resize(m_weights.size());

	for (size_t i = 0; i < m_errors.size(); ++i)
	{
		m_errors[i].resize(m_weights[i].size(), 0);
	}
}

/// <summary>
/// Updates the nerual network
/// 
/// This is also how i train it, adjusting the weights and biases based on the error between the actual and predicted outputs,
/// which IS a learning process. 
/// The network's ability to improve its predictions over time as it processes more data is neural network learning
/// </summary>
void Game::updateNeuralNetwork()
{
	if (m_train) // Training the neural network
	{
		m_totalErrors = 0;

		for (int i = 0; i < TRAININGS_PER_FRAME; i++)
		{
			int m_inputIndex = rand() % m_neural_network.getInputs().size();

			m_neural_network.forwardPropagation(true, m_neural_network.getInputs()[m_inputIndex], m_neuralNetwork, m_weights);
			m_neural_network.backPropagation(m_neural_network.getTargetOutputs()[m_inputIndex], m_errors, m_neuralNetwork, m_weights);
		}

		for (int i = 0; i < m_neural_network.getInputs().size(); i++)
		{
			vector_1d m_outputs = m_neural_network.forwardPropagation(false, m_neural_network.getInputs()[i], m_neuralNetwork, m_weights);

			for (int j = 0; j < m_outputs.size(); j++)
			{
				m_totalErrors += std::abs(m_outputs[j] - m_neural_network.getTargetOutputs()[i][j]);
			}
		}
	}
	m_outputTexture.update(m_outputImage);
}

/// <summary>
/// Renders the neural networks 
/// </summary>
/// <param name="m_window"></param>
void Game::drawNeuralNetwork(sf::RenderWindow& m_window)
{
	m_neural_network.drawNeuralPerceptron(m_window, m_neuralNetwork, m_weights); // draws the perceptrons

	// For visualization to see it working on the image sprite
	for (int i = 0; i < m_outputWidth; i++)
	{
		for (int j = 0; j < m_outputHeight; j++)
		{
			float m_inputOne = i / static_cast<float>(m_outputWidth);
			float m_inputTwo = j / static_cast<float>(m_outputHeight);

			std::vector m_outputColor = m_neural_network.forwardPropagation(0, { m_inputOne, m_inputTwo }, m_neuralNetwork, m_weights);

			m_outputImage.setPixel(i, j, sf::Color(round(255 * m_outputColor[0]), round(255 * m_outputColor[1]), round(255 * m_outputColor[2])));
		}
	}

	m_window.draw(m_outputSprite);
}

/// <summary>
/// Updates the fog for the buildings
/// </summary>
/// <param name="m_buildings"></param>
void Game::updateFogOfWarBasedOnBuildings(const std::vector<Building*>& m_buildings)
{
	for (auto& building : m_buildings)
	{
		sf::Vector2f m_buildingCenter = building->getPosition();
		float m_buildingRadius = 300;

		// Determine the range of tiles affected by this building
		int minX = std::max(static_cast<int>((m_buildingCenter.x - m_buildingRadius) / m_tiles.m_tileSize), 0);
		int maxX = std::min(static_cast<int>((m_buildingCenter.x + m_buildingRadius) / m_tiles.m_tileSize), m_levelEditor.m_numCols - 1);
		int minY = std::max(static_cast<int>((m_buildingCenter.y - m_buildingRadius) / m_tiles.m_tileSize), 0);
		int maxY = std::min(static_cast<int>((m_buildingCenter.y + m_buildingRadius) / m_tiles.m_tileSize), m_levelEditor.m_numRows - 1);

		for (int i = minY; i <= maxY; ++i)
		{
			for (int j = minX; j <= maxX; ++j)
			{
				// Check if the tile is within the circle's radius
				sf::Vector2f m_tileCenter(j * m_tiles.m_tileSize + m_tiles.m_tileSize / 2.0f, i * m_tiles.m_tileSize + m_tiles.m_tileSize / 2.0f);
				float m_distance = std::sqrt(std::pow(m_tileCenter.x - m_buildingCenter.x, 2) + std::pow(m_tileCenter.y - m_buildingCenter.y, 2));

				if (m_distance <= m_buildingRadius)
				{
					m_levelEditor.m_tiles[i][j].m_fogStatus = Tile::FogStatus::Visible;
				}
			}
		}
	}
}

/// <summary>
/// Updates the fog for units
/// </summary>
/// <param name="m_units"></param>
void Game::updateFogOfWarBasedOnUnits(const std::vector<Unit*>& m_units)
{

	for (int i = 0; i < m_levelEditor.m_numRows; ++i)
	{
		for (int j = 0; j < m_levelEditor.m_numCols; ++j)
		{
			if (m_levelEditor.m_tiles[i][j].m_fogStatus == Tile::FogStatus::Visible)
			{
				m_levelEditor.m_tiles[i][j].m_fogStatus = Tile::FogStatus::Explored;
			}
		}
	}

	for (auto& unit : m_units)
	{
		sf::Vector2f m_unitCenter = unit->getPosition();
		float m_unitRadius = unit->getViewRadius();

		int minX = std::max(static_cast<int>((m_unitCenter.x - m_unitRadius) / m_tiles.m_tileSize), 0);
		int maxX = std::min(static_cast<int>((m_unitCenter.x + m_unitRadius) / m_tiles.m_tileSize), m_levelEditor.m_numCols - 1);
		int minY = std::max(static_cast<int>((m_unitCenter.y - m_unitRadius) / m_tiles.m_tileSize), 0);
		int maxY = std::min(static_cast<int>((m_unitCenter.y + m_unitRadius) / m_tiles.m_tileSize), m_levelEditor.m_numRows - 1);

		for (int i = minY; i <= maxY; ++i)
		{
			for (int j = minX; j <= maxX; ++j)
			{
				sf::Vector2f m_tileCenter(j * m_tiles.m_tileSize + m_tiles.m_tileSize / 2.0f, i * m_tiles.m_tileSize + m_tiles.m_tileSize / 2.0f);
				float m_distance = std::sqrt(std::pow(m_tileCenter.x - m_unitCenter.x, 2) + std::pow(m_tileCenter.y - m_unitCenter.y, 2));

				if (m_distance <= m_unitRadius)
				{
					m_levelEditor.m_tiles[i][j].m_fogStatus = Tile::FogStatus::Visible;
				}
			}
		}
	}
}

/// <summary>
/// Creates the unit for players side
/// </summary>
void Game::createUnit()
{
	if (m_gui.m_unitConfirmed)
	{
		sf::Vector2f targetPositionOffset = sf::Vector2f(0, 60);

		// Infantry 
		if (m_gui.m_selectedInfantryType == InfantryType::RifleSquad)
		{
			if (m_gui.m_selectedBuilding)
			{
				sf::Vector2f buildingPosition = m_gui.m_selectedBuilding->getPosition();
				sf::Vector2f spawnPosition = buildingPosition + sf::Vector2f(0.0f, 60.0f);

				RiflemanSquad* newRiflemanSquad = new RiflemanSquad();
				newRiflemanSquad->setPosition(spawnPosition);
				newRiflemanSquad->setTargetPosition(spawnPosition + targetPositionOffset);
				newRiflemanSquad->setEnemyUnits(m_enemyUnits);
				newRiflemanSquad->setEnemyBuildings(m_placedEnemyBuildings);
				newRiflemanSquad->setTiles(m_levelEditor.m_tiles);

				m_playerUnits.push_back(newRiflemanSquad);
			}
		}
		// Vehicles
		if (m_gui.m_selectedVehicleType == VehicleType::Harvester)
		{
			if (m_gui.m_selectedBuilding)
			{
				sf::Vector2f buildingPosition = m_gui.m_selectedBuilding->getPosition();
				sf::Vector2f spawnPosition = buildingPosition + sf::Vector2f(0.0f, 60.0f);

				Harvester* newHarvester = new Harvester();
				newHarvester->setPosition(spawnPosition);
				newHarvester->setTargetPosition(spawnPosition + targetPositionOffset);
				newHarvester->setBuildings(m_placedPlayerBuildings);
				newHarvester->setTiles(m_levelEditor.m_tiles);
				newHarvester->m_currentState = newHarvester->MovingToResource;

				m_playerUnits.push_back(newHarvester);
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
				newBuggy->setTargetPosition(spawnPosition + targetPositionOffset);
				newBuggy->setEnemyUnits(m_enemyUnits);
				newBuggy->setEnemyBuildings(m_placedEnemyBuildings);
				newBuggy->setTiles(m_levelEditor.m_tiles);

				m_playerUnits.push_back(newBuggy);
			}
		}
		if (m_gui.m_selectedVehicleType == VehicleType::TankAurora)
		{
			if (m_gui.m_selectedBuilding)
			{
				sf::Vector2f buildingPosition = m_gui.m_selectedBuilding->getPosition();
				sf::Vector2f spawnPosition = buildingPosition + sf::Vector2f(0.0f, 60.0f);

				TankAurora* newTankAurora = new TankAurora();
				newTankAurora->setPosition(spawnPosition);
				newTankAurora->setTargetPosition(spawnPosition + targetPositionOffset);
				newTankAurora->setEnemyUnits(m_enemyUnits);
				newTankAurora->setTiles(m_levelEditor.m_tiles);

				m_playerUnits.push_back(newTankAurora);
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
				newHammerHead->setTargetPosition(spawnPosition + targetPositionOffset);
				newHammerHead->setEnemyUnits(m_enemyUnits);
				newHammerHead->setEnemyBuildings(m_placedEnemyBuildings);

				m_playerUnits.push_back(newHammerHead);
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
				newFirehawk->setTargetPosition(spawnPosition + targetPositionOffset);
				newFirehawk->setEnemyUnits(m_enemyUnits);
				newFirehawk->setEnemyBuildings(m_placedEnemyBuildings);

				m_playerUnits.push_back(newFirehawk);
			}
		}
		m_playerUnitStatCount++;
		m_gui.m_unitConfirmed = false;
		m_gui.m_selectedBuilding = nullptr;
		m_gui.m_selectedBuildingType = BuildingType::None;
		m_gui.m_selectedInfantryType = InfantryType::None;
		m_gui.m_selectedVehicleType = VehicleType::None;
		m_gui.m_selectedAircraftType = AirCraftType::None;
	}
}

/// <summary>
/// Selects the unit
/// </summary>
/// <param name="m_mousePos"></param>
void Game::selectUnitAt(const sf::Vector2f& m_mousePos)
{
	for (Unit* unit : m_playerUnits)
	{
		if (unit->getSprite().getGlobalBounds().contains(m_mousePos))
		{
			m_selectedUnit = unit;
			unit->setSelected(true);
			break;
		}
	}
}

/// <summary>
/// To select all units within the box
/// </summary>
void Game::selectUnitsInBox()
{
	sf::FloatRect selectionRect = m_selectionBox.getGlobalBounds();

	for (Unit* unit : m_playerUnits)
	{
		if (selectionRect.intersects(unit->getSprite().getGlobalBounds()))
		{
			m_selectedUnit = unit;
			unit->setSelected(true);
		}
	}
}

/// <summary>
/// Spawns a bubble particle effect around the world to give the theme better look
/// </summary>
void Game::spawnBubbleParticles()
{
	static sf::Clock m_spawnTimer;
	if (m_spawnTimer.getElapsedTime().asSeconds() > 2.0f)
	{
		m_spawnTimer.restart();

		int m_bubblesToSpawn = std::rand() % 100 + 20;

		for (int i = 0; i < m_bubblesToSpawn; ++i)
		{
			int m_randomX = std::rand() % m_levelEditor.m_numCols;
			int m_randomY = std::rand() % m_levelEditor.m_numRows;

			if (m_levelEditor.m_tiles[m_randomY][m_randomX].m_fogStatus != Tile::FogStatus::Unexplored)
			{
				sf::Vector2f m_position(m_randomX * m_tiles.m_tileSize + m_tiles.m_tileSize / 2.0f,
					m_randomY * m_tiles.m_tileSize + m_tiles.m_tileSize / 2.0f);

				m_angleParticle = static_cast<float>((std::rand() % 41) - 20);	// angle between -20 and 20
				m_speedParticle = 20.0f + static_cast<float>(std::rand() % 6);	// Speed Between 20 and 25

				sf::Vector2f velocity(std::sin(m_angleParticle * 3.14f / 180.0f) * m_speedParticle, -(m_speedParticle + static_cast<float>(std::rand() % 10)));

				m_lifetimeParticle = static_cast<float>(std::rand() % 4 + 3);	// lifetime between 3 and 6
				m_sizeParticle = static_cast<float>(std::rand() % 3 + 2);		// size between 2 and 4
				m_colorParticle = sf::Color(255, 255, 255, 120 + std::rand() % 135);

				m_particleSystem.addParticle(Particle(m_position, velocity, m_colorParticle, m_lifetimeParticle, m_sizeParticle, &m_bubbleTexture));
			}
		}
	}
}

/// <summary>
/// spawns a spark particle effect
/// </summary>
/// <param name="m_position"></param>
void Game::spawnBulletSparkParticles(const sf::Vector2f& m_position)
{
	int m_particleCount = 20;

	for (int i = 0; i < m_particleCount; ++i)
	{
		m_angleParticle = (std::rand() % 360) * 3.14159f / 180.0f;
		m_speedParticle = (std::rand() % 100) / 10.0f + 50.0f; // Random speed between 50 and 60
		sf::Vector2f m_velocity(std::cos(m_angleParticle) * m_speedParticle, std::sin(m_angleParticle) * m_speedParticle);

		m_lifetimeParticle = static_cast<float>(std::rand() % 40) / 100.0f + 0.1f;
		m_sizeParticle = static_cast<float>(std::rand() % 3 + 2);

		m_colorParticle = sf::Color(255, 255, 255, 160 + std::rand() % 95);
		m_particleSystem.addParticle(Particle(m_position, m_velocity, m_colorParticle, m_lifetimeParticle, m_sizeParticle, &m_bulletSparksTexture));
	}
}

/// <summary>
/// spawns an explosion particle effect
/// </summary>
/// <param name="m_position"></param>
void Game::spawnExplosionParticle(const sf::Vector2f& m_position)
{
	int m_particleCount = 30;

	std::vector<sf::Color> m_randomColors =
	{
		sf::Color::Red,  
		sf::Color::Yellow, 
		sf::Color(255, 165, 0), 
		sf::Color(255, 140, 0), 
		sf::Color(128, 128, 128),
		sf::Color(80, 80, 80), 
		sf::Color::Black 
	};

	for (int i = 0; i < m_particleCount; ++i) 
	{
		float m_angle = (std::rand() % 360) * 3.14f / 180.0f;
		float m_speed = static_cast<float>(std::rand() % 50 + 50); 
		sf::Vector2f m_velocity(m_speed * std::cos(m_angle), m_speed * std::sin(m_angle));
		sf::Color m_color = m_randomColors[std::rand() % m_randomColors.size()];

		float m_size = static_cast<float>(std::rand() % 2 + 1);
		float m_lifetime = 0.1f + static_cast<float>(std::rand() % 10) / 10.0f;

		m_particleSystem.addParticle(Particle(m_position, m_velocity, m_color, m_lifetime, m_size));
	}
}

/// <summary>
/// Initializes the win/lose panel
/// </summary>
void Game::initWinLosePanel()
{
	if (!m_panelBackgroundTexture.loadFromFile("Assets\\Images\\GUI\\panel.png"))
	{
		std::cout << "Error - loading Panel texture" << std::endl;
	}

	m_panelBackgroundSprite.setTexture(m_panelBackgroundTexture);
	m_panelBackgroundSprite.setOrigin(m_panelBackgroundSprite.getGlobalBounds().width / 2, m_panelBackgroundSprite.getGlobalBounds().height / 2);
	m_panelBackgroundSprite.setScale(5, 5);
	m_panelBackgroundSprite.setPosition(Global::S_WIDTH / 2, Global::S_HEIGHT / 2 - 100);

	m_winLoseText.setFont(m_font);
	m_winLoseText.setString("Victory!");
	m_winLoseText.setCharacterSize(50);
	m_winLoseText.setFillColor(sf::Color::White);
	m_winLoseText.setOutlineColor(sf::Color::Yellow);
	m_winLoseText.setOutlineThickness(0.3);
	m_winLoseText.setOrigin(m_winLoseText.getGlobalBounds().width / 2, m_winLoseText.getGlobalBounds().height / 2);
	m_winLoseText.setPosition(Global::S_WIDTH / 2, 300);

	m_playAgainButton.setSize(sf::Vector2f(150, 50));
	m_playAgainButton.setOrigin(m_playAgainButton.getSize().x / 2, m_playAgainButton.getSize().y / 2);
	m_playAgainButton.setFillColor(sf::Color(100, 100, 250));
	m_playAgainButton.setPosition(Global::S_WIDTH / 2, 550);

	m_playAgainText.setFont(m_font);
	m_playAgainText.setString("Play Again");
	m_playAgainText.setCharacterSize(24);
	m_playAgainText.setFillColor(sf::Color::White);
	m_playAgainText.setOutlineColor(sf::Color::Black);
	m_playAgainText.setOutlineThickness(0.5);
	m_playAgainText.setOrigin(m_playAgainText.getLocalBounds().width / 2, m_playAgainText.getLocalBounds().height / 2);
	m_playAgainText.setPosition(m_playAgainButton.getPosition());

	m_exitButton.setSize(sf::Vector2f(150, 50));
	m_exitButton.setOrigin(m_exitButton.getSize().x / 2, m_exitButton.getSize().y / 2);
	m_exitButton.setFillColor(sf::Color(250, 100, 100));
	m_exitButton.setPosition(Global::S_WIDTH / 2, 650);

	m_exitText.setFont(m_font);
	m_exitText.setString("Exit");
	m_exitText.setCharacterSize(24);
	m_exitText.setFillColor(sf::Color::White);
	m_exitText.setOutlineColor(sf::Color::Black);
	m_exitText.setOutlineThickness(0.5);
	m_exitText.setOrigin(m_exitText.getLocalBounds().width / 2, m_exitText.getLocalBounds().height / 2);
	m_exitText.setPosition(m_exitButton.getPosition());

	// STATS
	m_playerStatsText.setFont(m_font);
	m_playerStatsText.setCharacterSize(24);
	m_playerStatsText.setFillColor(sf::Color::White);
	m_playerStatsText.setString("Player Stats:");
	m_playerStatsText.setOrigin(m_playerStatsText.getLocalBounds().width / 2, m_playerStatsText.getLocalBounds().height / 2);
	m_playerStatsText.setPosition(Global::S_WIDTH / 2 - 250,
		m_panelBackgroundSprite.getPosition().y - 50);

	m_enemyStatsText.setFont(m_font);
	m_enemyStatsText.setCharacterSize(24);
	m_enemyStatsText.setFillColor(sf::Color::White);
	m_enemyStatsText.setString("Enemy Stats:");
	m_enemyStatsText.setOrigin(m_enemyStatsText.getLocalBounds().width / 2, m_enemyStatsText.getLocalBounds().height / 2);
	m_enemyStatsText.setPosition(Global::S_WIDTH / 2 + 200,
		m_panelBackgroundSprite.getPosition().y - 50);

	m_gameDurationText.setFont(m_font);
	m_gameDurationText.setCharacterSize(20);
	m_gameDurationText.setFillColor(sf::Color::White);
	m_gameDurationText.setOutlineColor(sf::Color::Yellow);
	m_gameDurationText.setOutlineThickness(0.2);
	m_gameDurationText.setString("Game Duration: 00:00");
	m_gameDurationText.setOrigin(m_gameDurationText.getLocalBounds().width / 2, m_gameDurationText.getLocalBounds().height / 2);
	m_gameDurationText.setPosition(Global::S_WIDTH / 2, m_winLoseText.getPosition().y + 100); 
}

/// <summary>
/// Renders the win/lose panel with stats
/// </summary>
/// <param name="m_window"></param>
void Game::renderWinLosePanel(sf::RenderWindow& m_window)
{
	m_window.draw(m_panelBackgroundSprite);
	m_window.draw(m_winLoseText);
	m_window.draw(m_playAgainButton);
	m_window.draw(m_playAgainText);
	m_window.draw(m_exitButton);
	m_window.draw(m_exitText);
	m_window.draw(m_playerStatsText);
	m_window.draw(m_enemyStatsText);
	m_window.draw(m_gameDurationText);
	
	m_playerStatsText.setString("PLAYER STATS\n\nBuildings: " + std::to_string(m_playerBuildingStatCount) +
		"\nUnits: " + std::to_string(m_playerUnitStatCount));
	m_enemyStatsText.setString("ENEMY STATS\n\nBuildings: " + std::to_string(m_enemyBuildingStatCount) +
		"\nUnits: " + std::to_string(m_enemyUnitStatCount));

	m_gameDurationText.setString("Game duration: " + std::to_string(m_minutes) + ":" + (m_seconds < 10 ? "0" : "") + std::to_string(m_seconds));
}

/// <summary>
/// handles the mouse input for the win/lose panel
/// </summary>
/// <param name="m_mousePosition"></param>
void Game::handleWinLosePanel(const sf::Vector2f& m_mousePosition)
{
	if (m_playAgainButton.getGlobalBounds().contains(m_mousePosition)) 
	{
		m_showWinLosePanel = false;
		gameReset();
		std::cout << "Playing again" << std::endl;
	}
	if (m_exitButton.getGlobalBounds().contains(m_mousePosition))
	{
		m_showWinLosePanel = false;
		m_currentState = GameState::MainMenu;
		m_levelLoaded = false;
		gameReset();
		std::cout << "Back to Main menu" << std::endl;
	}
}

/// <summary>
/// Resets the view back to normal
/// </summary>
void Game::resetView()
{
	m_gameView.setSize(sf::Vector2f(Global::S_WIDTH, Global::S_HEIGHT));
	m_gameView.setCenter(Global::S_WIDTH / 2, Global::S_HEIGHT / 2);
	m_window.setView(m_gameView);
	m_levelLoader.initLevelSelectionButtons();
}

/// <summary>
/// Resets the view for zooming
/// </summary>
void Game::resetZoom()
{
	float initialWidth = Global::S_WIDTH;
	float initialHeight = Global::S_HEIGHT;

	m_gameView.setSize(initialWidth, initialHeight);
	updateViewWithMouse();
}

int Game::calculateGridSize(int m_numberOfUnits)
{
	return std::ceil(std::sqrt(m_numberOfUnits));
}

/// <summary>
/// Loads the level
/// </summary>
/// <param name="m_filename"></param>
void Game::loadLevel(const std::string& m_filename)
{
	if (!m_levelLoaded)
	{
		m_levelEditor.loadLevelFromFile("Assets\\SaveFiles\\" + m_filename + ".txt");
		m_levelLoaded = true;
	}
}

/// <summary>
/// Checks the victory and lose conditions
/// </summary>
void Game::checkVictoryConditions()
{
	if (m_gameWinLose == WinLoseState::NONE)
	{
		m_gameDurationTime = m_gameDurationClock.getElapsedTime();
		m_minutes = static_cast<int>(m_gameDurationTime.asSeconds()) / 60;
		m_seconds = static_cast<int>(m_gameDurationTime.asSeconds()) % 60;
	}

	if (m_placedEnemyBuildings.empty())
	{
		m_gameWinLose = WinLoseState::WIN;
		m_showWinLosePanel = true;
		m_winLoseText.setString("Victory!");
	}
	else if (m_placedPlayerBuildings.empty())
	{
		m_gameWinLose = WinLoseState::LOSE;
		m_showWinLosePanel = true;
		m_winLoseText.setString("Game Over\nYou lost");
	}
}

/// <summary>
/// Creates some buildings and units to give an advantage to the enemy a bit at the start of the game
/// </summary>
void Game::createEnemyStarterBase()
{
	sf::Vector2f m_targetPositionOffset = sf::Vector2f(0, 60);

	Headquarters* newHeadquarters = new Headquarters();
	newHeadquarters->setPosition(sf::Vector2f(1900.0f, 2100.0f));
	m_placedEnemyBuildings.push_back(newHeadquarters);

	Refinery* newRefinery = new Refinery();
	newRefinery->setPosition(sf::Vector2f(2100.0f, 2300.0f));
	m_placedEnemyBuildings.push_back(newRefinery);

	sf::Vector2f spawnPosition = sf::Vector2f(2000.0f, 2150.0f);

	/*Harvester* newHarvester = new Harvester();
	newHarvester->setPosition(spawnPosition);
	newHarvester->setTargetPosition(spawnPosition + m_targetPositionOffset);
	newHarvester->setBuildings(m_placedEnemyBuildings);
	newHarvester->setTiles(m_levelEditor.m_tiles);
	newHarvester->m_currentState = newHarvester->MovingToResource;
	newHarvester->m_isEnemy = true;
	m_enemyUnits.push_back(newHarvester);*/

	std::cout << "Enemy Base Initilised" << std::endl;
	updateBuildingCounts();
}

/// <summary>
/// This function is to make the enemy decide to either create a building or a unit
/// </summary>
/// <param name="t_deltaTime"></param>
void Game::updateEnemyAIDecisionOnCreating(sf::Time t_deltaTime)
{
	static sf::Time m_enemyProductionTimer = sf::seconds(0);
	m_enemyProductionTimer += t_deltaTime;
	std::vector<BuildingType> m_availableBuildings;

	// Every few seconds the enemy will try to make either a unit or building
	if (m_enemyProductionTimer > sf::seconds(8))
	{
		updateBuildingCounts();

		if (rand() % 2)
		{
			decideNextEnemyBuilding();
		}
		else
		{
			// Collect all building types that are available for unit production
			if (m_enemyBuildingCounts[BuildingType::Barracks] > 0)
			{
				m_availableBuildings.push_back(BuildingType::Barracks);
			}
			if (m_enemyBuildingCounts[BuildingType::WarFactory] > 0)
			{
				m_availableBuildings.push_back(BuildingType::WarFactory);
			}
			if (m_enemyBuildingCounts[BuildingType::AirCraft] > 0)
			{
				m_availableBuildings.push_back(BuildingType::AirCraft);
			}

			if (!m_availableBuildings.empty())
			{
				// Randomly select an available building type
				BuildingType m_selectedBuilding = m_availableBuildings[rand() % m_availableBuildings.size()];

				// Based on the selected building, decide which unit to create
				switch (m_selectedBuilding)
				{
				case BuildingType::Barracks:
					createEnemyUnit("Rifleman");
					m_enemyUnitStatCount++;
					//std::cout << "Enemy created Rifleman" << std::endl;
					break;
				case BuildingType::WarFactory:
				{
					int unitChance = rand() % 10;
					if (unitChance < 4) // 40% chance for Buggy
					{
						createEnemyUnit("Buggy");
						m_enemyUnitStatCount++;
						//std::cout << "Enemy created Buggy" << std::endl;
					}
					else if (unitChance < 8) // 40% chance for Tank
					{
						createEnemyUnit("TankAurora");
						m_enemyUnitStatCount++;
						//std::cout << "Enemy created Tank" << std::endl;
					}
					else  // 20% chance for Harvester
					{
						createEnemyUnit("Harvester");
						m_enemyUnitStatCount++;
						//std::cout << "Enemy created Harvester" << std::endl;
					}
				}
				break;
				case BuildingType::AirCraft:
				{
					if (rand() % 2) // 50/50 chance
					{
						createEnemyUnit("Hammerhead");
						m_enemyUnitStatCount++;
						//std::cout << "Enemy created Hammerhead" << std::endl;
					}
					else
					{
						createEnemyUnit("Firehawk");
						m_enemyUnitStatCount++;
						//std::cout << "Enemy created Firehawk" << std::endl;
					}
				}
				break;
				default:
					std::cout << "Enemy has no buildings to create a unit" << std::endl;
					break;
				}
			}
			else
			{
				decideNextEnemyBuilding();
				std::cout << "Enemy has no buildings to make a unit so it shall create a building" << std::endl;
			}
		}
		m_enemyProductionTimer = sf::seconds(0);
	}
}

/// <summary>
/// This decides the enemy what to do Explore, group together and attack the player
/// </summary>
/// <param name="t_deltaTime"></param>
void Game::updateEnemyAIUnitDecisionState(sf::Time t_deltaTime)
{
	m_enemyStateTimer += t_deltaTime;

	switch (m_enemyAIState)
	{
	case EnemyAIState::Exploring:
		//std::cout << "Exploring" << std::endl;
		if (m_enemyStateTimer > sf::seconds(30))
		{
			m_enemyAIState = EnemyAIState::Grouping;
			m_enemyStateTimer = sf::seconds(0);
		}
		else
		{
			enemyExploring(t_deltaTime);
		}
		break;
	case EnemyAIState::Grouping:
		//std::cout << "Grouping" << std::endl;
		enemyGroupUnits(t_deltaTime);
		if (m_enemyUnits.size() >= 6)
		{
			m_enemyAIState = EnemyAIState::Attacking;
		}
		break;
	case EnemyAIState::Attacking:
		//std::cout << "Attacking" << std::endl;
		enemyAttacking(t_deltaTime);
		if (m_enemyStateTimer > sf::seconds(30))
		{
			m_enemyAIState = EnemyAIState::Exploring;
			m_enemyStateTimer = sf::seconds(0);
		}
		break;
	}
}

/// <summary>
/// Updates the enemies stuff like buildings, units to bullets and particle effects on them
/// </summary>
/// <param name="t_deltaTime"></param>
void Game::updateEnemyAssets(sf::Time t_deltaTime)
{
	for (Building* enemyBuilding : m_placedEnemyBuildings)
	{
		enemyBuilding->update(t_deltaTime);
	}

	for (Unit* eUnits : m_enemyUnits)
	{
		eUnits->update(t_deltaTime, m_enemyUnits);

		for (auto& bullet : eUnits->m_bullets) // BULLETS
		{
			bool hit = false;

			for (auto& playerUnit : m_playerUnits)
			{
				if (bullet.m_bulletShape.getGlobalBounds().intersects(playerUnit->getSprite().getGlobalBounds()))
				{
					bullet.m_active = false;
					playerUnit->takeDamage(eUnits->getDamage());
					if (playerUnit->getHealth() <= 0)
					{
						playerUnit->m_active = false;
					}
					spawnBulletSparkParticles(bullet.m_position);
				}
			}
			for (auto& playerBuilding : m_placedPlayerBuildings)
			{
				if (bullet.m_bulletShape.getGlobalBounds().intersects(playerBuilding->getBuildingSprite().getGlobalBounds()))
				{
					bullet.m_active = false;
					playerBuilding->takeDamage(eUnits->getDamage());
					hit = true;
					spawnBulletSparkParticles(bullet.m_position);
					break;
				}
			}
			if (hit)
			{
				continue;
			}
		}

		for (auto& missile : eUnits->m_missiles)
		{
			for (auto& playerUnit : m_playerUnits)
			{
				if (missile.m_missileSprite.getGlobalBounds().intersects(playerUnit->getSprite().getGlobalBounds()))
				{
					missile.m_active = false;
					playerUnit->takeDamage(missile.m_damage);
					if (playerUnit->getHealth() <= 0)
					{
						playerUnit->m_active = false;
					}
					spawnExplosionParticle(missile.m_position);
				}
			}
		}
		for (auto& missile : eUnits->m_missiles) // MISSILES
		{
			bool hit = false;

			for (auto& playerUnit : m_playerUnits)
			{
				if (missile.m_missileSprite.getGlobalBounds().intersects(playerUnit->getSprite().getGlobalBounds()))
				{
					missile.m_active = false;
					playerUnit->takeDamage(missile.m_damage);
					if (playerUnit->getHealth() <= 0)
					{
						playerUnit->m_active = false;
					}
					spawnExplosionParticle(missile.m_position);
				}
			}
			for (auto& playerBuilding : m_placedPlayerBuildings)
			{
				if (missile.m_missileSprite.getGlobalBounds().intersects(playerBuilding->getBuildingSprite().getGlobalBounds()))
				{
					missile.m_active = false;
					playerBuilding->takeDamage(eUnits->getDamage());
					hit = true;
					spawnExplosionParticle(missile.m_position);
					break;
				}
			}
			if (hit)
			{
				continue;
			}
		}

		// Remove inactive bullets
		eUnits->m_bullets.erase(std::remove_if(eUnits->m_bullets.begin(), eUnits->m_bullets.end(),
			[](const Bullet& bullet) { return !bullet.m_active; }), eUnits->m_bullets.end());
		// Remove inactive missiles
		eUnits->m_missiles.erase(std::remove_if(eUnits->m_missiles.begin(), eUnits->m_missiles.end(),
			[](const Missile& missile) { return !missile.m_active && missile.m_trail.empty(); }), eUnits->m_missiles.end());

		// Remove dead enemy units
		m_playerUnits.erase(std::remove_if(m_playerUnits.begin(), m_playerUnits.end(),
			[](Unit* playerUnits) { return !playerUnits->isActive(); }), m_playerUnits.end());
		// Remove buildings
		m_placedPlayerBuildings.erase(std::remove_if(m_placedPlayerBuildings.begin(), m_placedPlayerBuildings.end(),
			[](Building* building) { return building->getHealth() <= 0; }), m_placedPlayerBuildings.end());
	}
}

/// <summary>
/// Creates the specific unit type on that buildings position
/// </summary>
/// <param name="unitType"></param>
void Game::createEnemyUnit(const std::string& m_unitType)
{
	sf::Vector2f m_spawnPosition = sf::Vector2f(0, 0);
	sf::Vector2f m_targetPositionOffset = sf::Vector2f(0, 60);
	bool m_validBuildingFound = false;

	// Find a building of the appropriate type and use its position
	for (const auto& building : m_placedEnemyBuildings)
	{
		if (m_unitType == "Rifleman" && dynamic_cast<Barracks*>(building) != nullptr)
		{
			m_spawnPosition = building->getPosition();
			m_validBuildingFound = true;
			break;
		}
		else if ((m_unitType == "Harvester" || m_unitType == "Buggy" || m_unitType == "TankAurora") && dynamic_cast<WarFactory*>(building) != nullptr)
		{
			m_spawnPosition = building->getPosition();
			m_validBuildingFound = true;
			break;
		}
		else if ((m_unitType == "Hammerhead" || m_unitType == "Firehawk") && dynamic_cast<AirCraft*>(building) != nullptr)
		{
			m_spawnPosition = building->getPosition();
			m_validBuildingFound = true;	
			break;
		}
	}

	if (!m_validBuildingFound)
	{
		std::cerr << "No building found for creating unit: " << m_unitType << std::endl;
		return;
	}

	if (m_unitType == "Rifleman") // Infantry units
	{
		RiflemanSquad* newRiflemanSquad = new RiflemanSquad();
		newRiflemanSquad->setPosition(m_spawnPosition);
		newRiflemanSquad->setTargetPosition(m_spawnPosition + m_targetPositionOffset);
		newRiflemanSquad->setEnemyUnits(m_playerUnits);
		newRiflemanSquad->setEnemyBuildings(m_placedPlayerBuildings);
		newRiflemanSquad->setTiles(m_levelEditor.m_tiles);

		m_enemyUnits.push_back(newRiflemanSquad);
	}
	else if (m_unitType == "Harvester") // Vehicle units
	{
		Harvester* newHarvester = new Harvester();
		newHarvester->setPosition(m_spawnPosition);
		newHarvester->setTargetPosition(m_spawnPosition + m_targetPositionOffset);
		newHarvester->setBuildings(m_placedEnemyBuildings);
		newHarvester->setTiles(m_levelEditor.m_tiles);
		newHarvester->m_isEnemy = true;
		newHarvester->m_currentState = newHarvester->MovingToResource;

		m_enemyUnits.push_back(newHarvester);
	}
	else if (m_unitType == "Buggy")
	{
		Buggy* newBuggy = new Buggy();
		newBuggy->setPosition(m_spawnPosition);
		newBuggy->setTargetPosition(m_spawnPosition + m_targetPositionOffset);
		newBuggy->setEnemyUnits(m_playerUnits);
		newBuggy->setEnemyBuildings(m_placedPlayerBuildings);
		newBuggy->setTiles(m_levelEditor.m_tiles);

		m_enemyUnits.push_back(newBuggy);
	}
	else if (m_unitType == "TankAurora")
	{
		TankAurora* newTankAurora = new TankAurora();
		newTankAurora->setPosition(m_spawnPosition);
		newTankAurora->setTargetPosition(m_spawnPosition + m_targetPositionOffset);
		newTankAurora->setEnemyUnits(m_playerUnits);
		newTankAurora->setTiles(m_levelEditor.m_tiles);

		m_enemyUnits.push_back(newTankAurora);
	}
	else if (m_unitType == "Hammerhead")
	{
		HammerHead* newHammerHead = new HammerHead();
		newHammerHead->setPosition(m_spawnPosition);
		newHammerHead->setTargetPosition(m_spawnPosition + m_targetPositionOffset);
		newHammerHead->setEnemyUnits(m_playerUnits);
		newHammerHead->setEnemyBuildings(m_placedPlayerBuildings);

		m_enemyUnits.push_back(newHammerHead);
	}
	else if (m_unitType == "Firehawk")
	{
		Firehawk* newFirehawk = new Firehawk();
		newFirehawk->setPosition(m_spawnPosition);
		newFirehawk->setTargetPosition(m_spawnPosition + m_targetPositionOffset);
		newFirehawk->setEnemyUnits(m_playerUnits);
		newFirehawk->setEnemyBuildings(m_placedPlayerBuildings);

		m_enemyUnits.push_back(newFirehawk);
	}
}

/// <summary>
/// This create the enemy harvester unit this goes along side with the refinery building
/// </summary>
void Game::createEnemyHarvesterUnit()
{
	sf::Vector2f targetPositionOffset = sf::Vector2f(0, 60);

	Harvester* newHarvester = new Harvester();
	newHarvester->setPosition(enemyRefineryBuildingPosition);
	newHarvester->setTargetPosition(enemyRefineryBuildingPosition + targetPositionOffset);
	newHarvester->setBuildings(m_placedEnemyBuildings);
	newHarvester->setTiles(m_levelEditor.m_tiles);
	newHarvester->m_isEnemy = true;
	newHarvester->m_currentState = newHarvester->MovingToResource;

	m_enemyUnits.push_back(newHarvester);
}

/// <summary>
/// Checks how many buildings it has
/// </summary>
void Game::updateBuildingCounts()
{
	for (auto& count : m_enemyBuildingCounts)
	{
		count.second = 0;
	}

	m_enemyBuildingCounts =
	{
		{BuildingType::Refinery, 0},
		{BuildingType::Headquarters, 0},
		{BuildingType::Barracks, 0},
		{BuildingType::WarFactory, 0},
		{BuildingType::AirCraft, 0}
	};

	// Iterate over all placed enemy buildings and increment the count for each type
	for (const auto& building : m_placedEnemyBuildings)
	{
		if (dynamic_cast<Refinery*>(building) != nullptr)
		{
			m_enemyBuildingCounts[BuildingType::Refinery]++;
		}
		else if (dynamic_cast<Headquarters*>(building) != nullptr) 
		{
			m_enemyBuildingCounts[BuildingType::Headquarters]++;
		}
		else if (dynamic_cast<Barracks*>(building) != nullptr)
		{
			m_enemyBuildingCounts[BuildingType::Barracks]++;
		}
		else if (dynamic_cast<WarFactory*>(building) != nullptr)
		{
			m_enemyBuildingCounts[BuildingType::WarFactory]++;
		}
		else if (dynamic_cast<AirCraft*>(building) != nullptr)
		{
			m_enemyBuildingCounts[BuildingType::AirCraft]++;
		}
	}
}

/// <summary>
/// This is to decie which building it should be placing
/// </summary>
void Game::decideNextEnemyBuilding()
{
	updateBuildingCounts();

	const int maxRefineries = 2;
	const int maxBarracks = 2;
	const int maxWarFactories = 2;
	const int maxAirCrafts = 2;

	if (m_enemyBuildingCounts[BuildingType::Refinery] < maxRefineries)
	{
		placeEnemyBuilding(BuildingType::Refinery);
	}
	else if (m_enemyBuildingCounts[BuildingType::Barracks] < maxBarracks)
	{
		placeEnemyBuilding(BuildingType::Barracks);
	}
	else if (m_enemyBuildingCounts[BuildingType::WarFactory] < maxWarFactories)
	{
		placeEnemyBuilding(BuildingType::WarFactory);
	}
	else if (m_enemyBuildingCounts[BuildingType::AirCraft] < maxAirCrafts)
	{
		placeEnemyBuilding(BuildingType::AirCraft);
	}
}

/// <summary>
/// Decides which building type to place
/// </summary>
/// <param name="m_type"></param>
void Game::placeEnemyBuilding(BuildingType m_type)
{
	sf::Vector2f m_position = findPlacementPositionNearExistingBuilding();
	bool m_createHarvesterUnit = false;

	switch (m_type)
	{
	case BuildingType::Refinery:
		newEnemyBuilding = new Refinery();
		enemyRefineryBuildingPosition = m_position;
		m_createHarvesterUnit = true;
		break;
	case BuildingType::Barracks:
		newEnemyBuilding = new Barracks();
		break;
	case BuildingType::WarFactory:
		newEnemyBuilding = new WarFactory();
		break;
	case BuildingType::AirCraft:
		newEnemyBuilding = new AirCraft();
		break;
	default:
		break;
	}

	if (newEnemyBuilding)
	{
		newEnemyBuilding->setPosition(m_position);
		m_placedEnemyBuildings.push_back(newEnemyBuilding);
		m_enemyBuildingStatCount++;
	}
	if (m_createHarvesterUnit)
	{
		createEnemyHarvesterUnit();
	}
}

/// <summary>
/// Find a suitable place to create a building
/// </summary>
/// <returns></returns>
sf::Vector2f Game::findPlacementPositionNearExistingBuilding()
{
	const int m_maxAttempts = 500; // Limit the number of attempts to avoid an infinite loop
	int m_attempts = 0;
	bool m_intersectsBuilding = false;
	bool m_intersectsWallTile = false;
	float m_buildingWidth = 100.0f;
	float m_buildingHeight = 100.0f;
	sf::FloatRect m_mapBounds(100, 100, m_mapWidth, m_mapHeight);

	while (m_attempts < m_maxAttempts)
	{
		m_attempts++;

		if (m_placedEnemyBuildings.empty()) 
		{
			std::cout << "No enemy buildings found :( what the hell" << std::endl;
		}

		int m_index = rand() % m_placedEnemyBuildings.size();
		Building* baseBuilding = m_placedEnemyBuildings[m_index];

		// Calculate a random position within the placement radius of the selected building
		float m_radius = baseBuilding->getPlacementRadius().getRadius();
		sf::Vector2f m_centerPosition = baseBuilding->getPosition();
		float m_angle = static_cast<float>(rand() % 360) * (3.14159f / 180.0f);
		float m_distanceFromCenter = static_cast<float>(rand() % static_cast<int>(m_radius));
		sf::Vector2f m_newPos = m_centerPosition + sf::Vector2f(m_distanceFromCenter * cos(m_angle), m_distanceFromCenter * sin(m_angle));
		//sf::FloatRect newBuildingRect = baseBuilding->getBuildingSprite().getGlobalBounds();
		sf::FloatRect m_buildingBounds(m_newPos.x - m_buildingWidth / 2, m_newPos.y - m_buildingHeight / 2, m_buildingWidth, m_buildingHeight);

		bool m_isInsideMap = m_mapBounds.contains(m_buildingBounds.left, m_buildingBounds.top) &&
			m_mapBounds.contains(m_buildingBounds.left + m_buildingBounds.width, m_buildingBounds.top + m_buildingBounds.height);

		// Check for intersection with existing buildings
		m_intersectsBuilding = false;
		for (const auto& building : m_placedEnemyBuildings) 
		{
			sf::FloatRect existingBuildingRect = building->getBuildingSprite().getGlobalBounds();

			if (m_buildingBounds.intersects(existingBuildingRect))
			{
				m_intersectsBuilding = true;
				break;
			}
		}

		// Check for intersection with wall tiles
		m_intersectsWallTile = false;
		for (const auto& row : m_levelEditor.m_tiles)
		{
			for (const auto& tile : row)
			{
				if (tile.m_isWall)
				{
					sf::FloatRect wallTileRect = tile.m_tile.getGlobalBounds();

					if (m_buildingBounds.intersects(wallTileRect))
					{
						m_intersectsWallTile = true;
						break;
					}
				}
			}
			if (m_intersectsWallTile) 
			{
				break;
			}
		}

		if (!m_intersectsBuilding && !m_intersectsWallTile && m_isInsideMap) 
		{
			std::cout << m_newPos.x << " " << m_newPos.y << std::endl;
			return m_newPos;
		}
	}
	std::cout << "Could not find a valid position after " << m_maxAttempts << " attempts" << std::endl;
	return sf::Vector2f(-1, -1);
}

/// <summary>
/// Moves the enemies towards the player units or buildings
/// </summary>
void Game::moveEnemyUnits()
{
	for (auto& enemyUnit : m_enemyUnits) 
	{
		if (!enemyUnit)
		{ 
			continue;
		}
		if (dynamic_cast<Harvester*>(enemyUnit) != nullptr)
		{
			continue; // To ignore the Harvester units
		}

		// Find the nearest player unit or building position
		sf::Vector2f targetPos = findNearestPlayerObjectPosition(enemyUnit->getPosition());
		enemyUnit->moveTo(targetPos);
	}
}

sf::Vector2f Game::findNearestPlayerObjectPosition(const sf::Vector2f& m_enemyPosition)
{
	sf::Vector2f nearestTargetPosition;
	float nearestDistanceSquared = std::numeric_limits<float>::max();

	for (const auto& playerUnit : m_playerUnits) 
	{
		float distanceSquared = distanceBetweenPoints(m_enemyPosition, playerUnit->getPosition());
		if (distanceSquared < nearestDistanceSquared) 
		{
			nearestDistanceSquared = distanceSquared;
			nearestTargetPosition = playerUnit->getPosition();
		}
	}

	for (const auto& building : m_placedPlayerBuildings)
	{
		float distanceSquared = distanceBetweenPoints(m_enemyPosition, building->getPosition());
		if (distanceSquared < nearestDistanceSquared)
		{
			nearestDistanceSquared = distanceSquared;
			nearestTargetPosition = building->getPosition();
		}
	}

	return nearestTargetPosition;
}

float Game::distanceBetweenPoints(const sf::Vector2f& p1, const sf::Vector2f& p2)
{
	return (p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y);
}

/// <summary>
/// Makes the enemies explore the map
/// </summary>
/// <param name="t_deltaTime"></param>
void Game::enemyExploring(sf::Time t_deltaTime)
{
	static sf::Time m_exploreTimer = sf::Time::Zero;
	const sf::Time m_exploreInterval = sf::seconds(8);

	m_exploreTimer += t_deltaTime;

	if (m_exploreTimer >= m_exploreInterval)
	{
		m_exploreTimer -= m_exploreInterval;
		auto validTargets = getValidExplorationTargets(); // check for valid targets to explore

		if (!validTargets.empty()) 
		{
			for (auto& enemyUnit : m_enemyUnits)
			{
				if (!enemyUnit)
				{
					continue;
				}
				// Skip Harvester units from exploring
				if (dynamic_cast<Harvester*>(enemyUnit) != nullptr)
				{
					continue;
				}

				// Select a random target position from the valid targets
				int m_randomIndex = rand() % validTargets.size();
				sf::Vector2f m_targetPos = validTargets[m_randomIndex];
				//std::cout << m_targetPos.x << " " << m_targetPos.y << std::endl;
				enemyUnit->moveTo(m_targetPos);
			}
		}
	}
}

/// <summary>
/// Makes a group of enemies come together
/// </summary>
/// <param name="t_deltaTime"></param>
void Game::enemyGroupUnits(sf::Time t_deltaTime)
{
	static sf::Time m_groupUnitsTimer = sf::Time::Zero;
	const sf::Time m_groupInterval = sf::seconds(6);

	m_groupUnitsTimer += t_deltaTime;

	sf::Vector2f m_headquartersPos = findEnemyHeadquartersPosition(); // to make the rally point to be beside the enemy base 
	sf::Vector2f m_rallyPoint = m_headquartersPos + sf::Vector2f(-200, -200); 

	if (m_groupUnitsTimer >= m_groupInterval)
	{
		for (auto& enemyUnit : m_enemyUnits)
		{
			if (!enemyUnit)
			{
				continue;
			}
			// Exclude non-combat units like the Harvester from being grouped
			if (dynamic_cast<Harvester*>(enemyUnit) == nullptr)
			{
				enemyUnit->moveTo(m_rallyPoint);
			}
		}
		m_groupUnitsTimer -= m_groupInterval;
	}
}

/// <summary>
/// orders the enemy to attack the player units/buildings
/// </summary>
/// <param name="t_deltaTime"></param>
void Game::enemyAttacking(sf::Time t_deltaTime)
{
	static sf::Time m_moveUnitsTimer = sf::Time::Zero;
	const sf::Time m_moveInterval = sf::seconds(2);

	m_moveUnitsTimer += t_deltaTime;

	if (m_moveUnitsTimer >= m_moveInterval)
	{
		moveEnemyUnits(); // Move enemy units
		m_moveUnitsTimer -= m_moveInterval;
	}
}

std::vector<sf::Vector2f> Game::getValidExplorationTargets()
{
	for (const auto& row : m_levelEditor.m_tiles) 
	{
		for (const auto& tile : row) 
		{
			if (!tile.m_isWall)
			{
				sf::Vector2f position = tile.m_tile.getPosition();
				m_validTargets.push_back(position);
			}
		}
	}

	return m_validTargets;
}

sf::Vector2f Game::findEnemyHeadquartersPosition() 
{
	for (const auto& building : m_placedEnemyBuildings) 
	{
		if (dynamic_cast<Headquarters*>(building) != nullptr) 
		{
			return building->getPosition();
		}
	}

	return sf::Vector2f(1900.0f, 2100.0f);
}

/// <summary>
/// Just in case i wont get the runtime error if a building or a unit is outside the map
/// so this function makes sure its within bounds
/// </summary>
/// <param name="position"></param>
/// <returns></returns>
bool Game::checkPositionWithinMap(const sf::Vector2f& m_position)
{
	const float mapLeft = 0;
	const float mapTop = 0;
	const float mapRight = 2500; 
	const float mapBottom = 2500; 

	return m_position.x >= mapLeft && m_position.x <= mapRight &&
		m_position.y >= mapTop && m_position.y <= mapBottom;
}

/// <summary>
/// To reset the game and undo all the properties for a new game
/// </summary>
void Game::gameReset()
{
	resetView();
	m_viewCenter = sf::Vector2f(Global::S_WIDTH / 2, Global::S_HEIGHT / 2);
	m_levelEditor.resetFogOfWar();
	m_gameWinLose = WinLoseState::NONE;
	Global::playerCurrency = 5000;
	Global::enemyCurrency = 10000;
	m_playerUnitStatCount = 0;
	m_playerBuildingStatCount = 0;
	m_enemyUnitStatCount = 0;
	m_enemyBuildingStatCount = 0;
	m_gameDurationClock.restart();
	clearGameEntities();
	createBase();
	createEnemyStarterBase();
}

/// <summary>
/// Clears all game enities such as units and buildings
/// </summary>
void Game::clearGameEntities()
{
	for (auto building : m_placedPlayerBuildings) 
	{
		delete building; 
	}
	m_placedPlayerBuildings.clear();

	for (auto building : m_placedEnemyBuildings) 
	{
		delete building;
	}
	m_placedEnemyBuildings.clear();

	for (auto unit : m_playerUnits)
	{
		delete unit;
	}
	m_playerUnits.clear();

	for (auto unit : m_enemyUnits)
	{
		delete unit;
	}
	m_enemyUnits.clear();
}
