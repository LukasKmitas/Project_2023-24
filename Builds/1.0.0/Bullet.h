#pragma once
#include "SFML/Graphics.hpp"
#include <deque>

class Bullet
{
public:

    Bullet(const sf::Vector2f& m_startPosition, const sf::Vector2f& m_targetDirection, float m_speed);

    void update(sf::Time t_deltaTime);
    void render(sf::RenderWindow& m_window, const sf::Shader& m_glowShader) const;

    sf::CircleShape m_bulletShape;

    sf::Vector2f m_position;

    bool m_active = true;

private:

    struct TrailPart
    {
        sf::CircleShape m_trialShape;
        float m_lifetime = 0;
    };

    sf::Vector2f normalize(const sf::Vector2f& m_source);

    std::deque<TrailPart> m_trail;

    sf::Vector2f m_velocity;

    float m_lifetime = 1.0f;
    float m_trailMaxLifetime = 0.3f;
    float m_timeSinceLastTrailPart = 0.0f;
    float m_addTrailPartInterval = 0.03f;

};
