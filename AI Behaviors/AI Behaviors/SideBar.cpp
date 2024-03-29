#include "SideBar.h"

SideBar::SideBar(BuildingType& m_selectedBuildingType)
    :
    m_selectedBuildingType(m_selectedBuildingType)
{
    setupFont();
    initButton();
    setupSlider();
}

void SideBar::update(sf::Time t_deltaTime)
{
    static float time = 0.0f;
    time += t_deltaTime.asSeconds();
    m_colorShiftShader.setUniform("time", time);
    /*m_rippleShader.setUniform("center", sf::Glsl::Vec2(0.5, 0.5));
    m_rippleShader.setUniform("frequency", 10.0f);
    m_rippleShader.setUniform("amplitude", 0.05f);*/
}

void SideBar::render(sf::RenderWindow& m_window)
{
    m_window.draw(m_background);
    m_window.draw(m_bottomBackground);
   
    m_window.draw(m_sellSprite, &m_colorShiftShader);
    m_window.draw(m_sellText);

    drawGrid(m_window);

    for (const auto& buttonData : m_buttons)
    {
        m_window.draw(buttonData.m_sprite);
        m_window.draw(buttonData.m_text);
    }
}

void SideBar::addBuildingButton(const sf::Texture& texture, BuildingType buildingType, int gridX, int gridY, const std::string& buttonText)
{
    m_button.m_sprite.setTexture(texture);
    m_button.m_sprite.setScale(0.82, 0.81);

    buttonWidth = m_bottomBackground.getSize().x / gridCols;
    buttonHeight = m_bottomBackground.getSize().y / gridRows;
    xPosition = (m_bottomBackground.getPosition().x + gridX * buttonWidth) + 2;
    yPosition = (m_bottomBackground.getPosition().y + gridY * buttonHeight) + 2;

    if (buildingType == BuildingType::Refinery)
    {
        m_button.m_sprite.setTextureRect(sf::IntRect(250, 277, 120, 92));
        m_refineryIconPosition = sf::Vector2f(xPosition, yPosition);
    }
    else if (buildingType == BuildingType::Barracks)
    {
        m_button.m_sprite.setTextureRect(sf::IntRect(250, 649, 120, 92));
        m_barracksIconPosition = sf::Vector2f(xPosition, yPosition);
    }
    else if (buildingType == BuildingType::WarFactory)
    {
        m_button.m_sprite.setTextureRect(sf::IntRect(0, 0, 64, 48));
        m_button.m_sprite.setScale(1.53, 1.56);
        m_vehicleIconPosition = sf::Vector2f(xPosition, yPosition);
    }
    else
    {
        m_button.m_sprite.setTextureRect(sf::IntRect(250, 464, 120, 92));
        m_airCraftIconPosition = sf::Vector2f(xPosition, yPosition);
    }

    m_button.m_sprite.setPosition(xPosition, yPosition);
    m_button.m_text.setString(buttonText);
    textPosition = m_button.m_sprite.getPosition();
    textPosition.x += m_button.m_sprite.getGlobalBounds().width / 2;
    textPosition.y += m_button.m_sprite.getGlobalBounds().height - 8;
    textBounds = m_button.m_text.getLocalBounds();
    m_button.m_text.setOrigin(textBounds.width / 2, textBounds.height / 2);
    m_button.m_text.setPosition(textPosition);
    m_button.m_buildingType = buildingType;

    m_buttons.push_back(m_button);
}

void SideBar::addInfantryButton(const sf::Texture& texture, InfantryType inftantryType, int gridX, int gridY, const std::string& buttonText)
{
    m_button.m_sprite.setTexture(texture);
    m_button.m_sprite.setScale(3.26, 3.39);

     buttonWidth = m_bottomBackground.getSize().x / gridCols;
     buttonHeight = m_bottomBackground.getSize().y / gridRows;
     xPosition = (m_bottomBackground.getPosition().x + gridX * buttonWidth) + 2;
     yPosition = (m_bottomBackground.getPosition().y + gridY * buttonHeight) + 2;

    if (inftantryType == InfantryType::RifleSquad)
    {
        m_button.m_sprite.setTextureRect(sf::IntRect(1, 1, 30, 22));
        m_riflemanIconPosition = sf::Vector2f(xPosition, yPosition);
    }
    else if (inftantryType == InfantryType::GrenadeSquad)
    {
        m_button.m_sprite.setTextureRect(sf::IntRect(34, 1, 30, 22));
    }
    else if (inftantryType == InfantryType::RocketSquad)
    {
        m_button.m_sprite.setTextureRect(sf::IntRect(67, 1, 30, 22));
    }
    else
    {
        m_button.m_sprite.setTextureRect(sf::IntRect(133, 1, 30, 22));
    }

    m_button.m_sprite.setPosition(xPosition, yPosition);
    m_button.m_text.setString(buttonText);
    textPosition = m_button.m_sprite.getPosition();
    textPosition.x += m_button.m_sprite.getGlobalBounds().width / 2;
    textPosition.y += m_button.m_sprite.getGlobalBounds().height - 8;
    textBounds = m_button.m_text.getLocalBounds();
    m_button.m_text.setOrigin(textBounds.width / 2, textBounds.height / 2);
    m_button.m_text.setPosition(textPosition);
    m_button.m_infantryType = inftantryType;

    m_buttons.push_back(m_button);
}

