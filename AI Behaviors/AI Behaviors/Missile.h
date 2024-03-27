#pragma once
#include "SFML/Graphics.hpp"
#include <deque>

struct TrailPart
{
    sf::CircleShape shape;
    float lifetime;
    float expansionRate;
    sf::Color startColor;
    sf::Color endColor;
};

class Missile
{
public:

    Missile(const sf::Vector2f& startPosition, const sf::Vector2f& targetDirection, float speed, const sf::Texture& texture);

    void update(sf::Time deltaTime);
    void render(sf::RenderWindow& window) const;

    sf::Sprite missileSprite;
    std::deque<TrailPart> trail;

    sf::Vector2f position;

    float damage = 20.0f;

    bool active = true;

private:

    sf::Vector2f normalize(const sf::Vector2f& source) const;
    void updateTrail(sf::Time deltaTime);

    sf::Color lerpColor(const sf::Color& start, const sf::Color& end, float t);

    sf::Vector2f velocity;
    float speed;
    float lifetime = 3.0f;

    float trailMaxLifetime = 2.0f;
    float timeSinceLastTrailPart = 0.0f;
    float addTrailPartInterval = 0.05f;
};
