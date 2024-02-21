#pragma once
#include "SFML/Graphics.hpp"

struct Particle 
{
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Color color;
    float lifetime;

    Particle(const sf::Vector2f& position, const sf::Vector2f& velocity, const sf::Color& color, float lifetime)
        : position(position), velocity(velocity), color(color), lifetime(lifetime) {}
};

class ParticleSystem
{
public:
    std::vector<Particle> particles;

    void update(sf::Time dt);
    void render(sf::RenderWindow& window);

    void addParticle(const Particle& particle);

};

