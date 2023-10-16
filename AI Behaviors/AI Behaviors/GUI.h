#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include "Global.h"

class GUI
{
public:

    GUI();
    ~GUI();

    void update();
    void render(sf::RenderWindow& m_window);
    void handleMouseClick(sf::Vector2i mousePosition);

private:

    void setupFontAndText();
    void setupSprite();

    sf::Font m_ArialBlackfont;
    sf::Text m_welcomeMessage;
};

