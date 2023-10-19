#include "GUI.h"

GUI::GUI()
{
	setupFontAndText();
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

	if (m_selectedBuildingType == BuildingType::Headquarters) 
	{
		std::cout << "Headquarters Slide" << std::endl;
		if (m_showSlider) 
		{
			m_sideBar.render(m_window);
		}
	}
}

void GUI::handleMouseClick(sf::Vector2i mousePosition, sf::RenderWindow& m_window)
{
	//Testing purposes
	std::cout << "Mouse position: (" << mousePosition.x << ", " << mousePosition.y << ")" << std::endl;

	sf::Vector2f worldMousePosition = m_window.mapPixelToCoords(mousePosition, m_window.getView());

	if (m_headquarters->getBuildingSprite().getGlobalBounds().contains(worldMousePosition))
	{
		m_selectedBuildingType = BuildingType::Headquarters;
		std::cout << "Headquarters Selected" << std::endl;
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