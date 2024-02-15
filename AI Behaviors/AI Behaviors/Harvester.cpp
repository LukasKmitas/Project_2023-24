#include "Harvester.h"

Harvester::Harvester()
{
	setupHarvester();
	m_cost = 1600;
	m_speed = 80;
	m_health = 100;
    m_maxForce = 1;
}

Harvester::~Harvester()
{
}

void Harvester::update(sf::Time t_deltaTime, std::vector<Unit*>& allUnits)
{
    switch (currentState)
    {
    case Idle:
        
        break;
    case MovingToResource:
        findNearestResourceTile();
        if (isAtTargetPosition()) 
        {
            currentState = CollectingResource;
        }
        break;
    case CollectingResource:
        currentResourceLoad += collectionRate * t_deltaTime.asSeconds();
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
	VehicleUnit::update(t_deltaTime, allUnits);
	movement(t_deltaTime);
}

void Harvester::movement(sf::Time t_deltaTime)
{
    if (magnitude(m_velocity) > 0) 
    {
        float rotationAngle = angleFromVector(m_velocity);
        m_unitSprite.setRotation(rotationAngle + 90);
    }
}

void Harvester::setupHarvester()
{
	if (!m_unitTexture.loadFromFile("Assets\\Images\\Harvester.png"))
	{
		std::cout << "Error - Loading harvester Texture" << std::endl;
	}
	m_unitSprite.setTexture(m_unitTexture);
	m_unitSprite.setPosition(m_position);
	m_unitSprite.setTextureRect(sf::IntRect(0, 0, 136, 215));
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
            if (tile.isResource && (tile.fogStatus == Tile::FogStatus::Visible || tile.fogStatus == Tile::FogStatus::Explored))
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
        moveToResource(nearestResourcePos);
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

    Global::currency += addedCurrency;
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

void Harvester::moveToResource(const sf::Vector2f& resourcePos)
{
    m_targetPosition = resourcePos + sf::Vector2f(25,25);
    currentState = MovingToResource;
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