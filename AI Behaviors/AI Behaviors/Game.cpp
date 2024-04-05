#include "Game.h"

/// <summary>
/// default constructor
/// </summary>
Game::Game() :
	m_window{ sf::VideoMode{ Global::S_WIDTH, Global::S_HEIGHT, 32U }, "Gills & Glory" },
	m_exitGame{ false },
	m_neural_network(NeuralNetworks())
{
	gameView.setSize(sf::Vector2f(Global::S_WIDTH, Global::S_HEIGHT));
	gameView.setCenter(Global::S_WIDTH / 2, Global::S_HEIGHT / 2);
	m_window.setView(gameView);
	m_window.setMouseCursorVisible(false);

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

	createBase();
	createEnemyStarterBase();

	m_levelEditor.resetFogOfWar();

	selectionBox.setFillColor(sf::Color(0, 255, 0, 50));
	selectionBox.setOutlineColor(sf::Color(0, 255, 0));
	selectionBox.setOutlineThickness(1.0f);

	hiddenNeurons = m_neural_network.getHiddenNeurons();
	biasNeurons = m_neural_network.getBiasNeurons();
	initializeNeuralNetwork();
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
				train = !train;
			}
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
				if (m_gameWinLose != WinLoseState::NONE)
				{
					if (newEvent.mouseButton.button == sf::Mouse::Left) // Check for left mouse button
					{
						m_gui.handleMouseClick(guiMousePosition, m_window);
						if (m_selectedUnit)
						{
							std::vector<Unit*> selectedUnits;
							for (Unit* unit : playerUnits)
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
						for (Unit* unit : playerUnits)
						{
							unit->setSelected(false);
						}
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
		case GameState::NeuralNetworks:
			if (sf::Event::MouseButtonPressed == newEvent.type)
			{
				if (newEvent.mouseButton.button == sf::Mouse::Left)
				{
					m_neural_network.goToMainMenu(guiMousePosition, m_currentState);

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
			for (Unit* unit : playerUnits)
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
		if (m_gameWinLose != WinLoseState::NONE)
		{
			loadLevel(m_levelLoader.levelFilenames[m_levelLoader.selectedButtonIndex]);
			updateViewWithMouse();
			m_gui.update(t_deltaTime);
			m_gui.handleBuildingPlacement(mouseGUIPosition, m_window);
			createBuilding(m_window);
			createUnit();
			updatePlayerAssets(t_deltaTime);
			updateEnemyAssets(t_deltaTime);
			updateFogOfWarBasedOnUnits(playerUnits);
			updateFogOfWarBasedOnBuildings(placedPlayerBuildings);
			spawnBubbleParticles();
			updateEnemyAIDecisionOnCreating(t_deltaTime);
			updateEnemyAIUnitDecisionState(t_deltaTime);
		}
		m_levelEditor.animationForResources();
		m_levelEditor.animationForWeed();
		m_particleSystem.update(t_deltaTime);
		checkVictoryConditions();
		break;
	case GameState::LevelEditor:
		m_previousState = GameState::LevelEditor;
		updateViewWithMouse();
		m_levelEditor.update(t_deltaTime, m_window);
		break;
	case GameState::LevelLoad:
		loadLevel(m_levelLoader.levelFilenames[m_levelLoader.selectedButtonIndex]);
		break;
	case GameState::NeuralNetworks:
		updateNeuralNetwork();
		errorText.setString("Total Error: " + std::to_string(total_error));
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
		for (Building* playerBuilding : placedPlayerBuildings)
		{
			playerBuilding->render(m_window);
		}
		for (Building* enemyBuilding : placedEnemyBuildings)
		{
			sf::Vector2f buildingPosition = enemyBuilding->getPosition();
			if (isPositionWithinMap(buildingPosition))
			{
				int tileX = static_cast<int>(buildingPosition.x / m_tiles.tileSize);
				int tileY = static_cast<int>(buildingPosition.y / m_tiles.tileSize);

				if (m_levelEditor.m_tiles[tileY][tileX].fogStatus == Tile::FogStatus::Visible)
				{
					enemyBuilding->render(m_window);
				}
			}
		}
		for (Unit* unit : playerUnits)
		{
			unit->render(m_window);
		}
		for (Unit* eUnit : enemyUnits)
		{
			sf::Vector2f unitPosition = eUnit->getPosition();
			if (isPositionWithinMap(unitPosition))
			{
				int tileX = static_cast<int>(unitPosition.x / m_tiles.tileSize);
				int tileY = static_cast<int>(unitPosition.y / m_tiles.tileSize);

				if (m_levelEditor.m_tiles[tileY][tileX].fogStatus == Tile::FogStatus::Visible)
				{
					eUnit->render(m_window);
				}
			}
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
	case GameState::NeuralNetworks:
		drawNeuralNetwork(m_window);
		m_neural_network.render(m_window);
		m_window.draw(errorText);
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
			placedPlayerBuildings.push_back(newRefinery);
			sf::Vector2f buildingPosition = m_gui.m_selectedBuilding->getPosition();

			sf::Vector2f spawnPosition = worldMousePosition + sf::Vector2f(0.0f, 60.0f);
			Harvester* newHarvester = new Harvester();
			newHarvester->setPosition(spawnPosition);
			newHarvester->setTargetPosition(spawnPosition);
			newHarvester->setBuildings(placedPlayerBuildings);
			newHarvester->setTiles(m_levelEditor.m_tiles);
			newHarvester->currentState = newHarvester->MovingToResource;
			playerUnits.push_back(newHarvester);

			std::cout << "Refinery Created" << std::endl;
		}
		else if (m_selectedBuildingType == BuildingType::Barracks)
		{
			Barracks* newBarracks = new Barracks();
			newBarracks->setPosition(worldMousePosition);
			placedPlayerBuildings.push_back(newBarracks);
			std::cout << "Barracks Created" << std::endl;
		}
		else if (m_selectedBuildingType == BuildingType::WarFactory)
		{
			WarFactory* newVehicle = new WarFactory();
			newVehicle->setPosition(worldMousePosition);
			placedPlayerBuildings.push_back(newVehicle);
			std::cout << "Vehicle Created" << std::endl;
		}
		else if (m_selectedBuildingType == BuildingType::AirCraft)
		{
			AirCraft* newAirCraft = new AirCraft();
			newAirCraft->setPosition(worldMousePosition);
			placedPlayerBuildings.push_back(newAirCraft);
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
	placedPlayerBuildings.push_back(newHeadquarters);
	std::cout << "Base Initilised" << std::endl;
}

/// <summary>
/// Updates the players buildings, units and the weapons for the unit.
/// </summary>
/// <param name="t_deltaTime"></param>
void Game::updatePlayerAssets(sf::Time t_deltaTime)
{
	for (Building* playerBuilding : placedPlayerBuildings)
	{
		playerBuilding->update(t_deltaTime);
	}

	for (auto& unit : playerUnits)
	{
		unit->update(t_deltaTime, playerUnits);

		for (auto& bullet : unit->bullets) // BULLETS
		{
			bool hit = false;

			for (auto& enemyUnit : enemyUnits)
			{
				if (bullet.bulletShape.getGlobalBounds().intersects(enemyUnit->getSprite().getGlobalBounds()))
				{
					bullet.active = false;
					enemyUnit->takeDamage(unit->getDamage());
					if (enemyUnit->getHealth() <= 0)
					{
						enemyUnit->m_active = false;
					}
					spawnBulletSparkParticles(bullet.position);
				}
			}

			for (auto& enemyBuilding : placedEnemyBuildings)
			{
				if (bullet.bulletShape.getGlobalBounds().intersects(enemyBuilding->getBuildingSprite().getGlobalBounds()))
				{
					bullet.active = false;
					enemyBuilding->takeDamage(unit->getDamage());
					hit = true;
					spawnBulletSparkParticles(bullet.position);
					break;
				}
			}
			if (hit)
			{
				continue;
			}
		}
		for (auto& missile : unit->missiles) // MISSILES
		{
			bool hit = false;

			for (auto& enemyUnit : enemyUnits)
			{
				if (missile.missileSprite.getGlobalBounds().intersects(enemyUnit->getSprite().getGlobalBounds()))
				{
					missile.active = false;
					enemyUnit->takeDamage(missile.damage);
					if (enemyUnit->getHealth() <= 0)
					{
						enemyUnit->m_active = false;
					}
					spawnExplosionParticle(missile.position);
				}
			}
			for (auto& enemyBuilding : placedEnemyBuildings)
			{
				if (missile.missileSprite.getGlobalBounds().intersects(enemyBuilding->getBuildingSprite().getGlobalBounds()))
				{
					missile.active = false;
					enemyBuilding->takeDamage(unit->getDamage());
					hit = true;
					spawnExplosionParticle(missile.position);
					break;
				}
			}
			if (hit)
			{
				continue;
			}
		}

		// Remove inactive bullets
		unit->bullets.erase(std::remove_if(unit->bullets.begin(), unit->bullets.end(),
			[](const Bullet& bullet) { return !bullet.active; }), unit->bullets.end());
		// Remove inactive missiles
		unit->missiles.erase(std::remove_if(unit->missiles.begin(), unit->missiles.end(),
			[](const Missile& missile) { return !missile.active && missile.trail.empty(); }), unit->missiles.end());

		// Remove dead enemy units
		enemyUnits.erase(std::remove_if(enemyUnits.begin(), enemyUnits.end(),
			[](Unit* enemyUnit) { return !enemyUnit->isActive(); }), enemyUnits.end());
		// Remove buildings
		placedEnemyBuildings.erase(std::remove_if(placedEnemyBuildings.begin(), placedEnemyBuildings.end(),
			[](Building* building) { return building->getHealth() <= 0; }), placedEnemyBuildings.end());
	}
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

void Game::initializeNeuralNetwork()
{
	errorText.setFont(m_normalfont);
	errorText.setCharacterSize(20);
	errorText.setFillColor(sf::Color::White);
	errorText.setPosition(400, 10);

	neural_network.resize(2 + hiddenNeurons.size());
	neural_network[0].resize(m_neural_network.INPUT_NEURONS + biasNeurons[0], 0);
	neural_network.back().resize(m_neural_network.OUTPUT_NEURONS, 0);

	random_engine.seed(std::chrono::system_clock::now().time_since_epoch().count());
	std::uniform_real_distribution<float> distribution(-1, 1);

	weights.resize(neural_network.size() - 1);

	outputImage.create(outputWidth, outputHeight);
	outputTexture.loadFromImage(outputImage);
	outputSprite.setPosition(SCREEN_HEIGHT, 0);
	//outputs_sprite.setScale(15, 17);
	outputSprite.setScale(SCREEN_HEIGHT / outputWidth, SCREEN_WITDH / outputHeight);
	outputSprite.setOrigin(outputSprite.getGlobalBounds().width / 2, outputSprite.getGlobalBounds().height / 2);
	outputSprite.setTexture(outputTexture);

	for (size_t i = 1; i < neural_network.size() - 1; ++i)
	{
		neural_network[i].resize(biasNeurons[i] + hiddenNeurons[i - 1], 0);
	}

	for (size_t i = 0; i < weights.size(); ++i)
	{
		size_t next_layer_size = neural_network[i + 1].size() - (i < weights.size() - 1 ? biasNeurons[i + 1] : 0);
		weights[i].resize(next_layer_size, vector_1d(neural_network[i].size()));

		for (auto& weight_layer : weights[i])
		{
			for (float& weight : weight_layer)
			{
				weight = distribution(random_engine);
			}
		}
	}

	errors.resize(weights.size());

	for (size_t i = 0; i < errors.size(); ++i)
	{
		errors[i].resize(weights[i].size(), 0);
	}
}

void Game::updateNeuralNetwork()
{
	if (train)	// Training the neural network
	{
		total_error = 0;

		for (int i = 0; i < TRAININGS_PER_FRAME; i++)
		{
			int input_index = rand() % m_neural_network.getInputs().size();

			m_neural_network.forwardPropagation(true, m_neural_network.getInputs()[input_index], neural_network, weights);
			m_neural_network.backPropagation(m_neural_network.getTargetOutputs()[input_index], errors, neural_network, weights);
		}

		for (int i = 0; i < m_neural_network.getInputs().size(); i++)
		{
			vector_1d outputs = m_neural_network.forwardPropagation(false, m_neural_network.getInputs()[i], neural_network, weights);

			for (int j = 0; j < outputs.size(); j++)
			{
				total_error += std::abs(outputs[j] - m_neural_network.getTargetOutputs()[i][j]);
			}
		}
	}
	outputTexture.update(outputImage);
}

void Game::drawNeuralNetwork(sf::RenderWindow& m_window)
{
	m_neural_network.draw_neural_network(m_window, neural_network, weights);

	for (int i = 0; i < outputWidth; i++)
	{
		for (int j = 0; j < outputHeight; j++)
		{
			float input_1 = i / static_cast<float>(outputWidth);
			float input_2 = j / static_cast<float>(outputHeight);

			std::vector output_color = m_neural_network.forwardPropagation(0, { input_1, input_2 }, neural_network, weights);

			outputImage.setPixel(i, j, sf::Color(round(255 * output_color[0]), round(255 * output_color[1]), round(255 * output_color[2])));
		}
	}

	m_window.draw(outputSprite);
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
		sf::Vector2f targetPositionOffset = sf::Vector2f(0, 50);

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
				newRiflemanSquad->setEnemyUnits(enemyUnits);
				newRiflemanSquad->setEnemyBuildings(placedEnemyBuildings);

				playerUnits.push_back(newRiflemanSquad);
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
				newHarvester->setBuildings(placedPlayerBuildings);
				newHarvester->setTiles(m_levelEditor.m_tiles);
				newHarvester->currentState = newHarvester->MovingToResource;

				playerUnits.push_back(newHarvester);
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
				newBuggy->setEnemyUnits(enemyUnits);
				newBuggy->setEnemyBuildings(placedEnemyBuildings);

				playerUnits.push_back(newBuggy);
			}
		}
		if (m_gui.m_selectedVehicleType == VehicleType::Tank)
		{
			if (m_gui.m_selectedBuilding)
			{
				sf::Vector2f buildingPosition = m_gui.m_selectedBuilding->getPosition();
				sf::Vector2f spawnPosition = buildingPosition + sf::Vector2f(0.0f, 60.0f);

				TankAurora* newTankAurora = new TankAurora();
				newTankAurora->setPosition(spawnPosition);
				newTankAurora->setTargetPosition(spawnPosition + targetPositionOffset);
				newTankAurora->setEnemyUnits(enemyUnits);

				playerUnits.push_back(newTankAurora);
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
				newHammerHead->setEnemyUnits(enemyUnits);
				newHammerHead->setEnemyBuildings(placedEnemyBuildings);

				playerUnits.push_back(newHammerHead);
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
				newFirehawk->setEnemyUnits(enemyUnits);
				newFirehawk->setEnemyBuildings(placedEnemyBuildings);

				playerUnits.push_back(newFirehawk);
			}
		}
		m_gui.m_unitConfirmed = false;
		m_gui.m_selectedBuilding = nullptr;
		m_gui.m_selectedBuildingType = BuildingType::None;
		m_gui.m_selectedInfantryType = InfantryType::None;
		m_gui.m_selectedVehicleType = VehicleType::None;
		m_gui.m_selectedAircraftType = AirCraftType::None;
	}
}

void Game::selectUnitAt(const sf::Vector2f& mousePos)
{
	for (Unit* unit : playerUnits)
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

	for (Unit* unit : playerUnits)
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

void Game::spawnExplosionParticle(const sf::Vector2f& position) 
{
	int particleCount = 30;

	std::vector<sf::Color> colors =
	{
		sf::Color::Red,  
		sf::Color::Yellow, 
		sf::Color(255, 165, 0), 
		sf::Color(255, 140, 0), 
		sf::Color(128, 128, 128),
		sf::Color(80, 80, 80), 
		sf::Color::Black 
	};

	for (int i = 0; i < particleCount; ++i) 
	{
		float angle = (std::rand() % 360) * 3.14f / 180.0f;
		float speed = static_cast<float>(std::rand() % 50 + 50); 
		sf::Vector2f velocity(speed * std::cos(angle), speed * std::sin(angle));
		sf::Color color = colors[std::rand() % colors.size()];

		float size = static_cast<float>(std::rand() % 2 + 1);
		float lifetime = 0.1f + static_cast<float>(std::rand() % 10) / 10.0f;

		m_particleSystem.addParticle(Particle(position, velocity, color, lifetime, size));
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
	m_levelLoader.initLevelSelectionButtons();
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

void Game::loadLevel(const std::string& filename)
{
	if (!levelLoaded)
	{
		m_levelEditor.loadLevelFromFile("Assets\\SaveFiles\\" + filename + ".txt");
		levelLoaded = true;
	}
}

/// <summary>
/// Checks the victory conditions
/// </summary>
void Game::checkVictoryConditions()
{
	if (placedEnemyBuildings.empty())
	{
		m_gameWinLose = WinLoseState::WIN;
		std::cout << "Player has Won the game HEEEEHAAAA" << std::endl;
	}
	else if (placedPlayerBuildings.empty())
	{
		m_gameWinLose = WinLoseState::LOSE;
		std::cout << "Player has Lost the game :(" << std::endl;
	}
}

void Game::createEnemyStarterBase()
{
	sf::Vector2f targetPositionOffset = sf::Vector2f(0, 60);

	Headquarters* newHeadquarters = new Headquarters();
	newHeadquarters->setPosition(sf::Vector2f(1900.0f, 2100.0f));
	placedEnemyBuildings.push_back(newHeadquarters);

	Refinery* newRefinery = new Refinery();
	newRefinery->setPosition(sf::Vector2f(2100.0f, 2300.0f));
	placedEnemyBuildings.push_back(newRefinery);

	sf::Vector2f spawnPosition = sf::Vector2f(2000.0f, 2150.0f);

	Harvester* newHarvester = new Harvester();
	newHarvester->setPosition(spawnPosition);
	newHarvester->setTargetPosition(spawnPosition + targetPositionOffset);
	newHarvester->setBuildings(placedEnemyBuildings);
	newHarvester->setTiles(m_levelEditor.m_tiles);
	newHarvester->currentState = newHarvester->MovingToResource;
	newHarvester->m_isEnemy = true;
	enemyUnits.push_back(newHarvester);

	std::cout << "Enemy Base Initilised" << std::endl;
	updateBuildingCounts();
}

/// <summary>
/// This function is to make the enemy decide to either create a building or a unit
/// </summary>
/// <param name="t_deltaTime"></param>
void Game::updateEnemyAIDecisionOnCreating(sf::Time t_deltaTime)
{
	static sf::Time enemyProductionTimer = sf::seconds(0);
	enemyProductionTimer += t_deltaTime;
	std::vector<BuildingType> availableBuildings;

	// Every few seconds the enemy will try to make either a unit or building
	if (enemyProductionTimer > sf::seconds(80))
	{
		updateBuildingCounts();

		if (rand() % 2)
		{
			decideNextEnemyBuilding();
		}
		else
		{
			// Collect all building types that are available for unit production
			if (enemyBuildingCounts[BuildingType::Barracks] > 0)
			{
				availableBuildings.push_back(BuildingType::Barracks);
			}
			if (enemyBuildingCounts[BuildingType::WarFactory] > 0)
			{
				availableBuildings.push_back(BuildingType::WarFactory);
			}
			if (enemyBuildingCounts[BuildingType::AirCraft] > 0)
			{
				availableBuildings.push_back(BuildingType::AirCraft);
			}

			if (!availableBuildings.empty())
			{
				// Randomly select an available building type
				BuildingType selectedBuilding = availableBuildings[rand() % availableBuildings.size()];

				// Based on the selected building, decide which unit to create
				switch (selectedBuilding)
				{
				case BuildingType::Barracks:
					createEnemyUnit("Rifleman");
					std::cout << "Enemy created Rifleman" << std::endl;
					break;
				case BuildingType::WarFactory:
				{
					int unitChance = rand() % 10;
					if (unitChance < 4) // 40% chance for Buggy
					{
						createEnemyUnit("Buggy");
						std::cout << "Enemy created Buggy" << std::endl;
					}
					else if (unitChance < 8) // 40% chance for Tank
					{
						createEnemyUnit("TankAurora");
						std::cout << "Enemy created Tank" << std::endl;
					}
					else  // 20% chance for Harvester
					{
						createEnemyUnit("Harvester");
						std::cout << "Enemy created Harvester" << std::endl;
					}
				}
				break;
				case BuildingType::AirCraft:
				{
					if (rand() % 2) // 50/50 chance
					{
						createEnemyUnit("Hammerhead");
						std::cout << "Enemy created Hammerhead" << std::endl;
					}
					else
					{
						createEnemyUnit("Firehawk");
						std::cout << "Enemy created Firehawk" << std::endl;
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
		enemyProductionTimer = sf::seconds(0);
	}
}

void Game::updateEnemyAIUnitDecisionState(sf::Time deltaTime)
{
	stateTimer += deltaTime;

	switch (enemyAIState)
	{
	case EnemyAIState::Exploring:
		//std::cout << "Exploring" << std::endl;
		if (stateTimer > sf::seconds(30))
		{
			enemyAIState = EnemyAIState::Grouping;
			stateTimer = sf::seconds(0);
		}
		else
		{
			enemyExploring(deltaTime);
		}
		break;
	case EnemyAIState::Grouping:
		//std::cout << "Grouping" << std::endl;
		enemyGroupUnits(deltaTime);
		if (enemyUnits.size() >= 5)
		{
			enemyAIState = EnemyAIState::Attacking;
		}
		break;
	case EnemyAIState::Attacking:
		//std::cout << "Attacking" << std::endl;
		enemyAttacking(deltaTime);
		if (stateTimer > sf::seconds(30))
		{
			enemyAIState = EnemyAIState::Exploring;
			stateTimer = sf::seconds(0);
		}
		break;
	}
}

void Game::updateEnemyAssets(sf::Time t_deltaTime)
{
	for (Building* enemyBuilding : placedEnemyBuildings)
	{
		enemyBuilding->update(t_deltaTime);
	}

	for (Unit* eUnits : enemyUnits)
	{
		eUnits->update(t_deltaTime, enemyUnits);

		for (auto& bullet : eUnits->bullets) // BULLETS
		{
			bool hit = false;

			for (auto& playerUnit : playerUnits)
			{
				if (bullet.bulletShape.getGlobalBounds().intersects(playerUnit->getSprite().getGlobalBounds()))
				{
					bullet.active = false;
					playerUnit->takeDamage(eUnits->getDamage());
					if (playerUnit->getHealth() <= 0)
					{
						playerUnit->m_active = false;
					}
					spawnBulletSparkParticles(bullet.position);
				}
			}
			for (auto& playerBuilding : placedPlayerBuildings)
			{
				if (bullet.bulletShape.getGlobalBounds().intersects(playerBuilding->getBuildingSprite().getGlobalBounds()))
				{
					bullet.active = false;
					playerBuilding->takeDamage(eUnits->getDamage());
					hit = true;
					spawnBulletSparkParticles(bullet.position);
					break;
				}
			}
			if (hit)
			{
				continue;
			}
		}

		for (auto& missile : eUnits->missiles)
		{
			for (auto& playerUnit : playerUnits)
			{
				if (missile.missileSprite.getGlobalBounds().intersects(playerUnit->getSprite().getGlobalBounds()))
				{
					missile.active = false;
					playerUnit->takeDamage(missile.damage);
					if (playerUnit->getHealth() <= 0)
					{
						playerUnit->m_active = false;
					}
					spawnExplosionParticle(missile.position);
				}
			}
		}
		for (auto& missile : eUnits->missiles) // MISSILES
		{
			bool hit = false;

			for (auto& playerUnit : playerUnits)
			{
				if (missile.missileSprite.getGlobalBounds().intersects(playerUnit->getSprite().getGlobalBounds()))
				{
					missile.active = false;
					playerUnit->takeDamage(missile.damage);
					if (playerUnit->getHealth() <= 0)
					{
						playerUnit->m_active = false;
					}
					spawnExplosionParticle(missile.position);
				}
			}
			for (auto& playerBuilding : placedPlayerBuildings)
			{
				if (missile.missileSprite.getGlobalBounds().intersects(playerBuilding->getBuildingSprite().getGlobalBounds()))
				{
					missile.active = false;
					playerBuilding->takeDamage(eUnits->getDamage());
					hit = true;
					spawnExplosionParticle(missile.position);
					break;
				}
			}
			if (hit)
			{
				continue;
			}
		}

		// Remove inactive bullets
		eUnits->bullets.erase(std::remove_if(eUnits->bullets.begin(), eUnits->bullets.end(),
			[](const Bullet& bullet) { return !bullet.active; }), eUnits->bullets.end());
		// Remove inactive missiles
		eUnits->missiles.erase(std::remove_if(eUnits->missiles.begin(), eUnits->missiles.end(),
			[](const Missile& missile) { return !missile.active && missile.trail.empty(); }), eUnits->missiles.end());

		// Remove dead enemy units
		playerUnits.erase(std::remove_if(playerUnits.begin(), playerUnits.end(),
			[](Unit* playerUnits) { return !playerUnits->isActive(); }), playerUnits.end());
		// Remove buildings
		placedPlayerBuildings.erase(std::remove_if(placedPlayerBuildings.begin(), placedPlayerBuildings.end(),
			[](Building* building) { return building->getHealth() <= 0; }), placedPlayerBuildings.end());
	}
}

/// <summary>
/// Creates the specific unit type on that buildings position
/// </summary>
/// <param name="unitType"></param>
void Game::createEnemyUnit(const std::string& unitType)
{
	sf::Vector2f spawnPosition = sf::Vector2f(0, 0);
	sf::Vector2f targetPositionOffset = sf::Vector2f(0, 60);
	bool validBuildingFound = false;

	// Find a building of the appropriate type and use its position
	for (const auto& building : placedEnemyBuildings)
	{
		if (unitType == "Rifleman" && dynamic_cast<Barracks*>(building) != nullptr) 
		{
			spawnPosition = building->getPosition();
			validBuildingFound = true;
			break;
		}
		else if ((unitType == "Harvester" || unitType == "Buggy" || unitType == "TankAurora") && dynamic_cast<WarFactory*>(building) != nullptr)
		{
			spawnPosition = building->getPosition();
			validBuildingFound = true;
			break;
		}
		else if ((unitType == "Hammerhead" || unitType == "Firehawk") && dynamic_cast<AirCraft*>(building) != nullptr)
		{
			spawnPosition = building->getPosition();
			validBuildingFound = true;	
			break;
		}
	}

	if (!validBuildingFound)
	{
		std::cerr << "No building found for creating unit: " << unitType << std::endl;
		return;
	}

	if (unitType == "Rifleman") // Infantry units
	{
		RiflemanSquad* newRiflemanSquad = new RiflemanSquad();
		newRiflemanSquad->setPosition(spawnPosition);
		newRiflemanSquad->setTargetPosition(spawnPosition + targetPositionOffset);
		newRiflemanSquad->setEnemyUnits(playerUnits);
		newRiflemanSquad->setEnemyBuildings(placedPlayerBuildings);

		enemyUnits.push_back(newRiflemanSquad);
	}
	else if (unitType == "Harvester") // Vehicle units
	{
		Harvester* newHarvester = new Harvester();
		newHarvester->setPosition(spawnPosition);
		newHarvester->setTargetPosition(spawnPosition + targetPositionOffset);
		newHarvester->setBuildings(placedEnemyBuildings);
		newHarvester->setTiles(m_levelEditor.m_tiles);
		newHarvester->m_isEnemy = true;
		newHarvester->currentState = newHarvester->MovingToResource;

		enemyUnits.push_back(newHarvester);
	}
	else if (unitType == "Buggy")
	{
		Buggy* newBuggy = new Buggy();
		newBuggy->setPosition(spawnPosition);
		newBuggy->setTargetPosition(spawnPosition + targetPositionOffset);
		newBuggy->setEnemyUnits(playerUnits);
		newBuggy->setEnemyBuildings(placedPlayerBuildings);

		enemyUnits.push_back(newBuggy);
	}
	else if (unitType == "TankAurora")
	{
		TankAurora* newTankAurora = new TankAurora();
		newTankAurora->setPosition(spawnPosition);
		newTankAurora->setTargetPosition(spawnPosition + targetPositionOffset);
		newTankAurora->setEnemyUnits(playerUnits);

		enemyUnits.push_back(newTankAurora);
	}
	else if (unitType == "Hammerhead")
	{
		HammerHead* newHammerHead = new HammerHead();
		newHammerHead->setPosition(spawnPosition);
		newHammerHead->setTargetPosition(spawnPosition + targetPositionOffset);
		newHammerHead->setEnemyUnits(playerUnits);
		newHammerHead->setEnemyBuildings(placedPlayerBuildings);

		enemyUnits.push_back(newHammerHead);
	}
	else if (unitType == "Firehawk")
	{
		Firehawk* newFirehawk = new Firehawk();
		newFirehawk->setPosition(spawnPosition);
		newFirehawk->setTargetPosition(spawnPosition + targetPositionOffset);
		newFirehawk->setEnemyUnits(playerUnits);
		newFirehawk->setEnemyBuildings(placedPlayerBuildings);

		enemyUnits.push_back(newFirehawk);
	}
}

void Game::createEnemyHarvesterUnit()
{
	sf::Vector2f targetPositionOffset = sf::Vector2f(0, 60);

	Harvester* newHarvester = new Harvester();
	newHarvester->setPosition(enemyRefineryBuildingPosition);
	newHarvester->setTargetPosition(enemyRefineryBuildingPosition + targetPositionOffset);
	newHarvester->setBuildings(placedEnemyBuildings);
	newHarvester->setTiles(m_levelEditor.m_tiles);
	newHarvester->m_isEnemy = true;
	newHarvester->currentState = newHarvester->MovingToResource;

	enemyUnits.push_back(newHarvester);
}

void Game::updateBuildingCounts()
{
	for (auto& count : enemyBuildingCounts)
	{
		count.second = 0;
	}

	enemyBuildingCounts =
	{
		{BuildingType::Refinery, 0},
		{BuildingType::Headquarters, 0},
		{BuildingType::Barracks, 0},
		{BuildingType::WarFactory, 0},
		{BuildingType::AirCraft, 0}
	};

	// Iterate over all placed enemy buildings and increment the count for each type
	for (const auto& building : placedEnemyBuildings)
	{
		if (dynamic_cast<Refinery*>(building) != nullptr)
		{
			enemyBuildingCounts[BuildingType::Refinery]++;
		}
		else if (dynamic_cast<Headquarters*>(building) != nullptr) 
		{
			enemyBuildingCounts[BuildingType::Headquarters]++;
		}
		else if (dynamic_cast<Barracks*>(building) != nullptr)
		{
			enemyBuildingCounts[BuildingType::Barracks]++;
		}
		else if (dynamic_cast<WarFactory*>(building) != nullptr)
		{
			enemyBuildingCounts[BuildingType::WarFactory]++;
		}
		else if (dynamic_cast<AirCraft*>(building) != nullptr)
		{
			enemyBuildingCounts[BuildingType::AirCraft]++;
		}
	}
}

void Game::decideNextEnemyBuilding()
{
	updateBuildingCounts();

	const int maxRefineries = 2;
	const int maxBarracks = 2;
	const int maxWarFactories = 2;
	const int maxAirCrafts = 2;

	if (enemyBuildingCounts[BuildingType::Refinery] < maxRefineries)
	{
		placeEnemyBuilding(BuildingType::Refinery);
	}
	else if (enemyBuildingCounts[BuildingType::Barracks] < maxBarracks)
	{
		placeEnemyBuilding(BuildingType::Barracks);
	}
	else if (enemyBuildingCounts[BuildingType::WarFactory] < maxWarFactories)
	{
		placeEnemyBuilding(BuildingType::WarFactory);
	}
	else if (enemyBuildingCounts[BuildingType::AirCraft] < maxAirCrafts)
	{
		placeEnemyBuilding(BuildingType::AirCraft);
	}
}

void Game::placeEnemyBuilding(BuildingType type)
{
	sf::Vector2f position = findPlacementPositionNearExistingBuilding();
	bool createHarvesterUnit = false;

	switch (type)
	{
	case BuildingType::Refinery:
		newEnemyBuilding = new Refinery();
		enemyRefineryBuildingPosition = position;
		createHarvesterUnit = true;
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
		newEnemyBuilding->setPosition(position);
		placedEnemyBuildings.push_back(newEnemyBuilding);
	}
	if (createHarvesterUnit)
	{
		createEnemyHarvesterUnit();
	}
}

sf::Vector2f Game::findPlacementPositionNearExistingBuilding()
{
	const int maxAttempts = 500; // Limit the number of attempts to avoid an infinite loop
	int attempts = 0;
	bool intersectsBuilding = false;
	bool intersectsWallTile = false;
	float buildingWidth = 100.0f;
	float buildingHeight = 100.0f;
	sf::FloatRect mapBounds(100, 100, mapWidth, mapHeight);

	while (attempts < maxAttempts)
	{
		attempts++;

		if (placedEnemyBuildings.empty()) 
		{
			std::cout << "No enemy buildings found :( what the hell" << std::endl;
		}

		int index = rand() % placedEnemyBuildings.size();
		Building* baseBuilding = placedEnemyBuildings[index];

		// Calculate a random position within the placement radius of the selected building
		float radius = baseBuilding->getPlacementRadius().getRadius();
		sf::Vector2f centerPosition = baseBuilding->getPosition();
		float angle = static_cast<float>(rand() % 360) * (3.14159f / 180.0f);
		float distanceFromCenter = static_cast<float>(rand() % static_cast<int>(radius));
		sf::Vector2f newPosition = centerPosition + sf::Vector2f(distanceFromCenter * cos(angle), distanceFromCenter * sin(angle));
		//sf::FloatRect newBuildingRect = newEnemyBuilding->getBuildingSprite().getGlobalBounds();
		sf::FloatRect buildingBounds(newPosition.x - buildingWidth / 2, newPosition.y - buildingHeight / 2, buildingWidth, buildingHeight);

		bool isInsideMap = mapBounds.contains(buildingBounds.left, buildingBounds.top) &&
			mapBounds.contains(buildingBounds.left + buildingBounds.width, buildingBounds.top + buildingBounds.height);

		// Check for intersection with existing buildings
		intersectsBuilding = false;
		for (const auto& building : placedEnemyBuildings) 
		{
			sf::FloatRect existingBuildingRect = building->getBuildingSprite().getGlobalBounds();

			if (buildingBounds.intersects(existingBuildingRect))
			{
				intersectsBuilding = true;
				break;
			}
		}

		// Check for intersection with wall tiles
		intersectsWallTile = false;
		for (const auto& row : m_levelEditor.m_tiles)
		{
			for (const auto& tile : row)
			{
				if (tile.isWall)
				{
					sf::FloatRect wallTileRect = tile.m_tile.getGlobalBounds();

					if (buildingBounds.intersects(wallTileRect))
					{
						intersectsWallTile = true;
						break;
					}
				}
			}
			if (intersectsWallTile) 
			{
				break;
			}
		}

		if (!intersectsBuilding && !intersectsWallTile && isInsideMap) 
		{
			std::cout << newPosition.x << " " << newPosition.y << std::endl;
			return newPosition;
		}
	}
	std::cout << "Could not find a valid position after " << maxAttempts << " attempts" << std::endl;
	return sf::Vector2f(-1, -1);
}

void Game::moveEnemyUnits()
{
	for (auto& enemyUnit : enemyUnits) 
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

sf::Vector2f Game::findNearestPlayerObjectPosition(const sf::Vector2f& enemyPosition)
{
	sf::Vector2f nearestTargetPosition;
	float nearestDistanceSquared = std::numeric_limits<float>::max();

	for (const auto& playerUnit : playerUnits) 
	{
		float distanceSquared = distanceSquaredBetweenPoints(enemyPosition, playerUnit->getPosition());
		if (distanceSquared < nearestDistanceSquared) 
		{
			nearestDistanceSquared = distanceSquared;
			nearestTargetPosition = playerUnit->getPosition();
		}
	}

	for (const auto& building : placedPlayerBuildings)
	{
		float distanceSquared = distanceSquaredBetweenPoints(enemyPosition, building->getPosition());
		if (distanceSquared < nearestDistanceSquared)
		{
			nearestDistanceSquared = distanceSquared;
			nearestTargetPosition = building->getPosition();
		}
	}

	return nearestTargetPosition;
}

float Game::distanceSquaredBetweenPoints(const sf::Vector2f& p1, const sf::Vector2f& p2)
{
	return (p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y);
}

void Game::enemyExploring(sf::Time deltaTime)
{
	static sf::Time exploreTimer = sf::Time::Zero;
	const sf::Time exploreInterval = sf::seconds(8);

	exploreTimer += deltaTime;

	if (exploreTimer >= exploreInterval)
	{
		exploreTimer -= exploreInterval;
		auto validTargets = getValidExplorationTargets();

		if (!validTargets.empty()) 
		{
			for (auto& enemyUnit : enemyUnits)
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
				int randomIndex = rand() % validTargets.size();
				sf::Vector2f targetPos = validTargets[randomIndex];
				std::cout << targetPos.x << " " << targetPos.y << std::endl;
				enemyUnit->moveTo(targetPos);
			}
		}
	}
}

void Game::enemyGroupUnits(sf::Time deltaTime)
{
	static sf::Time groupUnitsTimer = sf::Time::Zero;
	const sf::Time groupInterval = sf::seconds(6);

	groupUnitsTimer += deltaTime;

	sf::Vector2f headquartersPos = findEnemyHeadquartersPosition();
	sf::Vector2f rallyPoint = headquartersPos + sf::Vector2f(-200, -200); 

	if (groupUnitsTimer >= groupInterval)
	{
		for (auto& enemyUnit : enemyUnits)
		{
			if (!enemyUnit)
			{
				continue;
			}
			// Exclude non-combat units like the Harvester from being grouped
			if (dynamic_cast<Harvester*>(enemyUnit) == nullptr)
			{
				enemyUnit->moveTo(rallyPoint);
			}
		}
		groupUnitsTimer -= groupInterval;
	}
}

void Game::enemyAttacking(sf::Time deltaTime)
{
	static sf::Time moveUnitsTimer = sf::Time::Zero;
	const sf::Time moveInterval = sf::seconds(2);

	moveUnitsTimer += deltaTime;

	if (moveUnitsTimer >= moveInterval)
	{
		moveEnemyUnits(); // Move enemy units
		moveUnitsTimer -= moveInterval;
	}
}

std::vector<sf::Vector2f> Game::getValidExplorationTargets()
{
	for (const auto& row : m_levelEditor.m_tiles) 
	{
		for (const auto& tile : row) 
		{
			if (!tile.isWall)
			{
				sf::Vector2f position = tile.m_tile.getPosition();
				validTargets.push_back(position);
			}
		}
	}

	return validTargets;
}

sf::Vector2f Game::findEnemyHeadquartersPosition() 
{
	for (const auto& building : placedEnemyBuildings) 
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
bool Game::isPositionWithinMap(const sf::Vector2f& position)
{
	const float mapLeft = 0;
	const float mapTop = 0;
	const float mapRight = 2500; 
	const float mapBottom = 2500; 

	return position.x >= mapLeft && position.x <= mapRight &&
		position.y >= mapTop && position.y <= mapBottom;
}