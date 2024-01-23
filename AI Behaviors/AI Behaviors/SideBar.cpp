#include "SideBar.h"

SideBar::SideBar(BuildingType& m_selectedBuildingType)
    :
    m_selectedBuildingType(m_selectedBuildingType)
{
    setupSlider();
    setupFont();
}

void SideBar::render(sf::RenderWindow& m_window)
{
    m_window.draw(m_background);
    m_window.draw(m_bottomBackground);

    drawGrid(m_window);

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
        m_barracksIconPosition = sf::Vector2f(xPosition, yPosition);
    }
    else if (buildingType == BuildingType::WarFactory)
    {
        button.m_sprite.setTextureRect(sf::IntRect(0, 0, 64, 48));
        button.m_sprite.setScale(1.82, 1.85);
        m_vehicleIconPosition = sf::Vector2f(xPosition, yPosition);
    }
    else
    {
        button.m_sprite.setTextureRect(sf::IntRect(250, 464, 120, 92));
        m_airCraftIconPosition = sf::Vector2f(xPosition, yPosition);
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

void SideBar::addInfantryButton(const sf::Texture& texture, InfantryType inftantryType, int gridX, int gridY, const std::string& buttonText)
{
    Button button;
    button.m_sprite.setTexture(texture);
    button.m_sprite.setScale(3.65, 3.7);

    float buttonWidth = m_bottomBackground.getSize().x / gridCols;
    float buttonHeight = m_bottomBackground.getSize().y / gridRows;
    float xPosition = m_bottomBackground.getPosition().x + gridX * buttonWidth;
    float yPosition = m_bottomBackground.getPosition().y + gridY * buttonHeight;

    if (inftantryType == InfantryType::RifleSquad)
    {
        button.m_sprite.setTextureRect(sf::IntRect(0, 0, 32, 24));
    }
    else if (inftantryType == InfantryType::GrenadeSquad)
    {
        button.m_sprite.setTextureRect(sf::IntRect(33, 0, 32, 24));
    }
    else if (inftantryType == InfantryType::RocketSquad)
    {
        button.m_sprite.setTextureRect(sf::IntRect(66, 0, 32, 24));
    }
    else
    {
        button.m_sprite.setTextureRect(sf::IntRect(132, 0, 32, 24));
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
    button.m_infantryType = inftantryType;

    m_buttons.push_back(button);
}

void SideBar::addVehicleButton(const sf::Texture& texture, VehicleType vehicleType, int gridX, int gridY, const std::string& buttonText)
{
    Button button;
    button.m_sprite.setTexture(texture);
    button.m_sprite.setScale(3.65, 3.7);

    float buttonWidth = m_bottomBackground.getSize().x / gridCols;
    float buttonHeight = m_bottomBackground.getSize().y / gridRows;
    float xPosition = m_bottomBackground.getPosition().x + gridX * buttonWidth;
    float yPosition = m_bottomBackground.getPosition().y + gridY * buttonHeight;

    if (vehicleType == VehicleType::Harvester)
    {
        button.m_sprite.setScale(0.113, 0.087);
    }
    else if (vehicleType == VehicleType::Ranger)
    {
        button.m_sprite.setTextureRect(sf::IntRect(132, 0, 32, 24));
    }
    else if (vehicleType == VehicleType::Tank)
    {
        button.m_sprite.setTextureRect(sf::IntRect(0, 0, 32, 24));
    }
    else if (vehicleType == VehicleType::Artillery)
    {
        button.m_sprite.setTextureRect(sf::IntRect(0, 25, 32, 24));
    }
    else
    {
        button.m_sprite.setTextureRect(sf::IntRect(500, 0, 32, 24));
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
    button.m_vehicleType = vehicleType;

    m_buttons.push_back(button);
}

void SideBar::addAirCraftButton(const sf::Texture& texture, AirCraftType aircraftType, int gridX, int gridY, const std::string& buttonText)
{
    Button button;
    button.m_sprite.setTexture(texture);
    button.m_sprite.setScale(3.65, 3.7);

    float buttonWidth = m_bottomBackground.getSize().x / gridCols;
    float buttonHeight = m_bottomBackground.getSize().y / gridRows;
    float xPosition = m_bottomBackground.getPosition().x + gridX * buttonWidth;
    float yPosition = m_bottomBackground.getPosition().y + gridY * buttonHeight;

    if (aircraftType == AirCraftType::Orca)
    {
        button.m_sprite.setTextureRect(sf::IntRect(0, 0, 32, 24));
    }
    else if (aircraftType == AirCraftType::HammerHead)
    {
        button.m_sprite.setTextureRect(sf::IntRect(33, 0, 32, 24));
    }
    else if (aircraftType == AirCraftType::Firehawk)
    {
        button.m_sprite.setTextureRect(sf::IntRect(99, 0, 32, 24));
    }
    else
    {
        button.m_sprite.setTextureRect(sf::IntRect(500, 0, 32, 24));
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
    button.m_airCraftType = aircraftType;

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

void SideBar::drawGrid(sf::RenderWindow& m_window)
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
        m_window.draw(line, 2, sf::Lines);
    }
    for (int i = 1; i < gridRows; ++i) 
    {
        sf::Vertex line[] = 
        {
            sf::Vertex(sf::Vector2f(m_bottomBackground.getPosition().x, m_bottomBackground.getPosition().y + i * cellHeight), gridColor),
            sf::Vertex(sf::Vector2f(m_bottomBackground.getPosition().x + m_bottomBackground.getSize().x, m_bottomBackground.getPosition().y + i * cellHeight), gridColor)
        };
        m_window.draw(line, 2, sf::Lines);
    }
}
