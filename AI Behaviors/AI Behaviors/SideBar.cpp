#include "SideBar.h"

SideBar::SideBar()
{
    setupSlider();
}

void SideBar::render(sf::RenderWindow& m_window)
{
    m_window.draw(m_background);
    m_window.draw(m_bottomBackground);

    drawGrid(m_window, gridCols, gridRows);

    for (const auto& buttonData : m_buildingButtons)
    {
        m_window.draw(buttonData.first);
    }
}

void SideBar::addBuildingButton(const sf::Texture& texture, BuildingType buildingType, int gridX, int gridY)
{
    sf::Sprite button;
    button.setTexture(texture);
    if (buildingType == BuildingType::Harvester)
    {
        button.setTextureRect(sf::IntRect(0, 0, 32, 24));
    }
    else if (buildingType == BuildingType::Infantry)
    {
        button.setTextureRect(sf::IntRect(33, 0, 32, 24));
    }
    else
    {
        button.setTextureRect(sf::IntRect(66, 0, 32, 24));
    }
    button.setScale(3.65, 3.7);
    float buttonWidth = m_bottomBackground.getSize().x / gridCols;
    float buttonHeight = m_bottomBackground.getSize().y / gridRows;

    float xPosition = m_bottomBackground.getPosition().x + gridX * buttonWidth;
    float yPosition = m_bottomBackground.getPosition().y + gridY * buttonHeight;

    button.setPosition(xPosition, yPosition);
    m_buildingButtons.push_back({ button, buildingType });
}

void SideBar::addUnitButton(const sf::Texture& texture, UnitType unitType)
{
    sf::Sprite button;
    button.setTexture(texture);
    button.setPosition(Global::S_WIDTH - 375, Global::S_HEIGHT / 2);

    m_unitButtons.push_back({ button, unitType });
}

void SideBar::setupSlider()
{
    m_background.setSize(sf::Vector2f(400, Global::S_HEIGHT - 20));
    m_background.setFillColor(sf::Color(128, 128, 128, 255));
    m_background.setOutlineColor(sf::Color(200, 100, 100, 200));
    m_background.setOutlineThickness(3);
    m_background.setPosition(Global::S_WIDTH - 400, 23);

    m_bottomBackground.setSize(sf::Vector2f(350, Global::S_HEIGHT / 2));
    m_bottomBackground.setFillColor(sf::Color(140, 140, 140, 255));
    m_bottomBackground.setOutlineColor(sf::Color(200, 100, 100, 200));
    m_bottomBackground.setOutlineThickness(3);
    m_bottomBackground.setPosition(Global::S_WIDTH - 375, Global::S_HEIGHT / 2);
}

void SideBar::drawGrid(sf::RenderWindow& window, int gridCols, int gridRows)
{
    // Calculate cell width and height
    float cellWidth = m_bottomBackground.getSize().x / gridCols;
    float cellHeight = m_bottomBackground.getSize().y / gridRows;

    // Set the grid line color and thickness
    sf::Color gridColor = sf::Color(100, 100, 100, 255);
    float gridThickness = 2.0f;

    for (int i = 1; i < gridCols; ++i) {
        // Draw vertical grid lines
        sf::Vertex line[] = {
            sf::Vertex(sf::Vector2f(m_bottomBackground.getPosition().x + i * cellWidth, m_bottomBackground.getPosition().y), gridColor),
            sf::Vertex(sf::Vector2f(m_bottomBackground.getPosition().x + i * cellWidth, m_bottomBackground.getPosition().y + m_bottomBackground.getSize().y), gridColor)
        };
        window.draw(line, 2, sf::Lines);
    }

    for (int i = 1; i < gridRows; ++i) {
        // Draw horizontal grid lines
        sf::Vertex line[] = {
            sf::Vertex(sf::Vector2f(m_bottomBackground.getPosition().x, m_bottomBackground.getPosition().y + i * cellHeight), gridColor),
            sf::Vertex(sf::Vector2f(m_bottomBackground.getPosition().x + m_bottomBackground.getSize().x, m_bottomBackground.getPosition().y + i * cellHeight), gridColor)
        };
        window.draw(line, 2, sf::Lines);
    }
}
