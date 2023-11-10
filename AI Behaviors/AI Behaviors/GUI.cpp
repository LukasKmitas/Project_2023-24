#include "GUI.h"

GUI::GUI(std::vector<Building*>& buildings) : placedBuildings(buildings)
{
	setupFontAndText();
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
	m_guiView.setSize(Global::S_WIDTH, Global::S_HEIGHT);
	m_guiView.setCenter(Global::S_WIDTH / 2, Global::S_HEIGHT / 2);
	m_refinery = new Refinery();
	m_barracks = new Barracks();
	m_refinery->setPosition(sf::Vector2f(-5000, -5000));
	m_barracks->setPosition(sf::Vector2f(-5000, -5000));
}

GUI::~GUI()
{
	delete m_headquarters;
	delete m_refinery;
	delete m_barracks;
}

void GUI::update(sf::Time t_deltaTime)
{
	updateCurrency();
}

void GUI::render(sf::RenderWindow& m_window)
{
	m_window.setView(m_guiView);

	m_window.draw(m_welcomeMessage);
	m_window.draw(m_topBar);
	m_window.draw(m_currencyText);
	if (m_showSlider) 
	{
		m_sideBar.render(m_window);
	}
	if (m_selectedBuildingType != BuildingType::None)
	{
		m_window.draw(m_buildingPreviewSprite);
	}
}