void SideBar::addVehicleButton(const sf::Texture& texture, VehicleType vehicleType, int gridX, int gridY, const std::string& buttonText)
{
    m_button.m_sprite.setTexture(texture);
    m_button.m_sprite.setScale(3.26, 3.39);

     buttonWidth = m_bottomBackground.getSize().x / gridCols;
     buttonHeight = m_bottomBackground.getSize().y / gridRows;
     xPosition = (m_bottomBackground.getPosition().x + gridX * buttonWidth) + 2;
     yPosition = (m_bottomBackground.getPosition().y + gridY * buttonHeight) + 2;

    if (vehicleType == VehicleType::Harvester)
    {
        m_button.m_sprite.setScale(0.096, 0.075);
        m_button.m_sprite.setTextureRect(sf::IntRect(0, 0, 1024, 1000));
        m_harvesterIconPosition = sf::Vector2f(xPosition, yPosition);
    }
    else if (vehicleType == VehicleType::Buggy)
    {
        m_button.m_sprite.setTextureRect(sf::IntRect(133, 1, 30, 22));
        m_buggyIconPosition = sf::Vector2f(xPosition, yPosition);
    }
    else
    {
        m_button.m_sprite.setTextureRect(sf::IntRect(1, 1, 30, 22));
        m_tankAuroraIconPosition = sf::Vector2f(xPosition, yPosition);
    }

    m_button.m_sprite.setPosition(xPosition, yPosition);
    m_button.m_text.setString(buttonText);
    textPosition = m_button.m_sprite.getPosition();
    textPosition.x += m_button.m_sprite.getGlobalBounds().width / 2;
    textPosition.y += m_button.m_sprite.getGlobalBounds().height - 8;
    textBounds = m_button.m_text.getLocalBounds();
    m_button.m_text.setOrigin(textBounds.width / 2, textBounds.height / 2);
    m_button.m_text.setPosition(textPosition);
    m_button.m_vehicleType = vehicleType;

    m_buttons.push_back(m_button);
}

void SideBar::addAirCraftButton(const sf::Texture& texture, AirCraftType aircraftType, int gridX, int gridY, const std::string& buttonText)
{
    m_button.m_sprite.setTexture(texture);
    m_button.m_sprite.setScale(3.26, 3.39);

    buttonWidth = m_bottomBackground.getSize().x / gridCols;
    buttonHeight = m_bottomBackground.getSize().y / gridRows;
    xPosition = (m_bottomBackground.getPosition().x + gridX * buttonWidth) + 2;
    yPosition = (m_bottomBackground.getPosition().y + gridY * buttonHeight) + 2;

    if (aircraftType == AirCraftType::HammerHead)
    {
        m_button.m_sprite.setTextureRect(sf::IntRect(34, 1, 30, 22));
        m_hammerHeadIconPosition = sf::Vector2f(xPosition, yPosition);
    }
    else
    {
        m_button.m_sprite.setTextureRect(sf::IntRect(100, 1, 30, 22));
        m_firehawkIconPosition = sf::Vector2f(xPosition, yPosition);
    }

    m_button.m_sprite.setPosition(xPosition, yPosition);
    m_button.m_text.setString(buttonText);
    textPosition = m_button.m_sprite.getPosition();
    textPosition.x += m_button.m_sprite.getGlobalBounds().width / 2;
    textPosition.y += m_button.m_sprite.getGlobalBounds().height - 10;
    textBounds = m_button.m_text.getLocalBounds();
    m_button.m_text.setOrigin(textBounds.width / 2, textBounds.height / 2);
    m_button.m_text.setPosition(textPosition);
    m_button.m_airCraftType = aircraftType;

    m_buttons.push_back(m_button);
}

sf::Vector2f SideBar::getRefineryIconPosition() const
{
    return m_refineryIconPosition;
}

sf::Vector2f SideBar::getBarracksIconPosition() const
{
    return m_barracksIconPosition;
}

sf::Vector2f SideBar::getVehicleIconPosition() const
{
    return m_vehicleIconPosition;
}

sf::Vector2f SideBar::getAirCraftIconPosition() const
{
    return m_airCraftIconPosition;
}

sf::Vector2f SideBar::getRiflemanIconPosition() const
{
    return m_riflemanIconPosition;
}

sf::Vector2f SideBar::getHarvesterIconPosition() const
{
    return m_harvesterIconPosition;
}

sf::Vector2f SideBar::getBuggyIconPosition() const
{
    return m_buggyIconPosition;
}

sf::Vector2f SideBar::getTankAuroraIconPosition() const
{
    return m_tankAuroraIconPosition;
}

