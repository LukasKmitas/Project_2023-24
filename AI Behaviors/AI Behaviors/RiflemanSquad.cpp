#include "RiflemanSquad.h"

RiflemanSquad::RiflemanSquad()
{
    initRifleman();
    initSquad();
    previousPosition = m_position;
    m_cost = 100;
    m_health = 100;
    m_viewRadius = 300;
    m_speed = 90;
    m_maxForce = 100;
    m_damage = 1.0f;
    m_bulletSpeed = 300.0f;
}

RiflemanSquad::~RiflemanSquad()
{
}

void RiflemanSquad::update(sf::Time t_deltaTime, std::vector<Unit*>& allyUnits)
{
    InfantryUnit::update(t_deltaTime, allyUnits);

    movement(t_deltaTime);

    updateFormation();

    if (fireTimer > 0.0f)
    {
        fireTimer -= t_deltaTime.asSeconds();
    }
    if (isReloading)
    {
        reloadTimer -= t_deltaTime.asSeconds();
        if (reloadTimer <= 0.0f)
        {
            isReloading = false;
            shotsFired = 0;
        }
    }

    if (enemyUnits)
    {
        aimAt(*enemyUnits);
    }
}

void RiflemanSquad::render(sf::RenderWindow& m_window)
{
    InfantryUnit::render(m_window);

    for (auto& soldier : m_entities)
    {
        m_window.draw(soldier);
    }
}

void RiflemanSquad::initSquad()
{
    float angleStep = 360.0f / numberOfSoldiers;
    for (int i = 0; i < numberOfSoldiers; i++)
    {
        sf::Sprite soldier = m_unitSprite;
        float angle = angleStep * i;
        float radians = angle * (PI / 180.0f);

        sf::Vector2f offset(cos(radians) * scatterRadius, sin(radians) * scatterRadius);
        m_offsets.push_back(offset);

        soldier.setPosition(m_unitSprite.getPosition() + offset);
        m_entities.push_back(soldier);
    }
}

void RiflemanSquad::initRifleman()
{
    if (!m_unitTexture.loadFromFile("Assets\\Images\\Units\\Rifleman.png"))
    {
        std::cout << "Error - Loading Tank Aurora Texture" << std::endl;
    }
    m_unitSprite.setTexture(m_unitTexture);
    m_unitSprite.setPosition(m_position);
    m_unitSprite.setRotation(180);
    m_unitSprite.setOrigin(m_unitSprite.getLocalBounds().width / 2, m_unitSprite.getLocalBounds().height / 2);
    m_unitSprite.setScale(0.15, 0.15);
}

void RiflemanSquad::movement(sf::Time t_deltaTime)
{
    float distance = magnitude(m_targetPosition - m_position);
    if (distance > m_arrivalTolerance)
    {
        sf::Vector2f desiredDirection = normalize(m_targetPosition - m_position);

        float speed = m_speed;
        if (distance < m_slowingRadius)
        {
            speed *= (distance / m_slowingRadius);
        }

        sf::Vector2f desiredVelocity = desiredDirection * speed;
        sf::Vector2f steeringForce = desiredVelocity - m_velocity;
        m_acceleration = normalize(steeringForce) * m_maxForce;
        m_velocity += m_acceleration * t_deltaTime.asSeconds();

        if (magnitude(m_velocity) > m_speed)
        {
            m_velocity = normalize(m_velocity) * m_speed;
        }

        m_position += m_velocity * t_deltaTime.asSeconds();
        setPosition(m_position);
    }
    else
    {
        m_velocity = sf::Vector2f(0, 0);
        m_acceleration = sf::Vector2f(0, 0);
    }

    orientSpriteToMovement(t_deltaTime);
}

void RiflemanSquad::updateFormation()
{
    for (int i = 0; i < m_entities.size(); i++) 
    {
        m_entities[i].setPosition(m_unitSprite.getPosition() + m_offsets[i]);
        m_entities[i].setRotation(m_unitSprite.getRotation());
    }
}

void RiflemanSquad::takeDamageSquad() 
{
    float healthPercentage = m_health / m_maxHealth;
    int newActiveEntitiesCount = static_cast<int>(std::ceil(healthPercentage * numberOfSoldiers));

    newActiveEntitiesCount = std::max(0, std::min(newActiveEntitiesCount, static_cast<int>(m_entities.size())));

    while (m_entities.size() > newActiveEntitiesCount)
    {
        m_entities.pop_back();
    }
}

void RiflemanSquad::squadEntityRemoval()
{
    int activeEntities = static_cast<int>((m_health / m_maxHealth) * numberOfSoldiers);
    if (activeEntities < m_entities.size()) 
    {
        m_entities.erase(m_entities.begin() + activeEntities, m_entities.end());
    }
    updateFormation();
}

void RiflemanSquad::squadEntityRegain()
{
    int activeEntities = static_cast<int>((m_health / m_maxHealth) * numberOfSoldiers);

    activeEntities = std::min(activeEntities, numberOfSoldiers);

    while (m_entities.size() < activeEntities) 
    {
        sf::Sprite newEntity = m_unitSprite; 
        m_entities.push_back(newEntity);
    }
}

void RiflemanSquad::aimAt(const std::vector<Unit*>& enemyUnits)
{
    if (isReloading || fireTimer > 0.0f)
    {
        return;
    }

    bool enemyInRange = false;
    sf::Vector2f forwardDirection = rotateVector(sf::Vector2f(0, -1), m_unitSprite.getRotation());

    for (Unit* enemy : enemyUnits)
    {
        targetPositions.push_back(enemy->getPosition());
    }
    if (enemyBuildings)
    {
        for (Building* building : *enemyBuildings)
        {
            targetPositions.push_back(building->getPosition());
        }
    }

    for (const auto& targetPos : targetPositions)
    {
        sf::Vector2f toTarget = normalize(targetPos - this->getPosition());
        float distance = magnitude(targetPos - this->getPosition());

        if (distance <= m_viewRadius - 20)
        {
            float dotProduct = forwardDirection.x * toTarget.x + forwardDirection.y * toTarget.y;
            float angleToEnemy = acos(dotProduct) * (180.0f / PI);

            std::vector<sf::Sprite> allShootingEntities = m_entities;
            allShootingEntities.push_back(m_unitSprite); // Including the unit and the entities in shooting

            if (angleToEnemy <= 45.0f)
            {
                enemyInRange = true;

                for (auto& entity : allShootingEntities)
                {
                    float sprayAngle = (std::rand() % 11 - 5) * (PI / 180.0f);
                    float cosAngle = cos(sprayAngle);
                    float sinAngle = sin(sprayAngle);
                    sf::Vector2f sprayedDirection = normalize(sf::Vector2f(
                        toTarget.x * cosAngle - toTarget.y * sinAngle,
                        toTarget.x * sinAngle + toTarget.y * cosAngle));

                    sf::Vector2f bulletStartPosition = entity.getPosition() + sprayedDirection * 10.0f;
                    bullets.emplace_back(bulletStartPosition, sprayedDirection, m_bulletSpeed);
                }
                shotsFired++;
                fireTimer = fireRate;

                if (shotsFired >= clipSize)
                {
                    isReloading = true;
                    reloadTimer = reloadTime;
                    shotsFired = 0;
                }
                break;
            }
        }
    }

    if (!enemyInRange)
    {
        fireTimer = fireRate;
    }
}
