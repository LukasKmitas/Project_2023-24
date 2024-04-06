#include "ParticleSystem.h"

void ParticleSystem::update(sf::Time t_deltaTime)
{
    float dt = t_deltaTime.asSeconds();

    for (auto& particle : m_particles)
    {
        particle.m_position += particle.m_velocity * dt;
        particle.m_lifetime -= dt;
    }

    m_particles.erase(std::remove_if(m_particles.begin(), m_particles.end(),
        [](const Particle& p) { return p.m_lifetime <= 0; }), m_particles.end());
}

/// <summary>
/// Renders the particles into existence
/// </summary>
/// <param name="m_window"></param>
void ParticleSystem::render(sf::RenderWindow& m_window)
{
    for (const auto& particle : m_particles) 
    {
        if (particle.m_texture)
        {
            sf::RectangleShape shape(sf::Vector2f(particle.m_size * 2, particle.m_size * 2));
            shape.setTexture(particle.m_texture);
            shape.setFillColor(particle.m_color);
            shape.setOrigin(particle.m_size, particle.m_size);
            shape.setPosition(particle.m_position);
            m_window.draw(shape);
        }
        else
        {
            sf::CircleShape shape(particle.m_size);
            shape.setFillColor(particle.m_color);
            shape.setPosition(particle.m_position);
            m_window.draw(shape);
        }
    }
}

/// <summary>
/// Adds the particle into the vector
/// </summary>
/// <param name="m_particle"></param>
void ParticleSystem::addParticle(const Particle& m_particle)
{
    m_particles.push_back(m_particle);
}
