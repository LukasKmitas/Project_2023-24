#include "Buggy.h"

Buggy::Buggy()
{
	setupBuggy();
	m_cost = 700;
    m_health = 100;
    m_viewRadius = 300;
    m_speed = 120;

    m_maxForce = 150;
    m_slowingRadius = 100;
    m_bulletSpeed = 300.0f;
    rotationSpeedDegreesPerSecond = 180;
}

Buggy::~Buggy()
{
}

void Buggy::update(sf::Time t_deltaTime, std::vector<Unit*>& allyUnits)
{
	VehicleUnit::update(t_deltaTime, allyUnits);

    movement(t_deltaTime);
    m_weaponSprite.setPosition(m_unitSprite.getPosition());

    if (fireTimer > 0.0f)
    {
        fireTimer -= t_deltaTime.asSeconds();
    }

    // Handle reloading
    if (isReloading) 
    {
        reloadTimer -= t_deltaTime.asSeconds();
        if (reloadTimer <= 0.0f) 
        {
            isReloading = false;
        }
    }

    if (enemyUnits) 
    {
        aimWeapon(*enemyUnits);
    }
}

void Buggy::setupBuggy()
{
	if (!m_unitTexture.loadFromFile("Assets\\Images\\Units\\buggy.png"))
	{
		std::cout << "Error - Loading buggy Texture" << std::endl;
	}
	m_unitSprite.setTexture(m_unitTexture);
	m_unitSprite.setPosition(m_position);
	m_unitSprite.setRotation(180);
	m_unitSprite.setOrigin(m_unitSprite.getLocalBounds().width / 2, m_unitSprite.getLocalBounds().height / 2);
	m_unitSprite.setScale(0.12, 0.12);

    if (!m_weaponTexture.loadFromFile("Assets\\Images\\Units\\weapon1.png"))
    {
        std::cout << "Error - Loading buggy weapon Texture" << std::endl;
    }
    m_weaponSprite.setTexture(m_weaponTexture);
    m_weaponSprite.setOrigin(m_weaponTexture.getSize().x / 2, m_weaponTexture.getSize().y / 2);
    m_weaponSprite.setScale(0.1, 0.1);
}

void Buggy::movement(sf::Time t_deltaTime)
{
    float distance = magnitude(m_targetPosition - m_position);
    if (distance < m_arrivalTolerance)
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

    // Orient the sprite towards the direction of movement
    orientSpriteToMovement(t_deltaTime);
}

void Buggy::aimWeapon(const std::vector<Unit*>& enemyUnits)
{
    if (enemyUnits.empty())
    {
        m_weaponSprite.setRotation(m_unitSprite.getRotation());
        return;
    }
     
    closestDistance = std::numeric_limits<float>::max();

    for (Unit* enemy : enemyUnits)
    {
        float distance = this->distance(this->getPosition(), enemy->getPosition());

        if (distance < closestDistance) 
        {
            closestDistance = distance;
            closestEnemy = enemy;
        }
    }

    if (closestEnemy && closestDistance <= this->getViewRadius() - 50) 
    {
        directionToEnemy = normalize(closestEnemy->getPosition() - this->getPosition());
        float angleDegrees = angleFromVector(directionToEnemy);
        m_weaponSprite.setRotation(angleDegrees + 90);
        shootAtEnemy();
    }
    else
    {
        m_weaponSprite.setRotation(m_unitSprite.getRotation());
    }
}

void Buggy::shootAtEnemy()
{
    if (!isReloading && fireTimer <= 0.0f) 
    {
        // create a random spray between -2.5 to 2.5 degrees
        float sprayAngle = (std::rand() % 6 - 2.5) * (3.14159265 / 180);

        float cosAngle = std::cos(sprayAngle);
        float sinAngle = std::sin(sprayAngle);
        sf::Vector2f sprayedDirection = 
        {
            directionToEnemy.x * cosAngle - directionToEnemy.y * sinAngle,
            directionToEnemy.x * sinAngle + directionToEnemy.y * cosAngle
        };

        sf::Vector2f bulletStartPosition = m_weaponSprite.getPosition();
        bullets.emplace_back(bulletStartPosition, sprayedDirection, m_bulletSpeed);
        shotsFired++;
        fireTimer = fireRate;

        if (shotsFired >= clip)
        {
            isReloading = true;
            shotsFired = 0;
            reloadTimer = reloadTime;
        }
    }
}
