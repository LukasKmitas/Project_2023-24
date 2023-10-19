#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>

class SideBar
{
public:
    SideBar();

    void render(sf::RenderWindow& window);

    void setPosition(float x, float y);
    void addIcon(const sf::Texture& texture, const sf::Vector2f& position);

    int handleMouseClick(const sf::Vector2f& mousePosition);

private:

    sf::RectangleShape m_background;
    std::vector<sf::Sprite> m_icons;

};

