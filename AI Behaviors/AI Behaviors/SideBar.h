#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include "Global.h"
#include "BuildingType.h"
#include "UnitType.h"

class SideBar
{
public:
    SideBar();

    void render(sf::RenderWindow& m_window);

    void addBuildingButton(const sf::Texture& texture, BuildingType buildingType, int gridX, int gridY);
    void addUnitButton(const sf::Texture& texture, UnitType unitType);

    std::vector<std::pair<sf::Sprite, BuildingType>> m_buildingButtons;
    std::vector<std::pair<sf::Sprite, UnitType>> m_unitButtons;

private:

    void setupSlider();
    void drawGrid(sf::RenderWindow& window, int gridCols, int gridRows);

    sf::RectangleShape m_background;
    sf::RectangleShape m_bottomBackground;

    int gridCols = 3;
    int gridRows = 6;
    float buttonWidth = m_bottomBackground.getSize().x / gridCols;
    float buttonHeight = m_bottomBackground.getSize().y / gridRows;
    float cellWidth = m_bottomBackground.getSize().x / gridCols;
    float cellHeight = m_bottomBackground.getSize().y / gridRows;


};

