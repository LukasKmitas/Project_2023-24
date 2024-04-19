#include "GUI.h"

GUI::GUI(std::vector<Building*>& m_buildings, BuildingType& m_selectedBuildingType, std::vector<std::vector<Tile>>& m_tiles)
	: 
	m_placedBuildings(m_buildings),
	m_selectedBuildingType(m_selectedBuildingType),
	m_sideBar(m_selectedBuildingType),
	m_tilesReference(m_tiles)
{
	initTopBar();
	loadIcons();

	m_guiView.setSize(Global::S_WIDTH, Global::S_HEIGHT);
	m_guiView.setCenter(Global::S_WIDTH / 2, Global::S_HEIGHT / 2);
	m_refinery = new Refinery();
	m_barracks = new Barracks();
	m_warFactory = new WarFactory();
	m_airCraft = new AirCraft();
}

GUI::~GUI()
{
}

/// <summary>
/// updates GUi content
/// </summary>
/// <param name="t_deltaTime"></param>
void GUI::update(sf::Time t_deltaTime)
{
	updateCurrency();
	if (m_showSlider)
	{
		m_sideBar.update(t_deltaTime);
	}
}

/// <summary>
/// Renders GUI
/// </summary>
/// <param name="m_window"></param>
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

/// <summary>
/// handles the mouse clicks for GUI related stuff
/// </summary>
/// <param name="m_mousePosition"></param>
/// <param name="m_window"></param>
void GUI::handleMouseClick(sf::Vector2i m_mousePosition, sf::RenderWindow& m_window)
{
	//std::cout << "Mouse position: (" << mousePosition.x << ", " << mousePosition.y << ")" << std::endl;
	sf::Vector2f worldMousePosition = m_window.mapPixelToCoords(m_mousePosition, m_window.getView());
	sf::Vector2f guiMousePosition = m_window.mapPixelToCoords(m_mousePosition, m_guiView);

	// Selling the building
	if (m_sideBar.getSellSprite().getGlobalBounds().contains(guiMousePosition))
	{
		if (m_selectedBuilding)
		{
			if (m_selectedBuildingType != BuildingType::Headquarters)
			{
				sellBuilding();
			}
			m_selectedBuilding = nullptr;
			m_selectedBuildingType = BuildingType::None;
			m_showSlider = false;
			m_confirmBuildingPlacement = false;
			return;
		}
	}

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
			// Refinery building
			sf::Vector2f refineryIconPosition = m_sideBar.getRefineryIconPosition();
			sf::FloatRect refineryIconBounds(refineryIconPosition, sf::Vector2f(98, 75));
			if (refineryIconBounds.contains(guiMousePosition))
			{
				std::cout << "Refinery Clicked" << std::endl;
				m_selectedBuildingType = BuildingType::Refinery;
				m_confirmBuildingPlacement = true;
			}
			// Barracks building
			sf::Vector2f barracksIconPosition = m_sideBar.getBarracksIconPosition();
			sf::FloatRect barrackIconBounds(barracksIconPosition, sf::Vector2f(98, 75));
			if (barrackIconBounds.contains(guiMousePosition))
			{
				std::cout << "Barracks Clicked" << std::endl;
				m_selectedBuildingType = BuildingType::Barracks;
				m_confirmBuildingPlacement = true;
			}
			// Vehicle building
			sf::Vector2f vehicleIconPosition = m_sideBar.getVehicleIconPosition();
			sf::FloatRect vehicleIconBounds(vehicleIconPosition, sf::Vector2f(98, 75));
			if (vehicleIconBounds.contains(guiMousePosition))
			{
				std::cout << "Vehicle Clicked" << std::endl;
				m_selectedBuildingType = BuildingType::WarFactory;
				m_confirmBuildingPlacement = true;
			}
			// Aircraft building
			sf::Vector2f aircraftIconPosition = m_sideBar.getAirCraftIconPosition();
			sf::FloatRect aircraftIconBounds(aircraftIconPosition, sf::Vector2f(98, 75));
			if (aircraftIconBounds.contains(guiMousePosition))
			{
				std::cout << "Aircraft Clicked" << std::endl;
				m_selectedBuildingType = BuildingType::AirCraft;
				m_confirmBuildingPlacement = true;
			}
		}
		else if (m_selectedBuildingType == BuildingType::Barracks)
		{
			// Rifleman Squad unit
			sf::Vector2f riflemanIconPosition = m_sideBar.getRiflemanIconPosition();
			sf::FloatRect riflemanIconBounds(riflemanIconPosition, sf::Vector2f(98, 75));
			if (riflemanIconBounds.contains(guiMousePosition))
			{
				std::cout << "Rifleman Squad Icon Clicked" << std::endl;
				if (m_riflemanSquad.checkAffordability())
				{
					m_selectedInfantryType = InfantryType::RifleSquad;
					m_unitConfirmed = true;
				}
			}
		}
		else if (m_selectedBuildingType == BuildingType::WarFactory)
		{
			// Harvester 
			sf::Vector2f harvesterIconPosition = m_sideBar.getHarvesterIconPosition();
			sf::FloatRect harvesterIconBounds(harvesterIconPosition, sf::Vector2f(98, 75));
			if (harvesterIconBounds.contains(guiMousePosition))
			{
				std::cout << "Harvester Icon Clicked" << std::endl;
				if (m_harvester.checkAffordability())
				{
					m_selectedVehicleType = VehicleType::Harvester;
					m_unitConfirmed = true;
				}
			}
			// Buggy
			sf::Vector2f buggyIconPosition = m_sideBar.getBuggyIconPosition();
			sf::FloatRect buggyIconBounds(buggyIconPosition, sf::Vector2f(98, 75));
			if (buggyIconBounds.contains(guiMousePosition))
			{
				std::cout << "Buggy Icon Clicked" << std::endl;
				if (m_buggy.checkAffordability())
				{
					m_selectedVehicleType = VehicleType::Buggy;
					m_unitConfirmed = true;
				}
			}
			// Tank Aurora
			sf::Vector2f tankAuroraIconPosition = m_sideBar.getTankAuroraIconPosition();
			sf::FloatRect tankAuroraIconBounds(tankAuroraIconPosition, sf::Vector2f(98, 75));
			if (tankAuroraIconBounds.contains(guiMousePosition))
			{
				std::cout << "Tank Aurora Icon Clicked" << std::endl;
				if (m_tankAurora.checkAffordability())
				{
					m_selectedVehicleType = VehicleType::TankAurora;
					m_unitConfirmed = true;
				}
			}
		}
		else if (m_selectedBuildingType == BuildingType::AirCraft)
		{
			// HammerHead
			sf::Vector2f hammerHeadIconPosition = m_sideBar.getHammerHeadIconPosition();
			sf::FloatRect hammerHeadIconBounds(hammerHeadIconPosition, sf::Vector2f(98, 75));
			if (hammerHeadIconBounds.contains(guiMousePosition))
			{
				std::cout << "HammerHead Icon Clicked" << std::endl;
				if (m_hammerHead.checkAffordability())
				{
					m_selectedAircraftType = AirCraftType::HammerHead;
					m_unitConfirmed = true;
				}
			}

			// Firehawk Unit
			sf::Vector2f firehawkIconPosition = m_sideBar.getFirehawkIconPosition();
			sf::FloatRect firehawkIconBounds(firehawkIconPosition, sf::Vector2f(98, 75));
			if (firehawkIconBounds.contains(guiMousePosition))
			{
				std::cout << "Firehawk Icon Clicked" << std::endl;
				if (m_firehawk.checkAffordability())
				{
					m_selectedAircraftType = AirCraftType::Firehawk;
					m_unitConfirmed = true;
				}
			}
		}
		m_ghostBuildingSprite.setPosition(-2000, -2000);
	}
}

