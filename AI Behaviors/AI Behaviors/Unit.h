#pragma once
#include <iostream>
#include "SFML/Graphics.hpp"

class Unit
{
public:

    Unit();
    virtual ~Unit();

    virtual void update(float deltaTime);
    virtual void render(sf::RenderWindow& m_window) const;

    virtual void move(const sf::Vector2f& direction);
    virtual void attack(Unit* target);

    void setPosition(const sf::Vector2f& position);

protected:

    sf::Texture m_unitTexture;
    sf::Sprite m_unitSprite;

    int m_cost = 100;

    float m_health = 100;

    sf::Vector2f m_position;

};

