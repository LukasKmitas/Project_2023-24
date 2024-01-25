#pragma once
#include <iostream>
#include "SFML/Graphics.hpp"

class Unit
{
public:

    Unit();
    virtual ~Unit();

    virtual void update(sf::Time t_deltaTime);
    virtual void render(sf::RenderWindow& m_window) const;

    virtual void move(const sf::Vector2f& direction);
    virtual void attack(Unit* target);

    void setPosition(const sf::Vector2f& m_position);
    void moveTo(const sf::Vector2f& targetPos);
    void setSelected(bool selected);

    const sf::Sprite& getSprite() const 
    {
        return m_unitSprite;
    }

    bool isSelected = false;

protected:

    sf::Texture m_unitTexture;
    sf::Sprite m_unitSprite;

    int m_cost;
    int m_speed;
    float m_health;

    sf::Vector2f m_position;
    sf::Vector2f m_targetPosition;

};

