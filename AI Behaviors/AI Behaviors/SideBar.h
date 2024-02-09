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

    void render(sf::RenderWindow& m_window);

    void addBuildingButton(const sf::Texture& texture, BuildingType buildingType, int gridX, int gridY, const std::string& buttonText);
    void addInfantryButton(const sf::Texture& texture, InfantryType inftantryType, int gridX, int gridY, const std::string& buttonText);
    void addVehicleButton(const sf::Texture& texture, VehicleType vehicleType, int gridX, int gridY, const std::string& buttonText);
    void addAirCraftButton(const sf::Texture& texture, AirCraftType aircraftType, int gridX, int gridY, const std::string& buttonText);

    std::vector<Button> m_buttons;

    sf::Vector2f getRefineryIconPosition() const 
    {
        return m_refineryIconPosition;
    }
    sf::Vector2f getBarracksIconPosition() const
    {
        return m_barracksIconPosition;
    }
    sf::Vector2f getVehicleIconPosition() const
    {
        return m_vehicleIconPosition;
    }
    sf::Vector2f getAirCraftIconPosition() const
    {
        return m_airCraftIconPosition;
    }
    sf::Vector2f getHarvesterIconPosition() const
    {
        return m_harvesterIconPosition;
    }
    sf::Vector2f getFirehawkIconPosition() const
    {
        return m_harvesterIconPosition;
    }

    const sf::RectangleShape& getSideBarRect() const
    {
        return m_background;
    }

private:

    void setupSlider();
    void setupFont();
    void drawGrid(sf::RenderWindow& m_window);

    BuildingType& m_selectedBuildingType;

    sf::Font m_buttonFont;

    sf::RectangleShape m_background;
    sf::RectangleShape m_bottomBackground;
    // Buildings
    sf::Vector2f m_refineryIconPosition;
    sf::Vector2f m_barracksIconPosition;
    sf::Vector2f m_vehicleIconPosition;
    sf::Vector2f m_airCraftIconPosition;
    // Vehicles
    sf::Vector2f m_harvesterIconPosition;
    // Air
    sf::Vector2f m_fireHawkIconPosition;

    int gridCols = 3;
    int gridRows = 6;
    float cellWidth = m_bottomBackground.getSize().x / gridCols;
    float cellHeight = m_bottomBackground.getSize().y / gridRows;

};

