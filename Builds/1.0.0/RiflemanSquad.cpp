#include "RiflemanSquad.h"

RiflemanSquad::RiflemanSquad()
{
    initRifleman();
    initSquad();
    m_previousPosition = m_position;
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

void RiflemanSquad::update(sf::Time t_deltaTime, std::vector<Unit*>& m_allyUnits)
{
    InfantryUnit::update(t_deltaTime, m_allyUnits);

    movement(t_deltaTime);

    updateFormation();

    if (m_fireTimer > 0.0f)
    {
        m_fireTimer -= t_deltaTime.asSeconds();
    }
    if (m_isReloading)
    {
        m_reloadTimer -= t_deltaTime.asSeconds();
        if (m_reloadTimer <= 0.0f)
        {
            m_isReloading = false;
            m_shotsFired = 0;
        }
    }

    if (m_enemyUnits)
    {
        aimAt(*m_enemyUnits);
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

/// <summary>
/// Initialize unit squad enities
/// </summary>
void RiflemanSquad::initSquad()
{
    float angleStep = 360.0f / m_numberOfSoldiers;
    for (int i = 0; i < m_numberOfSoldiers; i++)
    {
        sf::Sprite soldier = m_unitSprite;
        float angle = angleStep * i;
        float radians = angle * (PI / 180.0f);

        sf::Vector2f offset(cos(radians) * m_scatterRadius, sin(radians) * m_scatterRadius);
        m_offsets.push_back(offset);

        soldier.setPosition(m_unitSprite.getPosition() + offset);
        m_entities.push_back(soldier);
    }
}

/// <summary>
/// Initialize the rifleman unit
/// </summary>
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

/// <summary>
/// This units movement
/// </summary>
/// <param name="t_deltaTime"></param>
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

/// <summary>
/// To updates the formation of this unit
/// </summary>
void RiflemanSquad::updateFormation()
{
    for (int i = 0; i < m_entities.size(); i++) 
    {
        m_entities[i].setPosition(m_unitSprite.getPosition() + m_offsets[i]);
        m_entities[i].setRotation(m_unitSprite.getRotation());
    }
}

/// <summary>
/// when the unit takes damage the enities die
/// </summary>
void RiflemanSquad::takeDamageSquad() 
{
    float healthPercentage = m_health / m_maxHealth;
    int newActiveEntitiesCount = static_cast<int>(std::ceil(healthPercentage * m_numberOfSoldiers));

    newActiveEntitiesCount = std::max(0, std::min(newActiveEntitiesCount, static_cast<int>(m_entities.size())));

    while (m_entities.size() > newActiveEntitiesCount)
    {
        m_entities.pop_back();
    }
}

/// <summary>
/// Removes the enities from the squad
/// </summary>
void RiflemanSquad::squadEntityRemoval()
{
    int activeEntities = static_cast<int>((m_health / m_maxHealth) * m_numberOfSoldiers);
    if (activeEntities < m_entities.size()) 
    {
        m_entities.erase(m_entities.begin() + activeEntities, m_entities.end());
    }
    updateFormation();
}

/// <summary>
/// Ragains the squad members 
/// </summary>
void RiflemanSquad::squadEntityRegain()
{
    int activeEntities = static_cast<int>((m_health / m_maxHealth) * m_numberOfSoldiers);
    activeEntities = std::min(activeEntities, m_numberOfSoldiers);

    while (m_entities.size() < activeEntities) 
    {
        sf::Sprite newEntity = m_unitSprite; 
        m_entities.push_back(newEntity);
    }
}

/// <summary>
/// Tries to aim and shoots at the target with the squad members
/// </summary>
/// <param name="m_enemyUnits"></param>
void RiflemanSquad::aimAt(const std::vector<Unit*>& m_enemyUnits)
{
    m_targetPositions.clear();  // Clear previous target positions at the beginning

    if (m_isReloading || m_fireTimer > 0.0f)
    {
        return;
    }

    bool m_enemyInRange = false;
    sf::Vector2f forwardDirection = rotateVector(sf::Vector2f(0, -1), m_unitSprite.getRotation());

    for (Unit* enemy : m_enemyUnits)
    {
        m_targetPositions.push_back(enemy->getPosition());
    }
    if (m_enemyBuildings)
    {
        for (Building* building : *m_enemyBuildings)
        {
            m_targetPositions.push_back(building->getPosition());
        }
    }

    for (const auto& targetPos : m_targetPositions)
    {
        sf::Vector2f toTarget = normalize(targetPos - this->getPosition());
        float distance = magnitude(targetPos - this->getPosition());

        if (distance <= m_viewRadius - 20)
        {
            float dotProduct = forwardDirection.x * toTarget.x + forwardDirection.y * toTarget.y;
            float angleToEnemy = acos(dotProduct) * (180.0f / PI);

            std::vector<sf::Sprite> allShootingEntities = m_entities;
            allShootingEntities.push_back(m_unitSprite); // to shoot units and buildings

            if (angleToEnemy <= 45.0f)
            {
                m_enemyInRange = true;

                for (auto& entity : allShootingEntities)
                {
                    float sprayAngle = (std::rand() % 11 - 5) * (PI / 180.0f);
                    float cosAngle = cos(sprayAngle);
                    float sinAngle = sin(sprayAngle);
                    sf::Vector2f sprayedDirection = normalize(sf::Vector2f(
                        toTarget.x * cosAngle - toTarget.y * sinAngle,
                        toTarget.x * sinAngle + toTarget.y * cosAngle));

                    sf::Vector2f bulletStartPosition = entity.getPosition() + sprayedDirection * 10.0f;
                    m_bullets.emplace_back(bulletStartPosition, sprayedDirection, m_bulletSpeed);
                    SoundManager::getInstance().playSound("Laser3");
                }
                m_shotsFired++;
                m_fireTimer = m_fireRate;

                if (m_shotsFired >= m_clipSize)
                {
                    m_isReloading = true;
                    m_reloadTimer = m_reloadTime;
                    m_shotsFired = 0;
                }
                break;
            }
        }
    }

    if (!m_enemyInRange)
    {
        m_fireTimer = m_fireRate;
    }
}
