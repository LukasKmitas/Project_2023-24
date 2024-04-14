#pragma once
#include <iostream>
#include "SFML/Graphics.hpp"
#include "Global.h"
#include <cmath> 
#include "Bullet.h"
#include "Missile.h"
#include "ParticleSystem.h"
#include "Building.h"
#include "Tile.h"
#include "SoundManager.h"

enum class UnitTypeClass
{
    Infantry,
    Vehicle,
    Air
};

enum class UnitType
{
    Rifleman, 
    Harvester, Buggy, TankAurora, 
    Hammerhead, Firehawk
};

class Unit
{
public:

    Unit();
    virtual ~Unit();

    virtual void update(sf::Time t_deltaTime, std::vector<Unit*>& allyUnits);
    virtual void render(sf::RenderWindow& m_window);

    virtual UnitType getUnitType() const = 0;

    void setPosition(const sf::Vector2f& m_position);
    void setHealth(float m_setHealth);
    void moveTo(const sf::Vector2f& m_targetPos);
    void setSelected(bool m_selected);
    void setTargetPosition(const sf::Vector2f& m_targetPos);
    void takeDamage(float m_damageAmount);
    void addHealth(float m_healthAmount);
    void applySlowEffect(float m_speedFactor, float m_duration, float m_postSlowWait);

    void setEnemyUnits(std::vector<Unit*>& m_enemyUnits);
    void setEnemyBuildings(std::vector<Building*>& m_enemyBuildings);
    void setTiles(const std::vector<std::vector<Tile>>& m_tiles);

    const sf::Sprite& getSprite() const;

    sf::Vector2f getPosition() const;
    sf::Vector2f getTargetPosition() const;
    sf::Vector2f normalize(const sf::Vector2f m_source);
    sf::Vector2f steerTowards(sf::Vector2f m_target);
    sf::Vector2f rotateVector(sf::Vector2f m_vector, float m_angleDegrees);
    sf::Vector2f lerp(const sf::Vector2f& m_start, const sf::Vector2f& m_end, float m_time);
    sf::Vector2f findAvoidanceDirection(const sf::Vector2f& m_currentPosition, float m_checkAheadDistance);

    std::vector<Bullet> m_bullets;
    std::vector<Missile> m_missiles;

    float angleFromVector(const sf::Vector2f& m_vector);
    float getViewRadius() const;
    float distance(const sf::Vector2f& a, const sf::Vector2f& b);
    float magnitude(const sf::Vector2f& v) const;
    float getHealth() const;
    float toDegrees(float radians);
    float angleBetweenVectors(sf::Vector2f vec1, sf::Vector2f vec2);

    float getDamage() const;

    bool checkAffordability();
    bool isActive() const;

    int m_unitIndex = 0;

    bool isSelected = false;
    bool m_active = true;
    bool m_isEnemy = false;

protected:

    UnitTypeClass m_unitTypeClass;
    ParticleSystem m_particleSystem;

    std::vector<Unit*>* m_enemyUnits = nullptr;
    std::vector<Building*>* m_enemyBuildings = nullptr;
    Unit* m_closestEnemy = nullptr;
    Building* m_closestBuilding = nullptr;
    const std::vector<std::vector<Tile>>* m_tiles;

    std::vector<sf::Vector2f> m_debugRays;

    void initView();
    void initHealthBar();
    void initShader();

    void avoidCollisionsWithUnits(std::vector<Unit*>& m_allyUnits);
    void avoidCollisionsWithWalls();
    void orientSpriteToMovement(sf::Time t_deltaTime);

    virtual void squadEntityRemoval();
    virtual void squadEntityRegain();

    sf::Texture m_unitTexture;
    sf::Sprite m_unitSprite;

    sf::RectangleShape m_healthBarBackground;
    sf::RectangleShape m_healthBarForeground;

    sf::Texture m_weaponTexture;
    sf::Sprite m_weaponSprite;

    sf::CircleShape m_viewCircleShape;
    sf::Shader m_glowShader;

    int m_cost = 100;
    float m_health = 100.0f;
    const float m_maxHealth = 100.0f;
    float m_viewRadius = 100.0f;
    float m_damage = 4.0f;

    float m_speed = 100.0f;
    float m_stoppingDistance = 20;
    float m_slowingRadius = 100;
    float m_maxForce = 1.0f;
    float m_rotationSpeed = 250.0f;
    float m_bulletSpeed = 100;
    float m_closestDistance;
    float m_closestBuildingDistance;
    float m_arrivalTolerance = 5.0f;

    const float PI = 3.14159265358979323846f;

    bool isOrbiting = false;
    bool isReloading = false;

    sf::Vector2f m_position;
    sf::Vector2f m_targetPosition;
    sf::Vector2f m_velocity;
    sf::Vector2f m_directionToEnemy;
    sf::Vector2f m_acceleration = sf::Vector2f(0.0f, 0.0f);

    sf::Clock m_slowEffectClock;
    bool m_isSlowed = false;
    bool m_isGraduallySlowed = false;
    bool m_inPostSlowWait = false;
    float m_slowDownStartTime = 0.0f;
    float m_minimumSpeedFactor = 0.1f;
    float m_slowEffectDuration = 0.0f;
    float m_originalSpeed = 100.0f; 
    float m_postSlowWaitDuration = 3.0f;

};

