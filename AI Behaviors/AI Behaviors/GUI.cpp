#include "GUI.h"

GUI::GUI(std::vector<Building*>& buildings, BuildingType& selectedBuildingType, std::vector<std::vector<Tile>>& tiles)
	: 
	placedBuildings(buildings),
	m_selectedBuildingType(selectedBuildingType),
	m_sideBar(selectedBuildingType),
	m_tilesReference(tiles)
{
	setupTopBar();

	if (!m_BuildingTexture1.loadFromFile("Assets\\Images\\BuildingIcons.png"))
	{
		std::cout << "Error - Problem Loading Building 1 Texture" << std::endl;
	}
	if (!m_BuildingTexture2.loadFromFile("Assets\\Images\\VehicleIcon.png"))
	{
		std::cout << "Error - Problem Loading Building 2 Texture" << std::endl;
	}
	if (!m_unitInfantryTexture.loadFromFile("Assets\\Images\\InfantryUnitIcons.png"))
	{
		std::cout << "Error - Problem Loading Infantry Texture" << std::endl;
	}
	if (!m_unitVehicleTexture.loadFromFile("Assets\\Images\\Vehicle Unit Icons.png"))
	{
		std::cout << "Error - Problem Loading Infantry Texture" << std::endl;
	}
	if (!m_unitAirCraftTexture.loadFromFile("Assets\\Images\\Aircraft Unit Icons.png"))
	{
		std::cout << "Error - Problem Loading Infantry Texture" << std::endl;
	}
	m_guiView.setSize(Global::S_WIDTH, Global::S_HEIGHT);
	m_guiView.setCenter(Global::S_WIDTH / 2, Global::S_HEIGHT / 2);
	m_refinery = new Refinery();
	m_barracks = new Barracks();
	m_vehicle = new Vehicle();
	m_airCraft = new AirCraft();
}

GUI::~GUI()
{
}

void GUI::update(sf::Time t_deltaTime)
{
	updateCurrency();
}

void GUI::render(sf::RenderWindow& m_window)
{
	m_window.setView(m_guiView);
	m_window.draw(m_topBar);
	m_window.draw(m_currencyText);
	if (m_showSlider) 
	{
		m_sideBar.render(m_window);
	}
	if (m_selectedBuildingType != BuildingType::None)
	{
		m_window.draw(m_ghostBuildingSprite);
	}
}

void GUI::handleMouseClick(sf::Vector2i mousePosition, sf::RenderWindow& m_window)
{
	//std::cout << "Mouse position: (" << mousePosition.x << ", " << mousePosition.y << ")" << std::endl;
	sf::Vector2f worldMousePosition = m_window.mapPixelToCoords(mousePosition, m_window.getView());
	sf::Vector2f guiMousePosition = m_window.mapPixelToCoords(mousePosition, m_guiView);

	if (!m_sideBar.getSideBarRect().getGlobalBounds().contains(guiMousePosition))
	{
		m_selectedBuildingType = BuildingType::None;
		m_showSlider = false;
		m_confirmBuildingPlacement = false;
	}

	handleBuildingSelection(worldMousePosition);

	if (m_showSlider)
	{
		if (m_selectedBuildingType == BuildingType::Headquarters)
		{
			// Refinery
			sf::Vector2f refineryIconPosition = m_sideBar.getRefineryIconPosition();
			sf::FloatRect refineryIconBounds(refineryIconPosition, sf::Vector2f(120, 92));
			if (refineryIconBounds.contains(guiMousePosition))
			{
				std::cout << "Refinery Clicked" << std::endl;
				m_selectedBuildingType = BuildingType::Refinery;
				m_confirmBuildingPlacement = true;
			}
			// Barracks
			sf::Vector2f barracksIconPosition = m_sideBar.getBarracksIconPosition();
			sf::FloatRect barrackIconBounds(barracksIconPosition, sf::Vector2f(120, 92));
			if (barrackIconBounds.contains(guiMousePosition))
			{
				std::cout << "Barracks Clicked" << std::endl;
				m_selectedBuildingType = BuildingType::Barracks;
				m_confirmBuildingPlacement = true;
			}
			// Vehicle
			sf::Vector2f vehicleIconPosition = m_sideBar.getVehicleIconPosition();
			sf::FloatRect vehicleIconBounds(vehicleIconPosition, sf::Vector2f(120, 92));
			if (vehicleIconBounds.contains(guiMousePosition))
			{
				std::cout << "Vehicle Clicked" << std::endl;
				m_selectedBuildingType = BuildingType::Vehicle;
				m_confirmBuildingPlacement = true;
			}
			// Aircraft
			sf::Vector2f aircraftIconPosition = m_sideBar.getAirCraftIconPosition();
			sf::FloatRect aircraftIconBounds(aircraftIconPosition, sf::Vector2f(120, 92));
			if (aircraftIconBounds.contains(guiMousePosition))
			{
				std::cout << "Aircraft Clicked" << std::endl;
				m_selectedBuildingType = BuildingType::AirCraft;
				m_confirmBuildingPlacement = true;
			}
		}
		else if (m_selectedBuildingType == BuildingType::Barracks)
		{
			// InfantrySquad
		}
		m_ghostBuildingSprite.setPosition(-2000, -2000);
	}
}

