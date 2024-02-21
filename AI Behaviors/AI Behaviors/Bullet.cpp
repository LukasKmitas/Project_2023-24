#include "Bullet.h"

Bullet::Bullet(const sf::Vector2f& startPosition, const sf::Vector2f& targetDirection, float speed)
{
    position = startPosition;
    velocity = normalize(targetDirection) * speed;
    bulletShape.setRadius(2.0f);
    bulletShape.setOrigin(2, 2);
    bulletShape.setPosition(position);
    bulletShape.setFillColor(sf::Color::Red); 
}

void Bullet::update(sf::Time deltaTime)
{
    position += velocity * deltaTime.asSeconds();
    bulletShape.setPosition(position);

    for (auto& part : trail) 
    {
        part.lifetime -= deltaTime.asSeconds();
        part.trialShape.setFillColor(sf::Color(255, 0, 0, static_cast<sf::Uint8>(255 * (part.lifetime / trailMaxLifetime))));
    }
    trail.erase(std::remove_if(trail.begin(), trail.end(), [](const TrailPart& part) { return part.lifetime <= 0; }), trail.end());

    timeSinceLastTrailPart += deltaTime.asSeconds();
    if (timeSinceLastTrailPart >= addTrailPartInterval) 
    {
        TrailPart newPart;
        newPart.trialShape = bulletShape;
        newPart.trialShape.setRadius(bulletShape.getRadius() * 0.6f);
        newPart.lifetime = trailMaxLifetime;
        trail.push_front(newPart);

        timeSinceLastTrailPart = 0.0f;
    }

    lifetime -= deltaTime.asSeconds();
    if (lifetime <= 0.0f)
    {
        active = false;
    }
}

void Bullet::render(sf::RenderWindow& window, const sf::Shader& glowShader) const
{
    for (const auto& part : trail)
    {
        window.draw(part.trialShape);
    }
    window.draw(bulletShape, &glowShader);
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
