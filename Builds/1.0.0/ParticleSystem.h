#pragma once
#include "SFML/Graphics.hpp"

struct Particle 
{
    sf::Vector2f m_position;
    sf::Vector2f m_velocity;
    sf::Color m_color;
    float m_lifetime;
    float m_size;
    const sf::Texture* m_texture;

    Particle(const sf::Vector2f& position, const sf::Vector2f& velocity, const sf::Color& color, float lifetime, float size, const sf::Texture* texture = nullptr)
        : m_position(position), m_velocity(velocity), m_color(color), m_lifetime(lifetime), m_size(size), m_texture(texture) {}
};

class ParticleSystem
{
public:
    std::vector<Particle> m_particles;

    void update(sf::Time t_deltaTime);
    void render(sf::RenderWindow& m_window);

    void addParticle(const Particle& m_particle);

};