/// <summary>
/// This is more for the ghost buildings and confirms placement
/// </summary>
/// <param name="m_mousePosition"></param>
/// <param name="window"></param>
void GUI::handleBuildingPlacement(sf::Vector2i m_mousePosition, sf::RenderWindow& window)
{
	if (m_confirmBuildingPlacement)
	{
		sf::Vector2f guiMousePosition = window.mapPixelToCoords(m_mousePosition, m_guiView);
		bool isValidPlacement = IsPlacementValid(guiMousePosition, window);
		bool isValidPlacementForTiles = IsPlacementValidForTiles(guiMousePosition);

		if (m_selectedBuildingType == BuildingType::Refinery)
		{
			const sf::Sprite& buildingSprite = m_refinery->getBuildingSprite();
			m_ghostBuildingSprite = buildingSprite;
			m_ghostBuildingSprite.setPosition(guiMousePosition);
		}
		else if (m_selectedBuildingType == BuildingType::Barracks)
		{
			const sf::Sprite& buildingSprite = m_barracks->getBuildingSprite();
			m_ghostBuildingSprite = buildingSprite;
			m_ghostBuildingSprite.setPosition(guiMousePosition);
		}
		else if (m_selectedBuildingType == BuildingType::WarFactory)
		{
			const sf::Sprite& buildingSprite = m_warFactory->getBuildingSprite();
			m_ghostBuildingSprite = buildingSprite;
			m_ghostBuildingSprite.setPosition(guiMousePosition);
		}
		else if (m_selectedBuildingType == BuildingType::AirCraft)
		{
			const sf::Sprite& buildingSprite = m_airCraft->getBuildingSprite();
			m_ghostBuildingSprite = buildingSprite;
			m_ghostBuildingSprite.setPosition(guiMousePosition);
		}

		if (isValidPlacement && isValidPlacementForTiles)
		{
			m_ghostBuildingSprite.setColor(sf::Color(0, 255, 0, 200));
		}
		else
		{
			m_ghostBuildingSprite.setColor(sf::Color(255, 0, 0, 200));
		}
	}

	if (m_confirmBuildingPlacement && sf::Mouse::isButtonPressed(sf::Mouse::Right)) // Place building
	{
		sf::Vector2f guiMousePosition = window.mapPixelToCoords(m_mousePosition, m_guiView);
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
			case BuildingType::WarFactory:
				if (m_warFactory->checkAffordability())
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

/// <summary>
/// What "Buttons" suppose to appear when clicking on the building type in gameplay
/// </summary>
/// <param name="m_mousePosition"></param>
void GUI::handleBuildingSelection(sf::Vector2f m_mousePosition)
{
	for (Building* building : m_placedBuildings)
	{
		if (dynamic_cast<Headquarters*>(building) != nullptr)
		{
			if (building->getBuildingSprite().getGlobalBounds().contains(m_mousePosition))
			{
				std::cout << "You have selected Headquarters " /*<< building->getBuildingCount()*/ << std::endl;
				m_selectedBuilding = building;
				m_showSlider = !m_showSlider;
				if (m_showSlider)
				{
					m_sideBar.clearButtons();
					m_selectedBuildingType = BuildingType::Headquarters;
					m_sideBar.addBuildingButton(m_BuildingTexture1, BuildingType::Refinery, 0, 0, "");
					m_sideBar.addBuildingButton(m_BuildingTexture1, BuildingType::Barracks, 1, 0, "");
					m_sideBar.addBuildingButton(m_BuildingTexture2, BuildingType::WarFactory, 2, 0, "WarFactory");
					m_sideBar.addBuildingButton(m_BuildingTexture1, BuildingType::AirCraft, 0, 1, "");
				}
				break;
			}
		}
		else if (dynamic_cast<Barracks*>(building) != nullptr)
		{
			if (building->getBuildingSprite().getGlobalBounds().contains(m_mousePosition))
			{
				std::cout << "You have selected Barracks " /*<< building->getBuildingCount()*/ << std::endl;
				m_selectedBuilding = building;
				m_showSlider = !m_showSlider;
				if (m_showSlider)
				{
					m_sideBar.clearButtons();
					m_selectedBuildingType = BuildingType::Barracks;
					m_sideBar.addInfantryButton(m_unitInfantryTexture, InfantryType::RifleSquad, 0, 0, "Riflemen");
					//m_sideBar.addInfantryButton(m_unitInfantryTexture, InfantryType::GrenadeSquad, 1, 0, "Grenaders");
					//m_sideBar.addInfantryButton(m_unitInfantryTexture, InfantryType::RocketSquad, 2, 0, "Rocketeers");
					//m_sideBar.addInfantryButton(m_unitInfantryTexture, InfantryType::MedicUnit, 0, 1, "Medic");
				}
				break;
			}
		}
		else if (dynamic_cast<WarFactory*>(building) != nullptr)
		{
			if (building->getBuildingSprite().getGlobalBounds().contains(m_mousePosition))
			{
				std::cout << "You have selected Vehicle " /*<< building->getBuildingCount()*/ << std::endl;
				m_selectedBuilding = building;
				m_showSlider = !m_showSlider;
				if (m_showSlider)
				{
					m_sideBar.clearButtons();
					m_selectedBuildingType = BuildingType::WarFactory;
					m_sideBar.addVehicleButton(m_harvesterIconTexture, VehicleType::Harvester, 0, 0, "Harvester");
					m_sideBar.addVehicleButton(m_unitVehicleTexture, VehicleType::Buggy, 1, 0, "Ranger");
					m_sideBar.addVehicleButton(m_unitVehicleTexture, VehicleType::TankAurora, 2, 0, "TankAurora");
				}
				break;
			}
		}
		else if (dynamic_cast<AirCraft*>(building) != nullptr)
		{
			if (building->getBuildingSprite().getGlobalBounds().contains(m_mousePosition))
			{
				std::cout << "You have selected Aircraft " /*<< building->getBuildingCount()*/ << std::endl;
				m_selectedBuilding = building;
				m_showSlider = !m_showSlider;
				if (m_showSlider)
				{
					m_sideBar.clearButtons();
					m_selectedBuildingType = BuildingType::AirCraft;
					m_sideBar.addAirCraftButton(m_unitAirCraftTexture, AirCraftType::HammerHead, 0, 0, "HammerHead");
					m_sideBar.addAirCraftButton(m_unitAirCraftTexture, AirCraftType::Firehawk, 1, 0, "Firehawk");
				}
				break;
			}
		}
	}
}

/// <summary>
/// updates the currency for player text
/// </summary>
void GUI::updateCurrency()
{
	Global::playerCurrency = std::min(Global::playerCurrency, 5000);
	m_currencyText.setString("Currency:   " + std::to_string(Global::playerCurrency));
}

/// <summary>
/// Initializes the top bar content for currency area
/// </summary>
void GUI::initTopBar()
{
	if (!m_topBarTexture.loadFromFile("Assets\\Images\\GUI\\Card X5.png"))
	{
		std::cout << "problem loading font" << std::endl;
	}
	m_topBar.setSize(sf::Vector2f(400, 50));
	m_topBar.setOutlineColor(sf::Color(200, 100, 100, 200));
	m_topBar.setOutlineThickness(2);
	m_topBar.setPosition(Global::S_WIDTH - 400, 0);
	m_topBar.setTexture(&m_topBarTexture);

	m_currencyFont.loadFromFile("Assets\\Fonts\\ManicSea_19.ttf");
	m_currencyText.setFont(m_currencyFont);
	m_currencyText.setCharacterSize(28);
	m_currencyText.setFillColor(sf::Color(225, 245, 255));
	m_currencyText.setOutlineColor(sf::Color::Blue);
	m_currencyText.setOutlineThickness(0.3);
	m_currencyText.setString("Currency:   " + std::to_string(Global::playerCurrency));
	sf::FloatRect textBounds = m_currencyText.getLocalBounds();
	m_currencyText.setOrigin(textBounds.width / 2, 0);
	m_currencyText.setPosition(Global::S_WIDTH - 200, 5);
}

/// <summary>
/// Load all the icons for the buttons
/// </summary>
void GUI::loadIcons()
{
	if (!m_BuildingTexture1.loadFromFile("Assets\\Images\\GUI\\BuildingIcons.png"))
	{
		std::cout << "Error - Problem Loading Building 1 Texture" << std::endl;
	}
	if (!m_BuildingTexture2.loadFromFile("Assets\\Images\\GUI\\VehicleIcon.png"))
	{
		std::cout << "Error - Problem Loading Building 2 Texture" << std::endl;
	}
	if (!m_unitInfantryTexture.loadFromFile("Assets\\Images\\GUI\\InfantryUnitIcons.png"))
	{
		std::cout << "Error - Problem Loading Infantry Texture" << std::endl;
	}
	if (!m_unitVehicleTexture.loadFromFile("Assets\\Images\\GUI\\Vehicle Unit Icons.png"))
	{
		std::cout << "Error - Problem Loading Infantry Texture" << std::endl;
	}
	if (!m_unitAirCraftTexture.loadFromFile("Assets\\Images\\GUI\\Aircraft Unit Icons.png"))
	{
		std::cout << "Error - Problem Loading Infantry Texture" << std::endl;
	}
	if (!m_harvesterIconTexture.loadFromFile("Assets\\Images\\GUI\\HarvesterLogo.png"))
	{
		std::cout << "Error - Problem Loading Infantry Texture" << std::endl;
	}
}

/// <summary>
/// checks if the placement is valid for buildings
/// </summary>
/// <param name="m_position"></param>
/// <param name="m_window"></param>
/// <returns></returns>
bool GUI::IsPlacementValid(sf::Vector2f& m_position, sf::RenderWindow& m_window)
{
	sf::Vector2f worldPosition = m_window.mapPixelToCoords(sf::Vector2i(m_position), m_window.getView());
	m_ghostBuildingSprite.setPosition(worldPosition);
	sf::FloatRect newBuildingBounds = m_ghostBuildingSprite.getGlobalBounds();
	m_placementValid = false;

	for (Building* building : m_placedBuildings)
	{
		sf::FloatRect existingBuildingBounds = building->getBuildingSprite().getGlobalBounds();
		if (newBuildingBounds.intersects(existingBuildingBounds))
		{
			return false;
		}

		const sf::CircleShape& existingBuildingRadius = building->getPlacementRadius();
		sf::Vector2f center = existingBuildingRadius.getPosition();
		float radius = existingBuildingRadius.getRadius();

		sf::Vector2f newBuildingCenter(
			newBuildingBounds.left + newBuildingBounds.width / 2.0f,
			newBuildingBounds.top + newBuildingBounds.height / 2.0f);

		float distance = std::sqrt(std::pow(newBuildingCenter.x - center.x, 2) + std::pow(newBuildingCenter.y - center.y, 2));

		if (distance < radius)
		{
			m_placementValid = true;
		}
	}
	return m_placementValid;
}

/// <summary>
/// checks if the placement is valid for tiles
/// </summary>
/// <param name="position"></param>
/// <returns></returns>
bool GUI::IsPlacementValidForTiles(sf::Vector2f& position)
{
	// Convert the GUI position to tile coordinates
	int tileX = static_cast<int>(position.x) / m_tile.m_tileSize;
	int tileY = static_cast<int>(position.y) / m_tile.m_tileSize;

	if (tileX >= 0 && tileX < levelEditor.m_numRows && tileY >= 0 && tileY < levelEditor.m_numCols)
	{
		if (m_tilesReference[tileY][tileX].m_isWall)
		{
			return false;
		}

		// Check for building intersections with the tiles
		sf::FloatRect newBuildingBounds = m_ghostBuildingSprite.getGlobalBounds();
		for (int i = 0; i < levelEditor.m_numRows; ++i)
		{
			for (int j = 0; j < levelEditor.m_numCols; ++j)
			{
				if (m_tilesReference[i][j].m_isWall) 
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

/// <summary>
/// sell the building off
/// </summary>
void GUI::sellBuilding() 
{
	if (m_selectedBuilding) 
	{
		Global::playerCurrency += m_selectedBuilding->getCost() - 300;
		auto it = std::find(m_placedBuildings.begin(), m_placedBuildings.end(), m_selectedBuilding);
		if (it != m_placedBuildings.end()) 
		{
			m_placedBuildings.erase(it);
		}
	}
}