sf::Vector2f SideBar::getFirehawkIconPosition() const
{
    return m_firehawkIconPosition;
}

sf::Vector2f SideBar::getHammerHeadIconPosition() const
{
    return m_hammerHeadIconPosition;
}

const sf::RectangleShape& SideBar::getSideBarRect() const
{
    return m_background;
}

const sf::Sprite& SideBar::getSellSprite() const
{
    return m_sellSprite;
}

void SideBar::setupSlider()
{
    if (!m_backgroundTexture.loadFromFile("Assets\\Images\\GUI\\Panel_1.png"))
    {
        std::cout << "Error - problem loading font" << std::endl;
    }
    if (!m_bottomBackgroundTexture.loadFromFile("Assets\\Images\\GUI\\Card X2.png"))
    {
        std::cout << "Error - problem loading font" << std::endl;
    }
    if (!m_sellTexture.loadFromFile("Assets\\Images\\GUI\\GenericPanel.png"))
    {
        std::cout << "Error - problem loading font" << std::endl;
    }
    // Shaders
    if (!m_colorShiftShader.loadFromFile("Assets\\Shaders\\colorShiftShader.frag", sf::Shader::Fragment)) 
    {
        std::cout << "Error - Failed to load shader" << std::endl;
    }
    if (!m_rippleShader.loadFromFile("Assets/Shaders/rippleShader.frag", sf::Shader::Fragment))
    {
        std::cout << "Error - Failed to load fragment shader" << std::endl;
    }

    m_background.setSize(sf::Vector2f(400, Global::S_HEIGHT - 50));
    m_background.setOutlineColor(sf::Color(200, 100, 100, 200));
    m_background.setOutlineThickness(2);
    m_background.setPosition(Global::S_WIDTH - 400, 53);
    m_background.setTexture(&m_backgroundTexture);

    m_bottomBackground.setSize(sf::Vector2f(300, Global::S_HEIGHT - 620));
    m_bottomBackground.setFillColor(sf::Color(150, 150, 150, 255));
    m_bottomBackground.setOutlineColor(sf::Color(185, 232, 234));
    m_bottomBackground.setOutlineThickness(2);
    m_bottomBackground.setPosition(Global::S_WIDTH - 350, Global::S_HEIGHT / 2);
    m_bottomBackground.setTexture(&m_bottomBackgroundTexture);

    m_sellSprite.setPosition(Global::S_WIDTH - 270, 400);
    m_sellSprite.setTexture(m_sellTexture);
    m_sellSprite.setOrigin(m_sellSprite.getLocalBounds().width / 2, m_sellSprite.getLocalBounds().height / 2);
    m_sellSprite.setScale(2, 2);
    m_sellText.setFont(m_font);
    m_sellText.setString("Sell");
    m_sellText.setPosition(m_sellSprite.getPosition().x, m_sellSprite.getPosition().y - 10);
    m_sellText.setOrigin(m_sellText.getGlobalBounds().width / 2, m_sellText.getGlobalBounds().height / 2);
    m_sellText.setFillColor(sf::Color(225, 245, 255));
    m_sellText.setOutlineColor(sf::Color::Blue);
    m_sellText.setOutlineThickness(1);


}

void SideBar::setupFont()
{
    if (!m_font.loadFromFile("Assets\\Fonts\\ManicSea_19.ttf"))
    {
        std::cout << "problem loading font" << std::endl;
    }
}

void SideBar::drawGrid(sf::RenderWindow& m_window)
{
    float cellWidth = m_bottomBackground.getSize().x / gridCols;
    float cellHeight = m_bottomBackground.getSize().y / gridRows;

    sf::Color gridColor = sf::Color(63, 208, 212);
    float gridThickness = 2.0f;

    // Vertical lines
    for (int i = 1; i < gridCols; ++i)
    {
        sf::RectangleShape line(sf::Vector2f(gridThickness, m_bottomBackground.getSize().y));
        line.setPosition(m_bottomBackground.getPosition().x + i * cellWidth, m_bottomBackground.getPosition().y);
        line.setFillColor(gridColor);
        m_window.draw(line);
    }

    // Horizontal lines
    for (int i = 1; i < gridRows; ++i)
    {
        sf::RectangleShape line(sf::Vector2f(m_bottomBackground.getSize().x, gridThickness));
        line.setPosition(m_bottomBackground.getPosition().x, m_bottomBackground.getPosition().y + i * cellHeight);
        line.setFillColor(gridColor);
        m_window.draw(line);
    }
}

void SideBar::initButton()
{
    m_button.m_text.setFont(m_font);
    m_button.m_text.setCharacterSize(12);
    m_button.m_text.setFillColor(sf::Color::White);
    m_button.m_text.setOutlineColor(sf::Color::Black);
    m_button.m_text.setOutlineThickness(1);
    m_button.m_text.setStyle(sf::Text::Bold);
}

void SideBar::clearButtons()
{
    m_buttons.clear();
}
