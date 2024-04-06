#include "HammerHead.h"

HammerHead::HammerHead()
{
	initHammerhead();
	m_cost = 1500;
	m_health = 100;
    m_viewRadius = 310;
    m_speed = 150;

	m_slowingRadius = 100.0f;
    m_rotationSpeed = 80;
    m_maxForce = 200;
    m_rotationSpeed = 90;
}

HammerHead::~HammerHead()
{
}

void HammerHead::update(sf::Time t_deltaTime, std::vector<Unit*>& m_allyUnits)
{
    AircraftUnit::update(t_deltaTime, m_allyUnits);

    movement(t_deltaTime);
    orientSpriteToMovement(t_deltaTime);

    if (m_fireTimer > 0.0f)
    {
        m_fireTimer -= t_deltaTime.asSeconds();
    }

    if (m_enemyUnits)
    {
        aimWeapons(*m_enemyUnits);
    }

    spawnParticleExhaustEffect();
}

void HammerHead::render(sf::RenderWindow& m_window)
{
    AircraftUnit::render(m_window);

    m_window.draw(m_leftGunSprite);
    m_window.draw(m_rightGunSprite);
}

/// <summary>
/// initiliazes the hammerhead unit 
/// </summary>
void HammerHead::initHammerhead()
{
	if (!m_unitTexture.loadFromFile("Assets\\Images\\Units\\HammerHead.png"))
	{
		std::cout << "Error - Loading HammerHead Texture" << std::endl;
	}
	m_unitSprite.setTexture(m_unitTexture);
	m_unitSprite.setPosition(m_position);
	m_unitSprite.setOrigin(m_unitSprite.getLocalBounds().width / 2, m_unitSprite.getLocalBounds().height / 2);
	m_unitSprite.setScale(0.13, 0.13);

    if (!m_weaponTexture.loadFromFile("Assets\\Images\\Units\\weapon1.png"))
    {
        std::cout << "Error - Loading Gun Texture for hammerhead unit" << std::endl;
    }
    m_leftGunSprite.setTexture(m_weaponTexture);
    m_rightGunSprite.setTexture(m_weaponTexture);

    m_leftGunSprite.setOrigin(m_weaponTexture.getSize().x / 2, m_weaponTexture.getSize().y / 2);
    m_leftGunSprite.setScale(0.1, 0.1);
    m_rightGunSprite.setOrigin(m_weaponTexture.getSize().x / 2, m_weaponTexture.getSize().y / 2);
    m_rightGunSprite.setScale(0.1, 0.1);
}

/// <summary>
/// creates a fire/exhaust particle effect at the back of the unit 
/// </summary>
void HammerHead::spawnParticleExhaustEffect()
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

/// <summary>
/// makes the unit move
/// </summary>
/// <param name="t_deltaTime"></param>
void HammerHead::movement(sf::Time t_deltaTime)
{
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
}

/// <summary>
/// Makes it aim at the enemy
/// </summary>
/// <param name="m_enemyUnits"></param>
void HammerHead::aimWeapons(const std::vector<Unit*>& m_enemyUnits)
{
    if (m_enemyUnits.empty())
    {
        m_leftGunSprite.setRotation(m_unitSprite.getRotation());
        m_rightGunSprite.setRotation(m_unitSprite.getRotation());
        return;
    }

    float orientationAdjustment = -90; 
    float neutralAngle = m_unitSprite.getRotation() + orientationAdjustment;
    float weaponOffsetDistance = 30.0f; 
    float radians = (neutralAngle - orientationAdjustment) * (PI / 180.0f);

    // Calculate weapon positions
    sf::Vector2f leftWeaponPosition = m_position + sf::Vector2f(cos(radians - PI) * weaponOffsetDistance, sin(radians - PI) * weaponOffsetDistance);
    sf::Vector2f rightWeaponPosition = m_position + sf::Vector2f(cos(radians) * weaponOffsetDistance, sin(radians) * weaponOffsetDistance);

    m_leftGunSprite.setPosition(leftWeaponPosition);
    m_rightGunSprite.setPosition(rightWeaponPosition);

    // Initialize closest enemy data
    m_closestDistance = std::numeric_limits<float>::max();

    for (Unit* enemy : m_enemyUnits)
    {
        float distance = this->distance(this->getPosition(), enemy->getPosition());

        sf::Vector2f toEnemy = enemy->getPosition() - this->getPosition();
        float angleToEnemy = atan2(toEnemy.y, toEnemy.x) * (180.0f / PI);
        float angleDifference = fmod(neutralAngle - angleToEnemy + 360.0f, 360.0f);

        if (angleDifference > 180.0f)
        {
            angleDifference -= 360.0f;
        }
        if (distance < m_closestDistance && distance <= m_viewRadius && abs(angleDifference) <= 90)
        {
            m_closestDistance = distance;
            m_closestEnemy = enemy;
        }
    }

    for (Building* building : *m_enemyBuildings)
    {
        float distance = this->distance(this->getPosition(), building->getPosition());

        sf::Vector2f toBuilding = building->getPosition() - this->getPosition();
        float angleToBuilding = atan2(toBuilding.y, toBuilding.x) * (180.0f / PI);
        float angleDifference = fmod(neutralAngle - angleToBuilding + 360.0f, 360.0f);

        if (angleDifference > 180.0f)
        {
            angleDifference -= 360.0f;
        }
        if (distance < m_closestDistance && distance <= m_viewRadius && abs(angleDifference) <= 90)
        {
            m_closestDistance = distance;
            m_closestBuilding = building;
        }
    }

    if (m_closestEnemy && m_closestDistance <= this->getViewRadius() - 30)
    {
        // Aim at enemy
        m_directionToEnemy = normalize(m_closestEnemy->getPosition() - this->getPosition());
        float angleDegrees = angleFromVector(m_directionToEnemy);
        m_leftGunSprite.setRotation(angleDegrees + 90);
        m_rightGunSprite.setRotation(angleDegrees + 90);
        shootAtEnemy();
    }
    else if (m_closestBuilding && m_closestDistance <= this->getViewRadius() - 30)
    {
        m_directionToEnemy = normalize(m_closestBuilding->getPosition() - this->getPosition());
        float angleDegrees = angleFromVector(m_directionToEnemy);
        m_leftGunSprite.setRotation(angleDegrees + 90);
        m_rightGunSprite.setRotation(angleDegrees + 90);
        shootAtEnemy();
    }
    else 
    {
        m_leftGunSprite.setRotation(m_unitSprite.getRotation());
        m_rightGunSprite.setRotation(m_unitSprite.getRotation());
    }
}

/// <summary>
/// shoots bullets
/// </summary>
void HammerHead::shootAtEnemy()
{
    if (m_fireTimer <= 0.0f)
    {
        m_fireTimer = m_fireRate;

        float sprayAngle = (std::rand() % 11 - 5) * (3.14159265 / 180);

        float cosAngle = std::cos(sprayAngle);
        float sinAngle = std::sin(sprayAngle);
        sf::Vector2f sprayedDirection =
        {
            m_directionToEnemy.x * cosAngle - m_directionToEnemy.y * sinAngle,
            m_directionToEnemy.x * sinAngle + m_directionToEnemy.y * cosAngle
        };

        sf::Vector2f bulletStartPosition1 = m_leftGunSprite.getPosition();
        sf::Vector2f bulletStartPosition2 = m_rightGunSprite.getPosition();

        m_bullets.emplace_back(bulletStartPosition1, sprayedDirection, m_bulletSpeed);
        m_bullets.emplace_back(bulletStartPosition2, sprayedDirection, m_bulletSpeed);
    }
}