void GUI::handleMouseClick(sf::Vector2i mousePosition, sf::RenderWindow& m_window, BuildingType& selectedBuildingType)
{
	//std::cout << "Mouse position: (" << mousePosition.x << ", " << mousePosition.y << ")" << std::endl;

	sf::Vector2f worldMousePosition = m_window.mapPixelToCoords(mousePosition, m_window.getView());

	if (m_headquarters->getBuildingSprite().getGlobalBounds().contains(worldMousePosition))
	{
		std::cout << "Headquarters selected" << std::endl;
		m_showSlider = !m_showSlider;
		if (m_showSlider)
		{
			m_selectedBuildingType = BuildingType::Headquarters;
			m_sideBar.addBuildingButton(m_BuildingTexture1, BuildingType::Refinery, 0, 0, "");
			m_sideBar.addBuildingButton(m_BuildingTexture1, BuildingType::Barracks, 1, 0, "");
			m_sideBar.addBuildingButton(m_BuildingTexture2, BuildingType::Vehicle, 2, 0, "Vehicle");
			m_sideBar.addBuildingButton(m_BuildingTexture1, BuildingType::AirCraft, 0, 1, "");
		}
	}
	/*else if (m_barracks->getBuildingSprite().getGlobalBounds().contains(worldMousePosition))
	{
		std::cout << "Barracks selected" << std::endl;
		m_showSlider = !m_showSlider;
		if (m_showSlider)
		{
			m_selectedBuildingType = BuildingType::Barracks;
			m_sideBar.addInfantryButton(m_unitInfantryTexture, InfantryType::RifleSquad, 0, 0, "");
			m_sideBar.addInfantryButton(m_unitInfantryTexture, InfantryType::GrenadeSquad, 1, 0, "");
			m_sideBar.addInfantryButton(m_unitInfantryTexture, InfantryType::RocketSquad, 2, 0, "");
			m_sideBar.addInfantryButton(m_unitInfantryTexture, InfantryType::MedicUnit, 0, 1, "");
		}
	}*/
	else if (!m_sideBar.getSideBarRect().getGlobalBounds().contains(worldMousePosition))
	{
		std::cout << "BUILDING TYPE NONE" << std::endl;
		m_selectedBuildingType = BuildingType::None;
	}

	for (Building* building : placedBuildings) 
	{
		if (dynamic_cast<Barracks*>(building) != nullptr) 
		{
			if (building->getBuildingSprite().getGlobalBounds().contains(worldMousePosition)) 
			{
				std::cout << "You have selected Barracks " << building->getBuildingID() << std::endl;
				//barracksBuildingPosition = building->getPosition();
				//m_barracks->setPosition(barracksBuildingPosition);
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
	}

	if (m_showSlider)
	{
		sf::Vector2f guiMousePosition = m_window.mapPixelToCoords(mousePosition, m_guiView);
		if (m_selectedBuildingType == BuildingType::Headquarters)
		{
			// Refinery
			sf::Vector2f refineryIconPosition = m_sideBar.getRefineryIconPosition();
			sf::FloatRect refineryIconBounds(refineryIconPosition, sf::Vector2f(120, 92));
			if (refineryIconBounds.contains(guiMousePosition))
			{
				std::cout << "Refinery Clicked" << std::endl;
				m_selectedBuildingType = BuildingType::Refinery;
				selectedBuildingType = BuildingType::Refinery;
				m_confirmBuildingPlacement = true;
			}
			// Barracks
			sf::Vector2f barracksIconPosition = m_sideBar.getBarracksIconPosition();
			sf::FloatRect barrackIconBounds(barracksIconPosition, sf::Vector2f(120, 92));
			if (barrackIconBounds.contains(guiMousePosition))
			{
				std::cout << "Barracks Clicked" << std::endl;
				m_selectedBuildingType = BuildingType::Barracks;
				selectedBuildingType = BuildingType::Barracks;
				m_confirmBuildingPlacement = true;
			}
		}
		else if (m_selectedBuildingType == BuildingType::Barracks)
		{
			// InfantrySquad
			/*sf::Vector2f barracksIconPosition = m_sideBar.getBarracksIconPosition();
			sf::FloatRect barrackIconBounds(barracksIconPosition, sf::Vector2f(120, 92));
			if (barrackIconBounds.contains(guiMousePosition))
			{
				std::cout << "Barracks Clicked" << std::endl;
				m_selectedBuildingType = BuildingType::Barracks;
				selectedBuildingType = BuildingType::Barracks;
				m_confirmBuildingPlacement = true;
			}*/
		}

		m_buildingPreviewSprite.setPosition(-2000, -2000);
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
			m_buildingPreviewSprite = buildingSprite;
			m_buildingPreviewSprite.setPosition(guiMousePosition);
			m_buildingPreviewSprite.setColor(sf::Color(255, 255, 255, 128));
		}
		else if (m_selectedBuildingType == BuildingType::Barracks)
		{
			const sf::Sprite& buildingSprite = m_barracks->getBuildingSprite();
			m_buildingPreviewSprite = buildingSprite;
			m_buildingPreviewSprite.setPosition(guiMousePosition);
			m_buildingPreviewSprite.setColor(sf::Color(255, 255, 255, 128));
		}
	}

	if (m_confirmBuildingPlacement && sf::Mouse::isButtonPressed(sf::Mouse::Right))
	{
		sf::Vector2f worldMousePosition = window.mapPixelToCoords(mousePosition, window.getView());
		bool isValidPlacement = IsPlacementValid(worldMousePosition);

		if (isValidPlacement)
		{
			if (m_selectedBuildingType == BuildingType::Refinery)
			{
				m_refinery->checkAffordability();
			}
			else if (m_selectedBuildingType == BuildingType::Barracks)
			{
				m_barracks->checkAffordability();
			}
			m_confirmed = true;
			m_buildingPreviewSprite.setPosition(-2000, -2000);
		}
	}
}

void GUI::updateCurrency()
{
	Global::currency = std::min(Global::currency, 5000);
	m_currencyText.setString("Currency: " + std::to_string(Global::currency));
}

void GUI::setupFontAndText()
{
	if (!m_ArialBlackfont.loadFromFile("Assets\\Fonts\\ManicSea_19.ttf"))
	{
		std::cout << "problem loading font" << std::endl;
	}
	m_welcomeMessage.setFont(m_ArialBlackfont);
	m_welcomeMessage.setString("Gills & Glory");
	m_welcomeMessage.setStyle(sf::Text::Italic | sf::Text::Bold);
	m_welcomeMessage.setPosition(Global::S_WIDTH / 2, Global::S_HEIGHT / 10);
	m_welcomeMessage.setCharacterSize(80U);
	m_welcomeMessage.setOutlineColor(sf::Color::Blue);
	m_welcomeMessage.setFillColor(sf::Color::White);
	m_welcomeMessage.setOutlineThickness(1.0f);
	sf::FloatRect textBounds = m_welcomeMessage.getLocalBounds();
	m_welcomeMessage.setOrigin(textBounds.width / 2, textBounds.height / 2);
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

