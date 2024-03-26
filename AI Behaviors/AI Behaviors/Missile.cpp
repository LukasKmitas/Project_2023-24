#include "Missile.h"

Missile::Missile(const sf::Vector2f& startPosition, const sf::Vector2f& targetDirection, float initialSpeed, const sf::Texture& texture)
    : position(startPosition), speed(initialSpeed) 
{
    velocity = normalize(targetDirection) * speed;
    missileSprite.setScale(0.2, 0.2);
    missileSprite.setTexture(texture);
    missileSprite.setOrigin(texture.getSize().x / 2.0f, texture.getSize().y / 2.0f); // Center the origin
    missileSprite.setPosition(position);
}

void Missile::update(sf::Time deltaTime)
{
    if (active)
    {
        position += velocity * deltaTime.asSeconds();
        missileSprite.setPosition(position);
        float angle = atan2(velocity.y, velocity.x) * (180 / 3.14159265f);
        missileSprite.setRotation(angle + 90);

        lifetime -= deltaTime.asSeconds();
        if (lifetime <= 0.0f)
        {
            active = false; 
        }
    }

    updateTrail(deltaTime);

}

void Missile::render(sf::RenderWindow& window) const
{
    for (const auto& part : trail) 
    {
        window.draw(part.shape);
    }
    if (active)
    {
        window.draw(missileSprite);
    }
}

sf::Vector2f Missile::normalize(const sf::Vector2f& source) const
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

void Missile::updateTrail(sf::Time deltaTime)
{
    timeSinceLastTrailPart += deltaTime.asSeconds();

    if (timeSinceLastTrailPart >= addTrailPartInterval && active)
    {
        TrailPart newPart;
        newPart.shape.setRadius(3.0f);
        newPart.shape.setPosition(position);
        newPart.lifetime = trailMaxLifetime;
        newPart.expansionRate = 3.0f;
        newPart.startColor = sf::Color(255, 140, 0);
        newPart.endColor = sf::Color(50, 50, 50);
        newPart.shape.setFillColor(newPart.startColor);
        trail.push_front(newPart);

        timeSinceLastTrailPart = 0.0f;
    }

    for (auto& part : trail)
    {
        part.lifetime -= deltaTime.asSeconds();
        float lifeRatio = part.lifetime / trailMaxLifetime;
        sf::Color currentColor = lerpColor(part.startColor, part.endColor, 1.0f - lifeRatio);

        if (lifeRatio < 0.2f) // Last 20% of lifetime
        {
            currentColor.a = static_cast<sf::Uint8>(255 * (lifeRatio / 0.2f));
        }
        part.shape.setFillColor(currentColor);
        part.shape.setRadius(part.shape.getRadius() + (part.expansionRate * deltaTime.asSeconds()));
    }

    trail.erase(std::remove_if(trail.begin(), trail.end(),
        [](const TrailPart& part) { return part.lifetime <= 0; }), trail.end());
}

sf::Color Missile::lerpColor(const sf::Color& start, const sf::Color& end, float t) 
{
    return sf::Color(
        static_cast<sf::Uint8>(start.r + (end.r - start.r) * t),
        static_cast<sf::Uint8>(start.g + (end.g - start.g) * t),
        static_cast<sf::Uint8>(start.b + (end.b - start.b) * t),
        static_cast<sf::Uint8>(start.a + (end.a - start.a) * t)
    );
}