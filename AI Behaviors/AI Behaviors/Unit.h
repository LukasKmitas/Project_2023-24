#pragma once
#include <iostream>
#include "SFML/Graphics.hpp"
#include "Global.h"
#include <cmath> 
#include "Bullet.h"
#include "ParticleSystem.h"

enum class UnitType
{
    Infantry,
    Vehicle,
    Air
};

class Unit
{
public:

    Unit();
    virtual ~Unit();

    virtual void update(sf::Time t_deltaTime, std::vector<Unit*>& allUnits);
    virtual void render(sf::RenderWindow& m_window);

    virtual void attack(Unit* target);

    void setPosition(const sf::Vector2f& m_position);
    void moveTo(const sf::Vector2f& targetPos);
    void setSelected(bool selected);
    void setTargetPosition(const sf::Vector2f& targetPos);
    void takeDamage(float damageAmount);

    const sf::Sprite& getSprite() const;

    sf::Vector2f getPosition() const;
    sf::Vector2f normalize(const sf::Vector2f source);
    sf::Vector2f steerTowards(sf::Vector2f target);

    std::vector<Bullet> bullets;

    float angleFromVector(const sf::Vector2f& vector);
    float getViewRadius() const;
    float distance(const sf::Vector2f& a, const sf::Vector2f& b);
    float magnitude(const sf::Vector2f& v);
    float getHealth() const;
    float toDegrees(float radians);

    bool checkAffordability();
    bool isActive() const;

    int unitIndex = 0;

    bool isSelected = false;
    bool m_active = true;

protected:

    UnitType m_unitType;
    ParticleSystem m_particleSystem;

    void initView();
    void initShader();

    void avoidCollisions(std::vector<Unit*>& allUnits);
    void orientSpriteToMovement(sf::Time t_deltaTime);

    sf::Texture m_unitTexture;
    sf::Sprite m_unitSprite;

    sf::Texture m_weaponTexture;
    sf::Sprite m_weaponSprite;

    sf::CircleShape m_viewCircleShape;
    sf::Shader glowShader;

    int m_cost = 100;
    float m_health = 100.0f;
    float m_viewRadius = 100.0f;

    float m_speed = 100.0f;
    float m_stoppingDistance = 20;
    float m_slowingRadius = 100;
    float m_maxForce = 1.0f;
    float m_rotationSpeed = 250.0f;
    float m_bulletSpeed = 100;
    float rotationSpeedDegreesPerSecond = 90.0f;

    const float PI = 3.14159265358979323846f;

    bool isOrbiting = false;

    sf::Vector2f m_position;
    sf::Vector2f m_targetPosition;
    sf::Vector2f m_velocity;
    sf::Vector2f directionToEnemy;
    sf::Vector2f m_acceleration = sf::Vector2f(0.0f, 0.0f);
};

