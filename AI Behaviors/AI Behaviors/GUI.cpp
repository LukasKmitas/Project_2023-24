#include "GUI.h"

GUI::GUI()
{
	setupFontAndText();
	setupTopBar();

	if (!m_BuildingTexture1.loadFromFile("Assets\\Images\\BuildingIcons.png"))
	{
		std::cout << "problem loading arial black font" << std::endl;
	}
	if (!m_BuildingTexture2.loadFromFile("Assets\\Images\\VehicleIcon.png"))
	{
		std::cout << "problem loading arial black font" << std::endl;
	}
	m_guiView.setSize(Global::S_WIDTH, Global::S_HEIGHT);
	m_guiView.setCenter(Global::S_WIDTH / 2, Global::S_HEIGHT / 2);
	Global::currency = 1000;
}

GUI::~GUI()
{
}

void GUI::update()
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
}

void GUI::handleMouseClick(sf::Vector2i mousePosition, sf::RenderWindow& m_window)
{
	//Testing purposes
	std::cout << "Mouse position: (" << mousePosition.x << ", " << mousePosition.y << ")" << std::endl;

	sf::Vector2f worldMousePosition = m_window.mapPixelToCoords(mousePosition, m_window.getView());

	if (m_headquarters->getBuildingSprite().getGlobalBounds().contains(worldMousePosition))
	{
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
	else
	{
		m_selectedBuildingType = BuildingType::None;
	}

	if (m_showSlider)
	{
		sf::Vector2f refineryIconPosition = m_sideBar.getRefineryIconPosition();
		sf::FloatRect refineryIconBounds(refineryIconPosition, sf::Vector2f(120, 92));
		if (refineryIconBounds.contains(worldMousePosition))
		{
			std::cout << "Refinery Clicked" << std::endl;
			m_buildingToPlace = BuildingType::Refinery;
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

