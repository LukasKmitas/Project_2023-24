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

void Harvester::update(sf::Time t_deltaTime)
{
	movement(t_deltaTime);
}

void Harvester::render(sf::RenderWindow& m_window)
{
	m_window.draw(m_unitSprite);
}

void Harvester::movement(sf::Time t_deltaTime)
{
    if (m_position != m_targetPosition)
    {
        sf::Vector2f direction = m_targetPosition - m_position;
        float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);

        if (length != 0)
        {
            direction /= length;
        }

        float distanceToMove = t_deltaTime.asSeconds() * m_speed;

        if (length < distanceToMove)
        {
            m_unitSprite.setPosition(m_targetPosition);
            m_position = m_targetPosition;
        }
        else
        {
            m_unitSprite.move(direction * distanceToMove);
            m_position = m_unitSprite.getPosition();
        }
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
