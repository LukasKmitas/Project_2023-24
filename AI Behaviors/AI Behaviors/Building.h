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

    void takeDamage(float m_damageAmount);
    void setPosition(const sf::Vector2f& m_position);

    sf::Vector2f getPosition();
 
    int getCost() const;
    int getBuildingID() const;
    float getHealth() const;

    bool checkAffordability();

    void setPlacementRadiusSize(float m_radius);

    const sf::Sprite& getBuildingSprite() const;
    const sf::Texture& getBuildingTexture() const;
    const sf::CircleShape& getPlacementRadius() const;

protected:

    sf::Vector2f m_position;
    std::string m_name;
    
    sf::Texture m_buildingTexture;
    sf::Sprite m_buildingSprite;

    sf::CircleShape m_placementRadius;

    sf::RectangleShape m_healthBar;
    sf::RectangleShape m_healthBarBackground;

    int m_health = 100;
    int m_maxHealth = 100;
    int m_cost = 100;
    static int m_buildingCount;
    int m_buildingID = 0;

    bool m_placementRadiusVisible = false;
};