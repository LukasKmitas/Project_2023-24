#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include "Global.h"
#include "BuildingType.h"
#include "Headquarters.h"

class GUI
{
public:
    Headquarters m_headquarters;
    BuildingType m_selectedBuildingType = BuildingType::Headquarters;

    GUI();
    ~GUI();

    void update();
    void render(sf::RenderWindow& m_window);
    void handleMouseClick(sf::Vector2i mousePosition);

private:

    void setupFontAndText();

    sf::Font m_ArialBlackfont;
    sf::Text m_welcomeMessage;

};