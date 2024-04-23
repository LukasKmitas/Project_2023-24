#pragma once
#include <iostream>
#include "SFML/Graphics.hpp"
#include "Global.h"
#include "BuildingType.h"
#include "ParticleSystem.h"

class Building
{
public:

    Building(BuildingType m_type);
    virtual ~Building();

    virtual void update(sf::Time t_deltaTime);
    virtual void render(sf::RenderWindow& m_window);

    void takeDamage(float m_damageAmount);
    void setPosition(const sf::Vector2f& m_position);

    sf::Vector2f getPosition();
 
    int getCost() const;
    //int getBuildingCount() const;
    float getHealth() const;
    BuildingType getType() const;

    bool checkAffordability();

    void setPlacementRadiusSize(float m_radius);
    void setHealth(float newHealth);

    const sf::Sprite& getBuildingSprite() const;
    const sf::Texture& getBuildingTexture() const;
    const sf::CircleShape& getPlacementRadius() const;

    void updateHealthBar();

protected:

    void initSmokeEffect();
    void spawnSmokeEffect();

    ParticleSystem m_smokeEffect;
    sf::Texture m_smokeTexture;

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
    //static int m_buildingCount;
    //int m_numBuildingCount = 0;

    BuildingType m_type;

    bool m_placementRadiusVisible = false;
};