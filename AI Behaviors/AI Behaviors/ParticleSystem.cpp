#include "ParticleSystem.h"

void ParticleSystem::update(sf::Time t_deltaTime)
{
    float dt = t_deltaTime.asSeconds();

    for (auto& particle : particles)
    {
        particle.position += particle.velocity * dt;
        particle.lifetime -= dt;
    }

    particles.erase(std::remove_if(particles.begin(), particles.end(),
        [](const Particle& p) { return p.lifetime <= 0; }), particles.end());
}

void ParticleSystem::render(sf::RenderWindow& window)
{
    for (const auto& particle : particles)
    {
        sf::CircleShape shape(2);
        shape.setPosition(particle.position);
        shape.setFillColor(particle.color);
        window.draw(shape);
    }
}

void ParticleSystem::addParticle(const Particle& particle)
{
    particles.push_back(particle);
}
