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

void TankAurora::update(sf::Time t_deltaTime, std::vector<Unit*>& m_allUnits)
{
	VehicleUnit::update(t_deltaTime, m_allUnits);

    movement(t_deltaTime);

    m_healingAura.setPosition(getPosition());
    
    float currentTime = m_shaderClock.getElapsedTime().asSeconds();
    m_healingAuraShader.setUniform("time", currentTime);
    sf::Glsl::Vec4 outlineColor(0, 1, 0, 0.3);
    m_healingAuraShader.setUniform("outlineColor", outlineColor);

    for (Unit* unit : m_allUnits)
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

    if (m_isChargingEMPWave)
    {
        spawnElectricalParticleEffect(t_deltaTime);
        m_currentChargeTime += t_deltaTime.asSeconds();
        if (m_currentChargeTime >= m_empChargeTime)
        {
            emitEMP();
        }
    }
    
    if (m_isFiringEnergyWave)
    {
        m_energyWavePosition += m_directionToEnemy * m_energyWaveSpeed * t_deltaTime.asSeconds();
        m_energyWaveSprite.setPosition(m_energyWavePosition);
        m_energyWaveSize += m_energyWaveGrowthRate * t_deltaTime.asSeconds();

        if (m_energyWaveSize >= m_energyWaveMaxSize)
        {
            resetEnergyWave();
        }
        else
        {
            m_energyWaveSprite.setScale(m_energyWaveSize, m_energyWaveSize);
        }

        for (Unit* enemy : *m_enemyUnits) 
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

    m_waveShader.setUniform("time", m_clock.getElapsedTime().asSeconds());

    if (m_isFiringEnergyWave) 
    {
        m_window.draw(m_energyWaveSprite, &m_waveShader);
    }
}

/// <summary>
/// Initialize tank Aurora
/// </summary>
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

/// <summary>
/// Initialize the healing aura
/// </summary>
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

/// <summary>
/// Initialize the energy wave
/// </summary>
void TankAurora::setupEnergyWave()
{
    if (!m_energyWaveTexture.loadFromFile("Assets\\Images\\Units\\EnergyWave.png"))
    {
        std::cout << "Error - Loading Energy wave Texture" << std::endl;
    }
    if (!m_waveShader.loadFromFile("Assets\\Shaders\\vertexShader2.vert", "Assets\\Shaders\\EnergyWave.frag")) 
    {
        std::cerr << "Failed to load shader" << std::endl;
    }
    m_energyWaveSprite.setTexture(m_energyWaveTexture);
    m_energyWaveSprite.setPosition(m_position);
    m_energyWaveSprite.setRotation(m_unitSprite.getRotation() - 90);
    m_energyWaveSprite.setOrigin(m_energyWaveSprite.getGlobalBounds().width / 2, m_energyWaveSprite.getGlobalBounds().height / 2);
    m_energyWaveSprite.setScale(m_energyWaveSize, m_energyWaveSize);
}

/// <summary>
/// Makes this unit move
/// </summary>
/// <param name="t_deltaTime"></param>
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

/// <summary>
/// Creates the energy particle effect to charge up the wave attack
/// </summary>
/// <param name="t_deltaTime"></param>
void TankAurora::spawnElectricalParticleEffect(sf::Time t_deltaTime)
{
    float chargeRatio = m_currentChargeTime / m_empChargeTime;
    float size = std::min(chargeRatio * m_empExpandArea, m_empExpandArea);

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

/// <summary>
/// starts the charge up for the EMP 
/// </summary>
void TankAurora::startEMPCharge()
{
    m_isChargingEMPWave = true;
    m_currentChargeTime = 0.0f;
}

/// <summary>
/// After charging the emp it shoots out the energy wave to slow down enemies
/// </summary>
void TankAurora::emitEMP() 
{
    sf::Vector2f forwardDirection = rotateVector(sf::Vector2f(0, -1), m_unitSprite.getRotation());

    for (Unit* enemy : *m_enemyUnits)
    {
        sf::Vector2f toEnemy = enemy->getPosition() - this->getPosition();
        float distance = magnitude(toEnemy);

        if (distance <= m_viewRadius - 20)
        {
            m_directionToEnemy = normalize(toEnemy);
            float angleToEnemy = angleBetweenVectors(forwardDirection, m_directionToEnemy);

            if (std::abs(angleToEnemy) <= 45)
            {
                m_isChargingEMPWave = false;
                m_currentChargeTime = 0.0f;
               
                m_isFiringEnergyWave = true;
                m_energyWavePosition = m_position;

                m_energyWaveSprite.setPosition(m_energyWavePosition);
                m_energyWaveSprite.setRotation(m_unitSprite.getRotation() - 90);
                m_energyWaveSprite.setScale(m_energyWaveSize, m_energyWaveSize);

                break; 
            }
        }
    }
}

/// <summary>
/// Reset the energy wave
/// </summary>
void TankAurora::resetEnergyWave()
{
    m_isFiringEnergyWave = false;
    m_energyWaveSize = 0.4f; 
    m_energyWaveSprite.setScale(m_energyWaveSize, m_energyWaveSize);
    startEMPCharge();
}