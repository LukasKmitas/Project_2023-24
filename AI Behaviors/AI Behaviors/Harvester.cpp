#include "Harvester.h"

Harvester::Harvester()
{
	initHarvester();
	m_cost = 1600;
	m_speed = 70;
	m_health = 100;
    m_maxForce = 100;
    m_slowingRadius = 70;
}

Harvester::~Harvester()
{
}

void Harvester::update(sf::Time t_deltaTime, std::vector<Unit*>& m_allyUnits)
{
    switch (m_currentState)
    {
    case Idle:
        
        break;
    case MovingToResource:
        findNearestResourceTile();
        if (isAtTargetPosition() && m_foundResource)
        {
            m_currentState = CollectingResource;
        }
        break;
    case CollectingResource:
        m_currentResourceLoad += m_collectionRate * t_deltaTime.asSeconds();
        //std::cout << "Current resource collected: " << currentResourceLoad << std::endl;
        if (m_currentResourceLoad >= m_maxResourceCapacity) 
        {
            m_currentResourceLoad = m_maxResourceCapacity;
            m_currentState = MovingToRefinery;
        }
        break;
    case MovingToRefinery:
        findNearestRefinery();
        if (isAtRefinery()) 
        {
            m_currentState = UnloadingResource;
        }
        break;
    case UnloadingResource:
        unloadResources(t_deltaTime);
        if (m_currentResourceLoad <= 0) 
        {
            m_currentState = MovingToResource;
        }
        break;
    }
	VehicleUnit::update(t_deltaTime, m_allyUnits);
	movement(t_deltaTime);
}

