#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include "Global.h"
#include "BuildingType.h"
#include "SideBar.h"
#include "Headquarters.h"
#include "Refinery.h"

class GUI
{
public:

    Headquarters* m_headquarters = nullptr;
    BuildingType m_selectedBuildingType = BuildingType::None;

    GUI();
    ~GUI();

    void update();
    void render(sf::RenderWindow& m_window);
    void handleMouseClick(sf::Vector2i mousePosition, sf::RenderWindow& m_window);

private:

    SideBar m_sideBar;
    BuildingType m_buildingToPlace;

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

    bool m_showSlider = false;

};