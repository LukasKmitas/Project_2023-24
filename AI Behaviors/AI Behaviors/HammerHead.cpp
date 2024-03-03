#include "HammerHead.h"

HammerHead::HammerHead()
{
	setupHammerhead();
	m_cost = 1500;
	m_health = 100;
    m_viewRadius = 310;
    m_speed = 150;

	m_slowingRadius = 100.0f;
    m_rotationSpeed = 80;
    m_maxForce = 200;
    rotationSpeedDegreesPerSecond = 90;
}

HammerHead::~HammerHead()
{
}

void HammerHead::update(sf::Time t_deltaTime, std::vector<Unit*>& allUnits)
{
    AircraftUnit::update(t_deltaTime, allUnits);

    float arrivalTolerance = 5.0f;
    float distance = magnitude(m_targetPosition - m_position);
    if (distance < arrivalTolerance)
    {
        m_velocity = sf::Vector2f(0, 0);
        m_acceleration = sf::Vector2f(0, 0);
        return;
    }

    sf::Vector2f desiredDirection = normalize(m_targetPosition - m_position);

    float speed = m_speed;
    if (distance < m_slowingRadius)
    {
        speed *= (distance / m_slowingRadius);
    }

    sf::Vector2f desiredVelocity = desiredDirection * speed;
    sf::Vector2f steeringForce = desiredVelocity - m_velocity;
    steeringForce = normalize(steeringForce) * m_maxForce;

    m_acceleration = steeringForce;
    m_velocity += m_acceleration * t_deltaTime.asSeconds();

    if (magnitude(m_velocity) > m_speed)
    {
        m_velocity = normalize(m_velocity) * m_speed;
    }

    m_position += m_velocity * t_deltaTime.asSeconds();
    setPosition(m_position);

    orientSpriteToMovement(t_deltaTime);
    setupParticleExhaustEffect();
}

void HammerHead::setupHammerhead()
{
	if (!m_unitTexture.loadFromFile("Assets\\Images\\Units\\HammerHead.png"))
	{
		std::cout << "Error - Loading HammerHead Texture" << std::endl;
	}
	m_unitSprite.setTexture(m_unitTexture);
	m_unitSprite.setPosition(m_position);
	m_unitSprite.setOrigin(m_unitSprite.getLocalBounds().width / 2, m_unitSprite.getLocalBounds().height / 2);
	m_unitSprite.setScale(0.13, 0.13);
}

void HammerHead::setupParticleExhaustEffect()
{
    const int particlesPerUpdate = 3;
    float particleOffset = 20.0f;
    float rotationRadians = (m_unitSprite.getRotation() - 90) * (PI / 180.0f);
    sf::Vector2f backwardDirection(std::cos(rotationRadians), std::sin(rotationRadians));
    sf::Vector2f offsetPosition = m_position - backwardDirection * particleOffset;

    for (int i = 0; i < particlesPerUpdate; ++i)
    {
        float angle = static_cast<float>(rand() % 360) * (PI / 180.0f);
        float speed = 10.0f;
        sf::Vector2f velocity(speed * std::cos(angle), speed * std::sin(angle));

        sf::Color startColor = sf::Color(255, rand() % 256, 0, 255);
        float lifetime = 0.2f + static_cast<float>(rand() % 30) / 100.0f;
        float size = 1.5f;

        m_particleSystem.addParticle(Particle(offsetPosition, velocity, startColor, lifetime, size));
    }
}
