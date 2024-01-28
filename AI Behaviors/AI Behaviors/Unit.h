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

    const sf::Sprite& getSprite() const 
    {
        return m_unitSprite;
    }

    sf::Vector2f getPosition() const;
    float angleFromVector(const sf::Vector2f& vector);
    float getViewRadius() const;
    float distance(const sf::Vector2f& a, const sf::Vector2f& b);
    void normalize(sf::Vector2f& vec);
    float magnitude(const sf::Vector2f& v);

    int unitIndex = 0;

    bool isSelected = false;

protected:

    void initView();

    sf::Texture m_unitTexture;
    sf::Sprite m_unitSprite;

    sf::CircleShape m_viewCircleShape;

    int m_cost = 100;
    float m_speed = 100.0f;
    float m_health = 100.0f;
    float stoppingDistance = 20;
    float slowingRadius = 100;
    float viewRadius = 200.0f;

    const float PI = 3.14159265358979323846f;

    sf::Vector2f m_position;
    sf::Vector2f m_targetPosition;
    sf::Vector2f m_velocity;

};

