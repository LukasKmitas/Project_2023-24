#include "GUI.h"

GUI::GUI()
{
	setupFontAndText();
	setupTopBar();

	if (!m_BuildingTexture1.loadFromFile("Assets\\Images\\InfantryUnitIcons.png"))
	{
		std::cout << "problem loading arial black font" << std::endl;
	}
	if (!m_BuildingTexture2.loadFromFile("Assets\\Images\\InfantryUnitIcons.png"))
	{
		std::cout << "problem loading arial black font" << std::endl;
	}
}

GUI::~GUI()
{
}

void GUI::update()
{
	
}

void GUI::render(sf::RenderWindow& m_window)
{
	m_window.draw(m_welcomeMessage);
	m_window.draw(m_topBar);
	
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
		m_showSlider = true;
		m_selectedBuildingType = BuildingType::Headquarters;

		if (m_selectedBuildingType == BuildingType::Headquarters)
		{
			/*m_sideBar.addBuildingButton(m_BuildingTexture1, BuildingType::Harvester);
			m_sideBar.addBuildingButton(m_BuildingTexture2, BuildingType::Infantry);*/
			m_sideBar.addBuildingButton(m_BuildingTexture1, BuildingType::Harvester, 0, 0);
			m_sideBar.addBuildingButton(m_BuildingTexture1, BuildingType::Infantry, 1, 0);
			m_sideBar.addBuildingButton(m_BuildingTexture1, BuildingType::Heavy, 2, 0);
		}

	}
	else
	{
		m_showSlider = false;
		m_selectedBuildingType = BuildingType::None;
	}
}

void GUI::setupFontAndText()
{
	if (!m_ArialBlackfont.loadFromFile("Assets\\Fonts\\ManicSea_19.ttf"))
	{
		std::cout << "problem loading arial black font" << std::endl;
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
}

