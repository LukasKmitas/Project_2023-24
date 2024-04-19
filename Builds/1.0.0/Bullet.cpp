#include "Bullet.h"

Bullet::Bullet(const sf::Vector2f& m_startPosition, const sf::Vector2f& m_targetDirection, float m_speed)
{
    m_position = m_startPosition;
    m_velocity = normalize(m_targetDirection) * m_speed;
    m_bulletShape.setRadius(2.0f);
    m_bulletShape.setOrigin(2, 2);
    m_bulletShape.setPosition(m_position);
    m_bulletShape.setFillColor(sf::Color::Red); 
}

void Bullet::update(sf::Time t_deltaTime)
{
    m_position += m_velocity * t_deltaTime.asSeconds();
    m_bulletShape.setPosition(m_position);

    for (auto& part : m_trail) 
    {
        part.m_lifetime -= t_deltaTime.asSeconds();
        part.m_trialShape.setFillColor(sf::Color(255, 0, 0, static_cast<sf::Uint8>(255 * (part.m_lifetime / m_trailMaxLifetime))));
    }
    m_trail.erase(std::remove_if(m_trail.begin(), m_trail.end(), [](const TrailPart& part) { return part.m_lifetime <= 0; }), m_trail.end());

    m_timeSinceLastTrailPart += t_deltaTime.asSeconds();
    if (m_timeSinceLastTrailPart >= m_addTrailPartInterval) 
    {
        TrailPart newPart;
        newPart.m_trialShape = m_bulletShape;
        newPart.m_trialShape.setRadius(m_bulletShape.getRadius() * 0.6f);
        newPart.m_lifetime = m_trailMaxLifetime;
        m_trail.push_front(newPart);

        m_timeSinceLastTrailPart = 0.0f;
    }

    m_lifetime -= t_deltaTime.asSeconds();
    if (m_lifetime <= 0.0f)
    {
        m_active = false;
    }
}

void Bullet::render(sf::RenderWindow& m_window, const sf::Shader& m_glowShader) const
{
    for (const auto& part : m_trail)
    {
        m_window.draw(part.m_trialShape);
    }
    m_window.draw(m_bulletShape, &m_glowShader);
}

sf::Vector2f Bullet::normalize(const sf::Vector2f& m_source)
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
