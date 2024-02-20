#pragma once
#include "SFML/Graphics.hpp"

class Bullet
{
public:

    Bullet(const sf::Vector2f& startPosition, const sf::Vector2f& targetDirection, float speed);

    void update(sf::Time deltaTime);
    void render(sf::RenderWindow& window) const;

    sf::Vector2f normalize(const sf::Vector2f& source);

    sf::Vector2f position;
    sf::Vector2f velocity;

    sf::CircleShape shape;

    float damage = 10.0f;

    bool active = true;
};
