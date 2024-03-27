#include "TankAurora.h"

TankAurora::TankAurora()
{
	setupTankAurora();
    setupHealingAura();
	m_cost = 100;
	m_health = 200;
	m_viewRadius = 350;
	m_speed = 80;
    m_maxForce = 100;
}

TankAurora::~TankAurora()
{
}

void TankAurora::update(sf::Time t_deltaTime, std::vector<Unit*>& allUnits)
{
	VehicleUnit::update(t_deltaTime, allUnits);

    movement(t_deltaTime);

    m_healingAura.setPosition(getPosition());
    
    float currentTime = m_shaderClock.getElapsedTime().asSeconds();
    m_healingAuraShader.setUniform("time", currentTime);
    sf::Glsl::Vec4 outlineColor(0, 1, 0, 0.3);
    m_healingAuraShader.setUniform("outlineColor", outlineColor);

    for (Unit* unit : allUnits)
    {
        if (unit != this)
        {
            float distanceToAlly = magnitude(unit->getPosition() - getPosition());

            if (distanceToAlly <= m_healingRange)
            {
                unit->addHealth(10.0f * t_deltaTime.asSeconds());
            }
        }
    }

}

void TankAurora::render(sf::RenderWindow& m_window)
{
    m_window.draw(m_healingAura, &m_healingAuraShader);

    VehicleUnit::render(m_window);
}

void TankAurora::setupTankAurora()
{
	if (!m_unitTexture.loadFromFile("Assets\\Images\\Units\\TankAurora.png"))
	{
		std::cout << "Error - Loading Tank Aurora Texture" << std::endl;
	}
	m_unitSprite.setTexture(m_unitTexture);
	m_unitSprite.setPosition(m_position);
	m_unitSprite.setRotation(180);
	m_unitSprite.setOrigin(m_unitSprite.getLocalBounds().width / 2, m_unitSprite.getLocalBounds().height / 2);
	m_unitSprite.setScale(0.2, 0.2);
}

void TankAurora::setupHealingAura() 
{
    m_healingAura.setRadius(m_healingRange);
    m_healingAura.setOrigin(m_healingRange, m_healingRange);
    m_healingAura.setFillColor(sf::Color::Transparent);
    m_healingAura.setOutlineColor(sf::Color(0, 255, 0, 128));
    m_healingAura.setOutlineThickness(1.0f);

    if (!m_healingAuraShader.loadFromFile("Assets\\Shaders\\healingAura.frag", sf::Shader::Fragment)) 
    {
        std::cout << "Error loading shader" << std::endl;
    }
}

void TankAurora::movement(sf::Time t_deltaTime)
{
    float distance = magnitude(m_targetPosition - m_position);

    if (distance < m_arrivalTolerance)
    {
        m_velocity = sf::Vector2f(0, 0);
        m_acceleration = sf::Vector2f(0, 0);
    }
    else
    {
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
    }
}

void TankAurora::spawnElectricalParticleEffect()
{
}