void GUI::handleBuildingPlacement(sf::Vector2i mousePosition, sf::RenderWindow& window)
{
	if (m_confirmBuildingPlacement)
	{
		sf::Vector2f guiMousePosition = window.mapPixelToCoords(mousePosition, m_guiView);
		if (m_selectedBuildingType == BuildingType::Refinery)
		{
			const sf::Sprite& buildingSprite = m_refinery->getBuildingSprite();
			m_ghostBuildingSprite = buildingSprite;
			m_ghostBuildingSprite.setPosition(guiMousePosition);
			m_ghostBuildingSprite.setColor(sf::Color(255, 255, 255, 128));
		}
		else if (m_selectedBuildingType == BuildingType::Barracks)
		{
			const sf::Sprite& buildingSprite = m_barracks->getBuildingSprite();
			m_ghostBuildingSprite = buildingSprite;
			m_ghostBuildingSprite.setPosition(guiMousePosition);
			m_ghostBuildingSprite.setColor(sf::Color(255, 255, 255, 128));
		}
		else if (m_selectedBuildingType == BuildingType::Vehicle)
		{
			const sf::Sprite& buildingSprite = m_vehicle->getBuildingSprite();
			m_ghostBuildingSprite = buildingSprite;
			m_ghostBuildingSprite.setPosition(guiMousePosition);
			m_ghostBuildingSprite.setColor(sf::Color(255, 255, 255, 128));
		}
		else if (m_selectedBuildingType == BuildingType::AirCraft)
		{
			const sf::Sprite& buildingSprite = m_airCraft->getBuildingSprite();
			m_ghostBuildingSprite = buildingSprite;
			m_ghostBuildingSprite.setPosition(guiMousePosition);
			m_ghostBuildingSprite.setColor(sf::Color(255, 255, 255, 128));
		}
	}

	if (m_confirmBuildingPlacement && sf::Mouse::isButtonPressed(sf::Mouse::Right))
	{
		sf::Vector2f guiMousePosition = window.mapPixelToCoords(mousePosition, m_guiView);
		bool isValidPlacement = IsPlacementValid(guiMousePosition, window);

		if (isValidPlacement && IsPlacementValidForTiles(guiMousePosition))
		{
			switch (m_selectedBuildingType)
			{
			case BuildingType::Refinery:
				if (m_refinery->checkAffordability())
				{
					m_confirmed = true;
				}
				break;
			case BuildingType::Barracks:
				if (m_barracks->checkAffordability())
				{
					m_confirmed = true;
				}
				break;
			case BuildingType::Vehicle:
				if (m_vehicle->checkAffordability())
				{
					m_confirmed = true;
				}
				break;
			case BuildingType::AirCraft:
				if (m_airCraft->checkAffordability())
				{
					m_confirmed = true;
				}
				break;
			default:
				break;
			}
		}
		m_ghostBuildingSprite.setPosition(-2000, -2000);
	}
}

