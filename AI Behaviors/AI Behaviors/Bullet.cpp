#include "Bullet.h"

Bullet::Bullet(const sf::Vector2f& startPosition, const sf::Vector2f& targetDirection, float speed)
{
    position = startPosition;
    velocity = normalize(targetDirection) * speed;
    shape.setRadius(2.0f);
    shape.setPosition(position);
    shape.setFillColor(sf::Color::Red); 
}

void Bullet::update(sf::Time deltaTime)
{
    position += velocity * deltaTime.asSeconds();
    shape.setPosition(position);
}

void Bullet::render(sf::RenderWindow& window) const
{
    window.draw(shape);
}

sf::Vector2f Bullet::normalize(const sf::Vector2f& source)
{
    float length = std::sqrt(source.x * source.x + source.y * source.y);
    if (length != 0)
    {
        return source / length;
    }
    else
    {
        return source;
    }
}
