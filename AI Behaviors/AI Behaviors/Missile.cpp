#include "Missile.h"

Missile::Missile(const sf::Vector2f& m_startPosition, const sf::Vector2f& m_targetDirection, float m_speed, const sf::Texture& m_texture)
    : m_position(m_startPosition), m_speed(m_speed)
{
    m_velocity = normalize(m_targetDirection) * m_speed;
    m_missileSprite.setScale(0.2, 0.2);
    m_missileSprite.setTexture(m_texture);
    m_missileSprite.setOrigin(m_texture.getSize().x / 2.0f, m_texture.getSize().y / 2.0f); // Center the origin
    m_missileSprite.setPosition(m_position);
}

void Missile::update(sf::Time t_deltaTime)
{
    if (m_active)
    {
        m_position += m_velocity * t_deltaTime.asSeconds();
        m_missileSprite.setPosition(m_position);
        float angle = atan2(m_velocity.y, m_velocity.x) * (180 / 3.14159265f);
        m_missileSprite.setRotation(angle + 90);

        m_lifetime -= t_deltaTime.asSeconds();
        if (m_lifetime <= 0.0f)
        {
            m_active = false; 
        }
    }

    updateTrail(t_deltaTime);
}

void Missile::render(sf::RenderWindow& m_window) const
{
    for (const auto& part : m_trail) 
    {
        m_window.draw(part.m_shape);
    }
    if (m_active)
    {
        m_window.draw(m_missileSprite);
    }
}

sf::Vector2f Missile::normalize(const sf::Vector2f& m_source) const
{
    float length = std::sqrt(m_source.x * m_source.x + m_source.y * m_source.y);
    if (length != 0)
    {
        return m_source / length;
    }
    else
    {
        return m_source;
    }
}

void Missile::updateTrail(sf::Time t_deltaTime)
{
    m_timeSinceLastTrailPart += t_deltaTime.asSeconds();

    if (m_timeSinceLastTrailPart >= m_addTrailPartInterval && m_active)
    {
        TrailPart newPart;
        newPart.m_shape.setRadius(3.0f);
        newPart.m_shape.setPosition(m_position);
        newPart.m_lifetime = m_trailMaxLifetime;
        newPart.m_expansionRate = 3.0f;
        newPart.m_startColor = sf::Color(255, 140, 0);
        newPart.m_endColor = sf::Color(50, 50, 50);
        newPart.m_shape.setFillColor(newPart.m_startColor);
        m_trail.push_front(newPart);

        m_timeSinceLastTrailPart = 0.0f;
    }

    for (auto& part : m_trail)
    {
        part.m_lifetime -= t_deltaTime.asSeconds();
        float lifeRatio = part.m_lifetime / m_trailMaxLifetime;
        sf::Color currentColor = lerpColor(part.m_startColor, part.m_endColor, 1.0f - lifeRatio);

        if (lifeRatio < 0.2f) // Last 20% of lifetime
        {
            currentColor.a = static_cast<sf::Uint8>(255 * (lifeRatio / 0.2f));
        }
        part.m_shape.setFillColor(currentColor);
        part.m_shape.setRadius(part.m_shape.getRadius() + (part.m_expansionRate * t_deltaTime.asSeconds()));
    }

    m_trail.erase(std::remove_if(m_trail.begin(), m_trail.end(),
        [](const TrailPart& part) { return part.m_lifetime <= 0; }), m_trail.end());
}

sf::Color Missile::lerpColor(const sf::Color& m_start, const sf::Color& m_end, float m_lifeTime)
{
    return sf::Color
    (
        static_cast<sf::Uint8>(m_start.r + (m_end.r - m_start.r) * m_lifeTime),
        static_cast<sf::Uint8>(m_start.g + (m_end.g - m_start.g) * m_lifeTime),
        static_cast<sf::Uint8>(m_start.b + (m_end.b - m_start.b) * m_lifeTime),
        static_cast<sf::Uint8>(m_start.a + (m_end.a - m_start.a) * m_lifeTime)
    );
}