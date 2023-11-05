#include "SideBar.h"

SideBar::SideBar()
{
    setupSlider();
    setupFont();

    //button.setTextureRect(sf::IntRect(0, 0, 32, 24));
    //button.setTextureRect(sf::IntRect(33, 0, 32, 24));
    //button.setTextureRect(sf::IntRect(66, 0, 32, 24));
    //button.m_sprite.setScale(3.65, 3.7);
}

void SideBar::render(sf::RenderWindow& m_window)
{
    m_window.draw(m_background);
    m_window.draw(m_bottomBackground);

    drawGrid(m_window, gridCols, gridRows);

    for (const auto& buttonData : m_buttons)
    {
        m_window.draw(buttonData.m_sprite);
        m_window.draw(buttonData.m_text);
    }
}

void SideBar::addBuildingButton(const sf::Texture& texture, BuildingType buildingType, int gridX, int gridY, const std::string& buttonText)
{
    Button button;
    button.m_sprite.setTexture(texture);
    button.m_sprite.setScale(0.97, 0.97);

    float buttonWidth = m_bottomBackground.getSize().x / gridCols;
    float buttonHeight = m_bottomBackground.getSize().y / gridRows;
    float xPosition = m_bottomBackground.getPosition().x + gridX * buttonWidth;
    float yPosition = m_bottomBackground.getPosition().y + gridY * buttonHeight;

    if (buildingType == BuildingType::Refinery)
    {
        button.m_sprite.setTextureRect(sf::IntRect(250, 277, 120, 92));
        m_refineryIconPosition = sf::Vector2f(xPosition, yPosition);
    }
    else if (buildingType == BuildingType::Barracks)
    {
        button.m_sprite.setTextureRect(sf::IntRect(250, 649, 120, 92));
    }
    else if (buildingType == BuildingType::Vehicle)
    {
        button.m_sprite.setTextureRect(sf::IntRect(0, 0, 64, 48));
        button.m_sprite.setScale(1.82, 1.85);
    }
    else
    {
        button.m_sprite.setTextureRect(sf::IntRect(250, 464, 120, 92));
    }

    button.m_sprite.setPosition(xPosition, yPosition);

    //Text
    button.m_text.setFont(m_buttonFont);
    button.m_text.setString(buttonText);
    button.m_text.setCharacterSize(16);
    button.m_text.setFillColor(sf::Color::White);
    button.m_text.setOutlineColor(sf::Color::Black);
    button.m_text.setOutlineThickness(1);
    button.m_text.setStyle(sf::Text::Bold);

    sf::Vector2f textPosition = button.m_sprite.getPosition();
    textPosition.x += button.m_sprite.getGlobalBounds().width / 2;
    textPosition.y += button.m_sprite.getGlobalBounds().height - 10;
    sf::FloatRect textBounds = button.m_text.getLocalBounds();
    button.m_text.setOrigin(textBounds.width / 2, textBounds.height / 2);

    button.m_text.setPosition(textPosition);

    // Store the building type
    button.m_buildingType = buildingType;

    m_buttons.push_back(button);
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

void SideBar::setupFont()
{
    if (!m_buttonFont.loadFromFile("Assets\\Fonts\\ManicSea_19.ttf"))
    {
        std::cout << "problem loading font" << std::endl;
    }
}

void SideBar::drawGrid(sf::RenderWindow& window, int gridCols, int gridRows)
{
    float cellWidth = m_bottomBackground.getSize().x / gridCols;
    float cellHeight = m_bottomBackground.getSize().y / gridRows;

    sf::Color gridColor = sf::Color(100, 100, 100, 255);
    float gridThickness = 2.0f;

    for (int i = 1; i < gridCols; ++i) 
    {
        sf::Vertex line[] = 
        {
            sf::Vertex(sf::Vector2f(m_bottomBackground.getPosition().x + i * cellWidth, m_bottomBackground.getPosition().y), gridColor),
            sf::Vertex(sf::Vector2f(m_bottomBackground.getPosition().x + i * cellWidth, m_bottomBackground.getPosition().y + m_bottomBackground.getSize().y), gridColor)
        };
        window.draw(line, 2, sf::Lines);
    }
    for (int i = 1; i < gridRows; ++i) 
    {
        sf::Vertex line[] = 
        {
            sf::Vertex(sf::Vector2f(m_bottomBackground.getPosition().x, m_bottomBackground.getPosition().y + i * cellHeight), gridColor),
            sf::Vertex(sf::Vector2f(m_bottomBackground.getPosition().x + m_bottomBackground.getSize().x, m_bottomBackground.getPosition().y + i * cellHeight), gridColor)
        };
        window.draw(line, 2, sf::Lines);
    }
}
