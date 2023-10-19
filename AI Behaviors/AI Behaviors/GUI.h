#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include "Global.h"
#include "BuildingType.h"
#include "Headquarters.h"

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

    void setupFontAndText();

    sf::Font m_ArialBlackfont;
    sf::Text m_welcomeMessage;

};