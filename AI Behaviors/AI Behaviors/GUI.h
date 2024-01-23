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
#include "WarFactory.h"
#include "AirCraft.h"
#include "Tile.h"
#include "LevelEditor.h"

class GUI
{
public:

    Refinery* m_refinery = nullptr;
    Barracks* m_barracks = nullptr;
    WarFactory* m_warFactory = nullptr;
    AirCraft* m_airCraft = nullptr;
    InfantryType m_selectedUnitType = InfantryType::None;
    LevelEditor levelEditor;

    std::vector<Building*>& placedBuildings;
    BuildingType& m_selectedBuildingType;

    GUI(std::vector<Building*>& m_buildings, BuildingType& m_selectedBuildingType, std::vector<std::vector<Tile>>& m_tiles);
    ~GUI();

    void update(sf::Time t_deltaTime);
    void render(sf::RenderWindow& m_window);
    void handleMouseClick(sf::Vector2i m_mousePosition, sf::RenderWindow& m_window);
    void handleBuildingPlacement(sf::Vector2i m_mousePosition, sf::RenderWindow& m_window);
    void handleBuildingSelection(sf::Vector2f m_mousePosition);

    bool m_confirmBuildingPlacement = false;
    bool m_confirmed = false;

    bool IsPlacementValid(sf::Vector2f& m_position, sf::RenderWindow& m_window);
    bool IsPlacementValidForTiles(sf::Vector2f& m_position);

private:

    Tile m_tile;
    SideBar m_sideBar{m_selectedBuildingType};

    std::vector<std::vector<Tile>>& m_tilesReference;

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
    bool m_placementValid = false;
};