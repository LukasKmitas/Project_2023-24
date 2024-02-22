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
        if (particle.texture)
        {
            sf::RectangleShape shape(sf::Vector2f(particle.size * 2, particle.size * 2));
            shape.setTexture(particle.texture);
            shape.setFillColor(particle.color);
            shape.setOrigin(particle.size, particle.size);
            shape.setPosition(particle.position);
            window.draw(shape);
        }
        else
        {
            sf::CircleShape shape(particle.size);
            shape.setFillColor(particle.color);
            shape.setPosition(particle.position);
            window.draw(shape);
        }
    }
}

void ParticleSystem::addParticle(const Particle& particle)
{
    particles.push_back(particle);
}
