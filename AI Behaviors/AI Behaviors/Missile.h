#pragma once
#include "SFML/Graphics.hpp"
#include <deque>

struct TrailPart
{
    sf::CircleShape m_shape;
    float m_lifetime = 0;
    float m_expansionRate = 0;
    sf::Color m_startColor;
    sf::Color m_endColor;
};

class Missile
{
public:

    Missile(const sf::Vector2f& m_startPosition, const sf::Vector2f& m_targetDirection, float m_speed, const sf::Texture& m_texture);

    void update(sf::Time t_deltaTime);
    void render(sf::RenderWindow& m_window) const;

    sf::Sprite m_missileSprite;
    std::deque<TrailPart> m_trail;

    sf::Vector2f m_position;

    float m_damage = 20.0f;

    bool m_active = true;

private:

    sf::Vector2f normalize(const sf::Vector2f& m_source) const;
    void updateTrail(sf::Time t_deltaTime);

    sf::Color lerpColor(const sf::Color& m_start, const sf::Color& m_end, float m_lifeTime);

    sf::Vector2f m_velocity;
    float m_speed;
    float m_lifetime = 3.0f;

    float m_trailMaxLifetime = 2.0f;
    float m_timeSinceLastTrailPart = 0.0f;
    float m_addTrailPartInterval = 0.05f;
};
