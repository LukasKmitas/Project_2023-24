#pragma once
#include "SFML/Graphics.hpp"
#include <deque>

class Bullet
{
public:

    Bullet(const sf::Vector2f& startPosition, const sf::Vector2f& targetDirection, float speed);

    void update(sf::Time deltaTime);
    void render(sf::RenderWindow& window, const sf::Shader& glowShader) const;

    sf::CircleShape bulletShape;

    sf::Vector2f position;

    float damage = 4.0f;

    bool active = true;

private:

    struct TrailPart
    {
        sf::CircleShape trialShape;
        float lifetime;
    };

    sf::Vector2f normalize(const sf::Vector2f& source);

    std::deque<TrailPart> trail;

    sf::Vector2f velocity;

    float lifetime = 1.0f;
    float trailMaxLifetime = 0.3f;
    float timeSinceLastTrailPart = 0.0f;
    float addTrailPartInterval = 0.03f;

};
