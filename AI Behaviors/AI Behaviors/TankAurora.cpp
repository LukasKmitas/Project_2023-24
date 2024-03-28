#include "TankAurora.h"

TankAurora::TankAurora()
{
	setupTankAurora();
    setupHealingAura();
    setupEnergyWave();
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

    if (isChargingEMPWave)
    {
        spawnElectricalParticleEffect(t_deltaTime);
        currentChargeTime += t_deltaTime.asSeconds();
        if (currentChargeTime >= empChargeTime)
        {
            emitEMP();
        }
    }
    
    if (isFiringEnergyWave)
    {
        energyWavePosition += directionToEnemy * energyWaveSpeed * t_deltaTime.asSeconds();
        m_energyWaveSprite.setPosition(energyWavePosition);
        energyWaveSize += energyWaveGrowthRate * t_deltaTime.asSeconds();

        if (energyWaveSize >= energyWaveMaxSize)
        {
            resetEnergyWave();
        }
        else
        {
            m_energyWaveSprite.setScale(energyWaveSize, energyWaveSize);
        }

        for (Unit* enemy : *enemyUnits) 
        {
            if (m_energyWaveSprite.getGlobalBounds().intersects(enemy->getSprite().getGlobalBounds()))
            {
                enemy->applySlowEffect(0.0f, 3.0f, 3.0f);
            }
        }
    }
}

void TankAurora::render(sf::RenderWindow& m_window)
{
    m_window.draw(m_healingAura, &m_healingAuraShader);

    VehicleUnit::render(m_window);

    m_particleSystem.render(m_window);

    if (isFiringEnergyWave) 
    {
        m_window.draw(m_energyWaveSprite);
    }
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

    if (!m_lightningTexture.loadFromFile("Assets\\Images\\Particles\\OneLightning.png"))
    {
        std::cout << "Error - Loading Lightning Texture" << std::endl;
    }
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

void TankAurora::setupEnergyWave()
{
    if (!m_energyWaveTexture.loadFromFile("Assets\\Images\\Units\\EnergyWave.png"))
    {
        std::cout << "Error - Loading Energy wave Texture" << std::endl;
    }
    m_energyWaveSprite.setTexture(m_energyWaveTexture);
    m_energyWaveSprite.setPosition(m_position);
    m_energyWaveSprite.setRotation(m_unitSprite.getRotation() - 90);
    m_energyWaveSprite.setOrigin(m_energyWaveSprite.getGlobalBounds().width / 2, m_energyWaveSprite.getGlobalBounds().height / 2);
    m_energyWaveSprite.setScale(energyWaveSize, energyWaveSize);
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

void TankAurora::spawnElectricalParticleEffect(sf::Time t_deltaTime)
{
    float chargeRatio = currentChargeTime / empChargeTime;
    float size = std::min(chargeRatio * empExpandArea, empExpandArea);

    int numParticles = static_cast<int>(chargeRatio * 3);
    for (int i = 0; i < numParticles; ++i)
    {
        float angle = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2 * PI;
        float distanceFromCenter = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * size;

        sf::Vector2f positionOffset = rotateVector(sf::Vector2f(0, -20), m_unitSprite.getRotation());
        sf::Vector2f particlePos = m_position + rotateVector(sf::Vector2f(cos(angle) * distanceFromCenter, sin(angle) * distanceFromCenter), m_unitSprite.getRotation()) + positionOffset;

        sf::Vector2f velocity(0, 0);
        sf::Color color = sf::Color::White;
        float lifetime = 0.2f;
        float particleSize = 1.0f + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 5.0f;

        Particle particle(particlePos, velocity, color, lifetime, particleSize, &m_lightningTexture);
        m_particleSystem.addParticle(particle);
    }
}

void TankAurora::startEMPCharge()
{
    isChargingEMPWave = true;
    currentChargeTime = 0.0f;
}

void TankAurora::emitEMP() 
{
    sf::Vector2f forwardDirection = rotateVector(sf::Vector2f(0, -1), m_unitSprite.getRotation());

    for (Unit* enemy : *enemyUnits)
    {
        sf::Vector2f toEnemy = enemy->getPosition() - this->getPosition();
        float distance = magnitude(toEnemy);

        if (distance <= m_viewRadius - 20)
        {
            directionToEnemy = normalize(toEnemy);
            float angleToEnemy = angleBetweenVectors(forwardDirection, directionToEnemy);

            if (std::abs(angleToEnemy) <= 45)
            {
                isChargingEMPWave = false;
                currentChargeTime = 0.0f;
               
                isFiringEnergyWave = true;
                energyWavePosition = m_position;

                m_energyWaveSprite.setPosition(energyWavePosition);
                m_energyWaveSprite.setRotation(m_unitSprite.getRotation() - 90);
                m_energyWaveSprite.setScale(energyWaveSize, energyWaveSize);

                break; 
            }
        }
    }
}

sf::Vector2f TankAurora::rotateVector(sf::Vector2f vector, float angleDegrees)
{
    float rad = angleDegrees * PI / 180.0f;
    return sf::Vector2f
    (
        vector.x * cos(rad) - vector.y * sin(rad),
        vector.x * sin(rad) + vector.y * cos(rad)
    );
}

float TankAurora::angleBetweenVectors(sf::Vector2f vec1, sf::Vector2f vec2)
{
    float dot = vec1.x * vec2.x + vec1.y * vec2.y;
    float det = vec1.x * vec2.y - vec1.y * vec2.x;
    float angle = atan2(det, dot) * (180 / PI);
    return angle;
}

void TankAurora::resetEnergyWave()
{
    isFiringEnergyWave = false;
    energyWaveSize = 0.4f; 
    m_energyWaveSprite.setScale(energyWaveSize, energyWaveSize);
    startEMPCharge();
}