#include "Game.h"
#include <iostream>


/// <summary>
/// default constructor
/// </summary>
Game::Game() :
	m_window{ sf::VideoMode{ Global::S_WIDTH, Global::S_HEIGHT, 32U }, "Gills & Glory" },
	m_exitGame{false},
	m_grid(50,50,50), // Only for visual aid
	m_headquaters{new Headquarters()}
{
	m_gui.m_headquarters = m_headquaters;
	gameView.setSize(sf::Vector2f(Global::S_WIDTH, Global::S_HEIGHT));
	gameView.setCenter(Global::S_WIDTH / 2, Global::S_HEIGHT / 2);
	m_window.setView(gameView);
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
		}
		if (sf::Event::MouseButtonPressed == newEvent.type) // Check for mouse button press.
		{
			if (newEvent.mouseButton.button == sf::Mouse::Left) // Check for left mouse button.
			{
				sf::Vector2i mousePosition = sf::Mouse::getPosition(m_window);
				m_gui.handleMouseClick(mousePosition, m_window, m_selectedBuildingType);
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
	if (sf::Keyboard::Escape == t_event.key.code)
	{
		m_exitGame = true;
	}

	cameraVelocity = sf::Vector2f(0.0f, 0.0f);
	float speed = (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) ? 2.0f * viewMoveSpeed : viewMoveSpeed;

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

	updateView();
	createBuilding(m_window);
	m_headquaters->update(t_deltaTime);
	m_gui.update(t_deltaTime);
	for (Building* building : placedBuildings) 
	{
		building->update(t_deltaTime);
	}
	sf::Vector2i mousePosition = sf::Mouse::getPosition(m_window);
	m_gui.handleBuildingPlacement(mousePosition, m_window);
}

/// <summary>
/// draw the frame and then switch buffers
/// </summary>
void Game::render()
{
	m_window.clear(sf::Color::Black);

	m_grid.draw(m_window);
	for (Building* building : placedBuildings)
	{
		building->render(m_window);
	}
	m_gui.render(m_window);

	m_window.setView(gameView);

	m_headquaters->render(m_window);

	m_window.display();
}

/// <summary>
/// Update the screen view position
/// </summary>
void Game::updateView()
{
	sf::Vector2f viewCenter = gameView.getCenter();
	sf::Vector2f mousePosition = sf::Vector2f(sf::Mouse::getPosition(m_window));

	if (mousePosition.x < Global::S_WIDTH * 0.1 && viewCenter.x - viewMoveSpeed > minX) 
	{
		viewCenter.x -= viewMoveSpeed;
	}
	if (mousePosition.x > Global::S_WIDTH * 0.9 && viewCenter.x + viewMoveSpeed < maxX) 
	{
		viewCenter.x += viewMoveSpeed;
	}
	if (mousePosition.y < Global::S_HEIGHT * 0.1 && viewCenter.y - viewMoveSpeed > minY) 
	{
		viewCenter.y -= viewMoveSpeed;
	}
	if (mousePosition.y > Global::S_HEIGHT * 0.9 && viewCenter.y + viewMoveSpeed < maxY)
	{
		viewCenter.y += viewMoveSpeed;
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

		m_gui.m_confirmBuildingPlacement = false;
		m_gui.m_confirmed = false;
	}
}
