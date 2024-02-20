#include "Buggy.h"

Buggy::Buggy()
{
	setupBuggy();
	m_cost = 700;
	m_speed = 120;
    m_health = 100;
    m_maxForce = 150;
    m_slowingRadius = 50;
    m_bulletSpeed = 300.0f;
}

Buggy::~Buggy()
{
}

void Buggy::update(sf::Time t_deltaTime, std::vector<Unit*>& allUnits)
{
	VehicleUnit::update(t_deltaTime, allUnits);
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
        aimWeapon(t_deltaTime, *enemyUnits);
    }
}

void Buggy::setupBuggy()
{
	if (!m_unitTexture.loadFromFile("Assets\\Images\\buggy.png"))
	{
		std::cout << "Error - Loading buggy Texture" << std::endl;
	}
	m_unitSprite.setTexture(m_unitTexture);
	m_unitSprite.setPosition(m_position);
	m_unitSprite.setRotation(180);
	m_unitSprite.setOrigin(m_unitSprite.getLocalBounds().width / 2, m_unitSprite.getLocalBounds().height / 2);
	m_unitSprite.setScale(0.12, 0.12);

    if (!m_weaponTexture.loadFromFile("Assets\\Images\\weapon1.png"))
    {
        std::cout << "Error - Loading buggy weapon Texture" << std::endl;
    }
    m_weaponSprite.setTexture(m_weaponTexture);
    m_weaponSprite.setOrigin(m_weaponTexture.getSize().x / 2, m_weaponTexture.getSize().y / 2);
    m_weaponSprite.setScale(0.1, 0.1);
}

void Buggy::movement(sf::Time t_deltaTime)
{
    if (m_targetPosition == m_position)
    {
        m_velocity = sf::Vector2f(0, 0);
        return;
    }

    sf::Vector2f toTarget = m_targetPosition - m_position;
    float distance = magnitude(toTarget);

    sf::Vector2f desiredDirection = normalize(toTarget);

    float speed = m_speed;
    if (distance < m_slowingRadius)
    {
        speed = m_speed * (distance / m_slowingRadius);
    }

    sf::Vector2f desiredVelocity = desiredDirection * speed;
    sf::Vector2f steeringForce = desiredVelocity - m_velocity;
    steeringForce = normalize(steeringForce) * m_maxForce;

    sf::Vector2f acceleration = steeringForce;
    m_velocity += acceleration * t_deltaTime.asSeconds();

    if (magnitude(m_velocity) > speed)
    {
        m_velocity = normalize(m_velocity) * speed;
    }

    m_position += m_velocity * t_deltaTime.asSeconds();
    setPosition(m_position);

    if (magnitude(m_velocity) > 0.0f) 
    { 
        float desiredAngle = angleFromVector(m_velocity);
        float currentAngle = m_unitSprite.getRotation();
        float rotationStep = m_rotationSpeed * t_deltaTime.asSeconds();

        float angleDifference = desiredAngle - currentAngle + 90;
        while (angleDifference < -180) angleDifference += 360;
        while (angleDifference > 180) angleDifference -= 360;

        if (angleDifference > 0)
        {
            rotationStep = std::min(rotationStep, angleDifference);
        }
        else
        {
            rotationStep = std::max(-rotationStep, angleDifference);
        }

        m_unitSprite.setRotation(currentAngle + rotationStep);
    }
}

void Buggy::setEnemyUnits(std::vector<Unit*>& enemyUnits)
{
    this->enemyUnits = &enemyUnits;
}

void Buggy::aimWeapon(sf::Time t_deltaTime, const std::vector<Unit*>& enemyUnits)
{
    if (enemyUnits.empty())
    {
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

    if (closestEnemy && closestDistance <= this->getViewRadius()) 
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
        sf::Vector2f bulletStartPosition = m_weaponSprite.getPosition();
        bullets.emplace_back(bulletStartPosition, directionToEnemy, m_bulletSpeed);
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
