#pragma once
#include <iostream>
#include "SFML/Graphics.hpp"

class Unit
{
public:

    Unit();
    virtual ~Unit();

    virtual void update(sf::Time t_deltaTime, std::vector<Unit*>& allUnits);
    virtual void render(sf::RenderWindow& m_window) const;

    virtual void attack(Unit* target);

    void setPosition(const sf::Vector2f& m_position);
    void moveTo(const sf::Vector2f& targetPos);
    void setSelected(bool selected);

    sf::Vector2f separation(const std::vector<Unit*>& units);

    const sf::Sprite& getSprite() const 
    {
        return m_unitSprite;
    }

    sf::Vector2f getPosition() const;
    float distance(const sf::Vector2f& a, const sf::Vector2f& b);
    void normalize(sf::Vector2f& vec);
    float magnitude(const sf::Vector2f& v);

    bool isSelected = false;

protected:

    sf::Texture m_unitTexture;
    sf::Sprite m_unitSprite;

    int m_cost;
    int m_speed;
    float const maxSpeed = 100;
    float m_health;
    float desiredSeparation = 50.0f;
    float separationWeight = 1.0f;
    float stoppingDistance = 20.0f;
    float offsetRange = 100.0f;

    sf::Vector2f m_position;
    sf::Vector2f m_targetPosition;
    sf::Vector2f m_velocity;

};

