#pragma once
#include <iostream>
#include "SFML/Graphics.hpp"
#include "Global.h"

class Building
{
public:

    Building();
    virtual ~Building();

    virtual void update(sf::Time t_deltaTime);
    virtual void render(sf::RenderWindow& m_window) const;

    int getHealth() const;
    void setHealth(int m_health);
    void applyDamage(int m_damage);
    void setPosition(const sf::Vector2f& m_position);

    sf::Vector2f getPosition();
 
    int getCost() const;
    bool checkAffordability();

    const sf::Sprite& getBuildingSprite() const 
    {
        return m_buildingSprite;
    }
    const sf::Texture& getBuildingTexture() const
    {
        return m_buildingTexture;
    }
    int getBuildingID() const 
    {
        return buildingID;
    }

protected:

    sf::Vector2f m_position;
    std::string m_name;
    
    sf::Texture m_buildingTexture;
    sf::Sprite m_buildingSprite;

    int m_health = 100;
    int m_cost = 100;
    static int buildingCount;
    int buildingID = 0;
};