#pragma once
#include <iostream>
#include "SFML/Graphics.hpp"
#include "Global.h"

class Building
{
public:

    Building();
    virtual ~Building();

    virtual void update(sf::Time deltaTime);
    virtual void render(sf::RenderWindow& m_window) const;

    int getHealth() const;
    void setHealth(int health);
    void applyDamage(int damage);
    void setPosition(const sf::Vector2f& position);

    sf::Vector2f getPosition();
 
    int getCost() const;
    bool canAfford() const;
    void checkAffordability();

    const sf::Sprite& getBuildingSprite() const 
    {
        return m_buildingSprite;
    }

protected:

    sf::Vector2f m_position;
    std::string m_name;
    
    sf::Texture m_buildingTexture;
    sf::Sprite m_buildingSprite;

    int m_health = 100;
    int m_cost = 100;

};