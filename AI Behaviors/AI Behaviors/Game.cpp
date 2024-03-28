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

	createBase();
	initParticles();

	m_levelEditor.resetFogOfWar();

	selectionBox.setFillColor(sf::Color(0, 255, 0, 50));
	selectionBox.setOutlineColor(sf::Color(0, 255, 0));
	selectionBox.setOutlineThickness(1.0f);

	hiddenNeurons = m_neural_network.getHiddenNeurons();
	biasNeurons = m_neural_network.getBiasNeurons();
	initializeNeuralNetwork();
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
		loadLevel(m_levelLoader.levelFilenames[m_levelLoader.selectedButtonIndex]);
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
		for (auto& unit : playerUnits)
		{
			unit->update(t_deltaTime, playerUnits);

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
			for (auto& missile : unit->missiles)
			{
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
		}
		for (Unit* eUnits : enemyUnits)
		{
			eUnits->update(t_deltaTime, enemyUnits);

			for (auto& bullet : eUnits->bullets)
			{
				for (auto& playerUnit : playerUnits)
				{
					if (bullet.bulletShape.getGlobalBounds().intersects(playerUnit->getSprite().getGlobalBounds()))
					{
						bullet.active = false;
						playerUnit->takeDamage(bullet.damage);
						if (playerUnit->getHealth() <= 0)
						{
							playerUnit->m_active = false;
						}
						spawnBulletSparkParticles(bullet.position);
					}
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
		}
		m_particleSystem.update(t_deltaTime);
		updateFogOfWarBasedOnUnits(playerUnits);
		updateFogOfWarBasedOnBuildings(placedBuildings);
		spawnBubbleParticles();
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
		for (Building* building : placedBuildings)
		{
			building->render(m_window);
		}
		for (Unit* unit : playerUnits)
		{
			unit->render(m_window);
		}
		for (Unit* eUnit : enemyUnits)
		{
			sf::Vector2f unitPosition = eUnit->getPosition();
			int tileX = static_cast<int>(unitPosition.x / m_tiles.tileSize);
			int tileY = static_cast<int>(unitPosition.y / m_tiles.tileSize);

			if (m_levelEditor.m_tiles[tileY][tileX].fogStatus == Tile::FogStatus::Visible)
			{
				eUnit->render(m_window);
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
			placedBuildings.push_back(newRefinery);
			sf::Vector2f buildingPosition = m_gui.m_selectedBuilding->getPosition();

			sf::Vector2f spawnPosition = worldMousePosition + sf::Vector2f(0.0f, 60.0f);
			Harvester* newHarvester = new Harvester();
			newHarvester->setPosition(spawnPosition);
			newHarvester->setTargetPosition(spawnPosition);
			newHarvester->setBuildings(placedBuildings);
			newHarvester->setTiles(m_levelEditor.m_tiles);
			newHarvester->currentState = newHarvester->MovingToResource;
			playerUnits.push_back(newHarvester);

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

				playerUnits.push_back(newHarvester);

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
				playerUnits.push_back(newBuggy);

				m_gui.m_unitConfirmed = false;
				m_gui.m_selectedBuilding = nullptr;
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
				newTankAurora->setTargetPosition(spawnPosition);
				newTankAurora->setEnemyUnits(enemyUnits);
				playerUnits.push_back(newTankAurora);

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
				newHammerHead->setEnemyUnits(enemyUnits);
				playerUnits.push_back(newHammerHead);

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
				newFirehawk->setEnemyUnits(enemyUnits);
				playerUnits.push_back(newFirehawk);

				m_gui.m_unitConfirmed = false;
				m_gui.m_selectedBuilding = nullptr;
			}
		}
	}
}

void Game::createEnemyUnit()
{
	if (!runOnce)
	{
		sf::Vector2f spawnPosition = sf::Vector2f(300.0f, 950.0f);

		Buggy* newBuggy = new Buggy();
		newBuggy->setPosition(spawnPosition);
		newBuggy->setTargetPosition(spawnPosition);
		newBuggy->setEnemyUnits(playerUnits);
		enemyUnits.push_back(newBuggy);

		sf::Vector2f spawnPosition2 = sf::Vector2f(500.0f, 950.0f);

		Buggy* newBuggy2 = new Buggy();
		newBuggy2->setPosition(spawnPosition2);
		newBuggy2->setTargetPosition(spawnPosition2);
		newBuggy2->setEnemyUnits(playerUnits);
		enemyUnits.push_back(newBuggy2);

		sf::Vector2f spawnPosition3 = sf::Vector2f(700.0f, 950.0f);

		TankAurora* newTankAurora = new TankAurora();
		newTankAurora->setPosition(spawnPosition3);
		newTankAurora->setTargetPosition(spawnPosition3);
		newTankAurora->setEnemyUnits(playerUnits);
		enemyUnits.push_back(newTankAurora);

		runOnce = true;
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