void GUI::handleBuildingSelection(sf::Vector2f mousePosition)
{
	for (Building* building : placedBuildings)
	{
		if (dynamic_cast<Headquarters*>(building) != nullptr)
		{
			if (building->getBuildingSprite().getGlobalBounds().contains(mousePosition))
			{
				std::cout << "You have selected Headquarters " << building->getBuildingID() << std::endl;
				//std::cout << "Headquarters selected" << std::endl;
				m_showSlider = !m_showSlider;
				if (m_showSlider)
				{
					m_selectedBuildingType = BuildingType::Headquarters;
					m_sideBar.addBuildingButton(m_BuildingTexture1, BuildingType::Refinery, 0, 0, "");
					m_sideBar.addBuildingButton(m_BuildingTexture1, BuildingType::Barracks, 1, 0, "");
					m_sideBar.addBuildingButton(m_BuildingTexture2, BuildingType::Vehicle, 2, 0, "Vehicle");
					m_sideBar.addBuildingButton(m_BuildingTexture1, BuildingType::AirCraft, 0, 1, "");
				}
				break;
			}
		}
		else if (dynamic_cast<Barracks*>(building) != nullptr)
		{
			if (building->getBuildingSprite().getGlobalBounds().contains(mousePosition))
			{
				std::cout << "You have selected Barracks " << building->getBuildingID() << std::endl;
				//std::cout << "Barracks selected" << std::endl;
				m_showSlider = !m_showSlider;
				if (m_showSlider)
				{
					m_selectedBuildingType = BuildingType::Barracks;
					m_sideBar.addInfantryButton(m_unitInfantryTexture, InfantryType::RifleSquad, 0, 0, "");
					m_sideBar.addInfantryButton(m_unitInfantryTexture, InfantryType::GrenadeSquad, 1, 0, "");
					m_sideBar.addInfantryButton(m_unitInfantryTexture, InfantryType::RocketSquad, 2, 0, "");
					m_sideBar.addInfantryButton(m_unitInfantryTexture, InfantryType::MedicUnit, 0, 1, "");
				}
				break;
			}
		}
		else if (dynamic_cast<Vehicle*>(building) != nullptr)
		{
			if (building->getBuildingSprite().getGlobalBounds().contains(mousePosition))
			{
				std::cout << "You have selected Vehicle " << building->getBuildingID() << std::endl;
				//std::cout << "Vehicle selected" << std::endl;
				m_showSlider = !m_showSlider;
				if (m_showSlider)
				{
					m_selectedBuildingType = BuildingType::Vehicle;
					m_sideBar.addVehicleButton(m_unitVehicleTexture, VehicleType::Ranger, 0, 0, "Ranger");
					m_sideBar.addVehicleButton(m_unitVehicleTexture, VehicleType::Tank, 1, 0, "Tank");
					m_sideBar.addVehicleButton(m_unitVehicleTexture, VehicleType::Artillery, 2, 0, "Artillery");
					m_sideBar.addVehicleButton(m_unitVehicleTexture, VehicleType::None, 0, 1, "");
				}
				break;
			}
		}
		else if (dynamic_cast<AirCraft*>(building) != nullptr)
		{
			if (building->getBuildingSprite().getGlobalBounds().contains(mousePosition))
			{
				std::cout << "You have selected Aircraft " << building->getBuildingID() << std::endl;
				//std::cout << "Aircraft selected" << std::endl;
				m_showSlider = !m_showSlider;
				if (m_showSlider)
				{
					m_selectedBuildingType = BuildingType::AirCraft;
					m_sideBar.addAirCraftButton(m_unitAirCraftTexture, AirCraftType::Orca, 0, 0, "Orca");
					m_sideBar.addAirCraftButton(m_unitAirCraftTexture, AirCraftType::HammerHead, 1, 0, "HammerHead");
					m_sideBar.addAirCraftButton(m_unitAirCraftTexture, AirCraftType::Firehawk, 2, 0, "Firehawk");
					m_sideBar.addAirCraftButton(m_unitAirCraftTexture, AirCraftType::None, 0, 1, "");
				}
				break;
			}
		}
	}
}

void GUI::updateCurrency()
{
	Global::currency = std::min(Global::currency, 5000);
	m_currencyText.setString("Currency: " + std::to_string(Global::currency));
}

void GUI::setupTopBar()
{
	m_topBar.setSize(sf::Vector2f(400, 20));
	m_topBar.setFillColor(sf::Color(135, 135, 135, 255));
	m_topBar.setOutlineColor(sf::Color(200, 100, 100, 200));
	m_topBar.setOutlineThickness(3);
	m_topBar.setPosition(Global::S_WIDTH - 400, 0);

	m_currencyFont.loadFromFile("Assets\\Fonts\\ManicSea_19.ttf");
	m_currencyText.setFont(m_currencyFont);
	m_currencyText.setCharacterSize(16);
	m_currencyText.setFillColor(sf::Color::White);
	m_currencyText.setOutlineColor(sf::Color::Black);
	m_currencyText.setOutlineThickness(1);
	m_currencyText.setStyle(sf::Text::Bold);
	m_currencyText.setString("Currency: " + std::to_string(Global::currency));
	sf::FloatRect textBounds = m_currencyText.getLocalBounds();
	m_currencyText.setOrigin(textBounds.width / 2, 0);
	m_currencyText.setPosition(Global::S_WIDTH - 200, 0);
}

bool GUI::IsPlacementValid(sf::Vector2f& m_position, sf::RenderWindow& m_window)
{
	sf::Vector2f worldPosition = m_window.mapPixelToCoords(sf::Vector2i(m_position), m_window.getView());
	m_ghostBuildingSprite.setPosition(worldPosition);
	sf::FloatRect newBuildingBounds = m_ghostBuildingSprite.getGlobalBounds();

	for (Building* building : placedBuildings)
	{
		sf::FloatRect existingBuildingBounds = building->getBuildingSprite().getGlobalBounds();
		if (newBuildingBounds.intersects(existingBuildingBounds))
		{
			return false;
		}
	}
	return true;
}

bool GUI::IsPlacementValidForTiles(sf::Vector2f& position)
{
	// Convert the GUI position to tile coordinates
	int tileX = static_cast<int>(position.x) / m_tile.tileSize;
	int tileY = static_cast<int>(position.y) / m_tile.tileSize;

	if (tileX >= 0 && tileX < levelEditor.numRows && tileY >= 0 && tileY < levelEditor.numCols)
	{
		if (m_tilesReference[tileY][tileX].isWall)
		{
			return false;
		}

		// Check for building intersections with the tiles
		sf::FloatRect newBuildingBounds = m_ghostBuildingSprite.getGlobalBounds();
		for (int i = 0; i < levelEditor.numRows; ++i)
		{
			for (int j = 0; j < levelEditor.numCols; ++j)
			{
				if (m_tilesReference[i][j].isWall) 
				{
					sf::FloatRect tileRect(j * 50, i * 50, 50, 50);
					if (newBuildingBounds.intersects(tileRect))
					{
						return false;
					}
				}
			}
		}
		return true;
	}
	return false;
}
