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

class GUI
{
public:

    Headquarters* m_headquarters = nullptr;
    Refinery* m_refinery = nullptr;
    Barracks* m_barracks = nullptr;
    BuildingType m_selectedBuildingType = BuildingType::None;
    InfantryType m_selectedUnitType = InfantryType::None;

    GUI();
    ~GUI();

    void update(sf::Time t_deltaTime);
    void render(sf::RenderWindow& m_window);
    void handleMouseClick(sf::Vector2i mousePosition, sf::RenderWindow& m_window, BuildingType& selectedBuildingType);
    void handleBuildingPlacement(sf::Vector2i mousePosition, sf::RenderWindow& window);

    bool m_confirmBuildingPlacement = false;
    bool m_confirmed = false;

    bool IsPlacementValid(const sf::Vector2f& position)
    {

        return true;
    }


private:

    SideBar m_sideBar;

    void updateCurrency();
    void setupFontAndText();
    void setupTopBar();

    sf::View m_guiView;

    sf::RectangleShape m_topBar;

    sf::Font m_ArialBlackfont;
    sf::Text m_welcomeMessage;

    sf::Font m_currencyFont;
    sf::Text m_currencyText;

    sf::Texture m_BuildingTexture1;
    sf::Texture m_BuildingTexture2;
    sf::Texture m_unitInfantryTexture;
    sf::Sprite m_buildingPreviewSprite;

    

    bool m_showSlider = false;
};