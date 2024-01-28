#include "Harvester.h"

Harvester::Harvester()
{
	setupHarvester();
	m_cost = 1600;
	m_speed = 80;
	m_health = 100;
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
    
        break;
    case CollectingResource:
        
        break;
    case MovingToRefinery:
        findNearestRefinery();
        break;
    case UnloadingResource:
       
        break;
    }

	Vehicle::update(t_deltaTime, allUnits);
	movement(t_deltaTime);
}

void Harvester::render(sf::RenderWindow& m_window)
{
	m_window.draw(m_unitSprite);
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
	m_unitSprite.setScale(0.3, 0.3);
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

void Harvester::moveToRefinery(Refinery* refinery)
{
    if (refinery) 
    {
        currentState = MovingToRefinery;
        moveTo(refinery->getPosition());
    }
}

void Harvester::setBuildings(const std::vector<Building*>& buildings) 
{
    this->allBuildings = &buildings;
}