void Harvester::movement(sf::Time t_deltaTime)
{
    if (m_targetPosition == m_position)
    {
        return;
    }

    float distanceToTarget = distance(m_position, m_targetPosition);

    if (distanceToTarget < 5.0f)
    {
        m_velocity = sf::Vector2f(0.0f, 0.0f);
        return;
    }

    sf::Vector2f steeringForce = steerTowards(m_targetPosition);
    sf::Vector2f acceleration = steeringForce;

    m_velocity += acceleration * t_deltaTime.asSeconds();

    if (distanceToTarget < m_slowingRadius)
    {
        float decelerationSpeed = m_speed * (distanceToTarget / m_slowingRadius);
        decelerationSpeed = std::max(decelerationSpeed, 10.0f);
        m_velocity = normalize(m_velocity) * decelerationSpeed;
    }

    if (magnitude(m_velocity) > m_speed) 
    {
        m_velocity = normalize(m_velocity) * m_speed;
    }

    m_position += m_velocity * t_deltaTime.asSeconds();
    setPosition(m_position);

    // Gradually rotate the unit to face its direction of travel
    if (magnitude(m_velocity) > 0) 
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

/// <summary>
/// initializes the harvester unit
/// </summary>
void Harvester::initHarvester()
{
	if (!m_unitTexture.loadFromFile("Assets\\Images\\Units\\Harvester.png"))
	{
		std::cout << "Error - Loading harvester Texture" << std::endl;
	}
	m_unitSprite.setTexture(m_unitTexture);
	m_unitSprite.setPosition(m_position);
	m_unitSprite.setTextureRect(sf::IntRect(0, 0, 136, 215));
    m_unitSprite.setRotation(180);
	m_unitSprite.setOrigin(m_unitSprite.getLocalBounds().width / 2, m_unitSprite.getLocalBounds().height / 2);
	m_unitSprite.setScale(0.2, 0.2);
}

/// <summary>
/// finds the nearest refinery building
/// </summary>
void Harvester::findNearestRefinery()
{
    float minDistance = std::numeric_limits<float>::max();
    Refinery* nearestRefinery = nullptr;

    for (auto building : *m_allBuildings)
    {
        if (Refinery* refinery = dynamic_cast<Refinery*>(building)) 
        {
            float distance = this->distance(this->getPosition(), refinery->getPosition());
            if (distance < minDistance) 
            {
                minDistance = distance;
                nearestRefinery = refinery;
            }
        }
    }

    if (nearestRefinery)
    {
        this->moveToRefinery(nearestRefinery);
    }
}

/// <summary>
/// To find the nearest resource tile 
/// </summary>
void Harvester::findNearestResourceTile()
{
    float minDistance = std::numeric_limits<float>::max();
    sf::Vector2f nearestResourcePos;

    for (int y = 0; y < m_tiles->size(); ++y)
    {
        for (int x = 0; x < (*m_tiles)[0].size(); ++x)
        {
            const Tile& tile = (*m_tiles)[y][x];
            if (!m_isEnemy && tile.m_isResource && (tile.m_fogStatus == Tile::FogStatus::Visible || tile.m_fogStatus == Tile::FogStatus::Explored))
            {
                sf::Vector2f tilePos(x * tile.m_tileSize, y * tile.m_tileSize);
                float distance = this->distance(this->getPosition(), tilePos);
                if (distance < minDistance)
                {
                    minDistance = distance;
                    nearestResourcePos = tilePos;
                }
            }
            if (m_isEnemy && tile.m_isResource)
            {
                sf::Vector2f tilePos(x * tile.m_tileSize, y * tile.m_tileSize);
                float distance = this->distance(this->getPosition(), tilePos);
                if (distance < minDistance)
                {
                    minDistance = distance;
                    nearestResourcePos = tilePos;
                }
            }
        }
    }

    if (minDistance != std::numeric_limits<float>::max())
    {
        gettingResourcePosition(nearestResourcePos);
    }
}

/// <summary>
/// Unloads the resouceses it collected
/// </summary>
/// <param name="t_deltaTime"></param>
void Harvester::unloadResources(sf::Time t_deltaTime) 
{
    float unloadingRate = 6.0f; 
    float unloadedResources = unloadingRate * t_deltaTime.asSeconds(); 
    m_currentResourceLoad -= unloadedResources; 
    m_currentResourceLoad = std::max(0.0f, m_currentResourceLoad);

    int conversionRate = 14;
    int addedCurrency = static_cast<int>(unloadedResources * conversionRate);
    if (m_isEnemy)
    {
        Global::enemyCurrency += addedCurrency;
    }
    else
    {
        Global::playerCurrency += addedCurrency;
    }
}

/// <summary>
/// Moves to the refinery position
/// </summary>
/// <param name="m_refinery"></param>
void Harvester::moveToRefinery(Refinery* m_refinery)
{
    if (m_refinery)
    {
        m_targetRefinery = m_refinery;
        m_currentState = MovingToRefinery;
        moveTo(m_refinery->getPosition());
    }
}

/// <summary>
/// Gets the resource position
/// </summary>
/// <param name="m_resourcePos"></param>
void Harvester::gettingResourcePosition(const sf::Vector2f& m_resourcePos)
{
    m_targetPosition = m_resourcePos + sf::Vector2f(25,25);
    //std::cout << "Resource tile found at: " << m_targetPosition.x << " " << m_targetPosition.y << std::endl;
    m_foundResource = true;
}

/// <summary>
/// checks if its at target
/// </summary>
/// <returns></returns>
bool Harvester::isAtTargetPosition() 
{
    float distanceToTarget = distance(this->getPosition(), m_targetPosition);
    float arrivalThreshold = 50;
    return distanceToTarget <= arrivalThreshold;
}

/// <summary>
/// checks if its at the refinery building
/// </summary>
/// <returns></returns>
bool Harvester::isAtRefinery() 
{
    if (m_targetRefinery != nullptr) 
    {
        float distanceToRefinery = distance(this->getPosition(), m_targetRefinery->getPosition());
        float arrivalThreshold = 25;
        return distanceToRefinery <= arrivalThreshold;
    }
    return false;
}

/// <summary>
/// sets the buildings to find the refinery buildings
/// </summary>
/// <param name="m_allyUnits"></param>
void Harvester::setBuildings(const std::vector<Building*>& m_allyUnits)
{
    this->m_allBuildings = &m_allyUnits;
}

/// <summary>
/// sets the tiles to find resources tile
/// </summary>
/// <param name="m_tiles"></param>
void Harvester::setTiles(const std::vector<std::vector<Tile>>& m_tiles)
{
    this->m_tiles = &m_tiles;
}