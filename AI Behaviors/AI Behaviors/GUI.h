#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include "Global.h"
#include "BuildingType.h"
#include "InfantryType.h"

#include "SideBar.h"
#include "Headquarters.h"
#include "Refinery.h"
#include "Barracks.h"
#include "Vehicle.h"
#include "AirCraft.h"

class GUI
{
public:

    Refinery* m_refinery = nullptr;
    Barracks* m_barracks = nullptr;
    Vehicle* m_vehicle = nullptr;
    AirCraft* m_airCraft = nullptr;
    InfantryType m_selectedUnitType = InfantryType::None;

    std::vector<Building*>& placedBuildings;
    BuildingType& m_selectedBuildingType;

    GUI(std::vector<Building*>& buildings, BuildingType& selectedBuildingType);
    ~GUI();

    void update(sf::Time t_deltaTime);
    void render(sf::RenderWindow& m_window);
    void handleMouseClick(sf::Vector2i mousePosition, sf::RenderWindow& m_window);
    void handleBuildingPlacement(sf::Vector2i mousePosition, sf::RenderWindow& window);
    void handleBuildingSelection(sf::Vector2f mousePosition);

    bool m_confirmBuildingPlacement = false;
    bool m_confirmed = false;

    bool IsPlacementValid(const sf::Vector2f& position)
    {
        sf::FloatRect newBuildingBounds = m_ghostBuildingSprite.getGlobalBounds();
        for (Building* building : placedBuildings)
        {
            sf::FloatRect existingBuildingBounds = building->getBuildingSprite().getGlobalBounds();
            if (newBuildingBounds.intersects(existingBuildingBounds))
            {
                return false;
            }
        }
        return true;
    }


private:

    SideBar m_sideBar{m_selectedBuildingType};

    void updateCurrency();
    void setupTopBar();

    sf::View m_guiView;

    sf::RectangleShape m_topBar;

    sf::Font m_currencyFont;
    sf::Text m_currencyText;

    sf::Texture m_BuildingTexture1;
    sf::Texture m_BuildingTexture2;
    sf::Texture m_unitInfantryTexture;
    sf::Texture m_unitVehicleTexture;
    sf::Texture m_unitAirCraftTexture;

    sf::Sprite m_ghostBuildingSprite;
    sf::Texture m_ghostBuildingTexture;

    bool m_showSlider = false;
};