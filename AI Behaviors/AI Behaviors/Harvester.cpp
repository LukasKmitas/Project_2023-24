#include "Harvester.h"

Harvester::Harvester()
{
	setupHarvester();
	m_cost = 1600;
	m_speed = 70;
	m_health = 100;
    m_maxForce = 100;
    m_slowingRadius = 70;
}

Harvester::~Harvester()
{
}

void Harvester::update(sf::Time t_deltaTime, std::vector<Unit*>& allyUnits)
{
    switch (currentState)
    {
    case Idle:
        
        break;
    case MovingToResource:
        findNearestResourceTile();
        if (isAtTargetPosition() && foundResource)
        {
            currentState = CollectingResource;
        }
        break;
    case CollectingResource:
        currentResourceLoad += collectionRate * t_deltaTime.asSeconds();
        //std::cout << "Current resource collected: " << currentResourceLoad << std::endl;
        if (currentResourceLoad >= maxResourceCapacity) 
        {
            currentResourceLoad = maxResourceCapacity;
            currentState = MovingToRefinery;
        }
        break;
    case MovingToRefinery:
        findNearestRefinery();
        if (isAtRefinery()) 
        {
            currentState = UnloadingResource;
        }
        break;
    case UnloadingResource:
        unloadResources(t_deltaTime);
        if (currentResourceLoad <= 0) 
        {
            currentState = MovingToResource;
        }
        break;
    }
	VehicleUnit::update(t_deltaTime, allyUnits);
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

void Harvester::setupHarvester()
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

void Harvester::findNearestRefinery()
{
    float minDistance = std::numeric_limits<float>::max();
    Refinery* nearestRefinery = nullptr;

    for (auto building : *allBuildings)
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

void Harvester::findNearestResourceTile()
{
    float minDistance = std::numeric_limits<float>::max();
    sf::Vector2f nearestResourcePos;

    for (int y = 0; y < tiles->size(); ++y)
    {
        for (int x = 0; x < (*tiles)[0].size(); ++x)
        {
            const Tile& tile = (*tiles)[y][x];
            if (!m_isEnemy && tile.isResource && (tile.fogStatus == Tile::FogStatus::Visible || tile.fogStatus == Tile::FogStatus::Explored))
            {
                sf::Vector2f tilePos(x * tile.tileSize, y * tile.tileSize);
                float distance = this->distance(this->getPosition(), tilePos);
                if (distance < minDistance)
                {
                    minDistance = distance;
                    nearestResourcePos = tilePos;
                }
            }
            if (m_isEnemy && tile.isResource)
            {
                sf::Vector2f tilePos(x * tile.tileSize, y * tile.tileSize);
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

void Harvester::unloadResources(sf::Time t_deltaTime) 
{
    float unloadingRate = 6.0f; 
    float unloadedResources = unloadingRate * t_deltaTime.asSeconds(); 
    currentResourceLoad -= unloadedResources; 
    currentResourceLoad = std::max(0.0f, currentResourceLoad);

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

void Harvester::moveToRefinery(Refinery* refinery)
{
    if (refinery) 
    {
        targetRefinery = refinery;
        currentState = MovingToRefinery;
        moveTo(refinery->getPosition());
    }
}

void Harvester::gettingResourcePosition(const sf::Vector2f& resourcePos)
{
    m_targetPosition = resourcePos + sf::Vector2f(25,25);
    //std::cout << "Resource tile found at: " << m_targetPosition.x << " " << m_targetPosition.y << std::endl;
    foundResource = true;
}

bool Harvester::isAtTargetPosition() 
{
    float distanceToTarget = distance(this->getPosition(), m_targetPosition);
    float arrivalThreshold = 50;
    return distanceToTarget <= arrivalThreshold;
}

bool Harvester::isAtRefinery() 
{
    if (targetRefinery != nullptr) 
    {
        float distanceToRefinery = distance(this->getPosition(), targetRefinery->getPosition());
        float arrivalThreshold = 25;
        return distanceToRefinery <= arrivalThreshold;
    }
    return false;
}

void Harvester::setBuildings(const std::vector<Building*>& buildings) 
{
    this->allBuildings = &buildings;
}

void Harvester::setTiles(const std::vector<std::vector<Tile>>& tiles)
{
    this->tiles = &tiles;
}