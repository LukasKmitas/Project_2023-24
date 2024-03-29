#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include "Global.h"
#include "BuildingType.h"
#include "InfantryType.h"
#include "VehicleType.h"
#include "AirCraftType.h"

struct Button
{
    sf::Sprite m_sprite;
    sf::Text m_text;
    BuildingType m_buildingType;
    InfantryType m_infantryType;
    VehicleType m_vehicleType;
    AirCraftType m_airCraftType;
};

class SideBar
{
public:
    SideBar(BuildingType& m_selectedBuildingType);

    void update(sf::Time t_deltaTime);
    void render(sf::RenderWindow& m_window);

    void addBuildingButton(const sf::Texture& texture, BuildingType buildingType, int gridX, int gridY, const std::string& buttonText);
    void addInfantryButton(const sf::Texture& texture, InfantryType inftantryType, int gridX, int gridY, const std::string& buttonText);
    void addVehicleButton(const sf::Texture& texture, VehicleType vehicleType, int gridX, int gridY, const std::string& buttonText);
    void addAirCraftButton(const sf::Texture& texture, AirCraftType aircraftType, int gridX, int gridY, const std::string& buttonText);

    void clearButtons();

    std::vector<Button> m_buttons;

    // Buildings
    sf::Vector2f getRefineryIconPosition() const;
    sf::Vector2f getBarracksIconPosition() const;
    sf::Vector2f getVehicleIconPosition() const;
    sf::Vector2f getAirCraftIconPosition() const;
    // Vehicles
    sf::Vector2f getHarvesterIconPosition() const;
    sf::Vector2f getBuggyIconPosition() const;
    // Air
    sf::Vector2f getFirehawkIconPosition() const;
    sf::Vector2f getHammerHeadIconPosition() const;

    const sf::RectangleShape& getSideBarRect() const;
    const sf::Sprite& getSellSprite() const;

private:

    void setupSlider();
    void setupFont();
    void drawGrid(sf::RenderWindow& m_window);

    void initButton();

    BuildingType& m_selectedBuildingType;

    sf::Font m_font;
    sf::Text m_sellText;

    sf::RectangleShape m_background;
    sf::Texture m_backgroundTexture;
    sf::RectangleShape m_bottomBackground;
    sf::Texture m_bottomBackgroundTexture;

    sf::Sprite m_sellSprite;
    sf::Texture m_sellTexture;

    Button m_button;

    sf::Shader m_colorShiftShader;
    sf::Shader m_rippleShader;

    // Buildings
    sf::Vector2f m_refineryIconPosition;
    sf::Vector2f m_barracksIconPosition;
    sf::Vector2f m_vehicleIconPosition;
    sf::Vector2f m_airCraftIconPosition;
    // Vehicles
    sf::Vector2f m_harvesterIconPosition;
    sf::Vector2f m_buggyIconPosition;
    // Air
    sf::Vector2f m_hammerHeadIconPosition;
    sf::Vector2f m_firehawkIconPosition;

    int gridCols = 3;
    int gridRows = 6;
    float cellWidth = m_bottomBackground.getSize().x / gridCols;
    float cellHeight = m_bottomBackground.getSize().y / gridRows;

    float buttonWidth = m_bottomBackground.getSize().x / gridCols;
    float buttonHeight = m_bottomBackground.getSize().y / gridRows;
    float xPosition;
    float yPosition;

    sf::Vector2f textPosition = m_button.m_sprite.getPosition();
    sf::FloatRect textBounds = m_button.m_text.getLocalBounds();
};

