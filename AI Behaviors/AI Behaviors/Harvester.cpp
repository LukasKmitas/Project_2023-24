#include "Harvester.h"

Harvester::Harvester()
{
	setupHarvester();
}

Harvester::~Harvester()
{
}

void Harvester::update(sf::Time t_deltaTime)
{
}

void Harvester::render(sf::RenderWindow& m_window)
{
	m_window.draw(m_unitSprite);
}

void Harvester::setPosition(const sf::Vector2f& position) 
{
	m_unitSprite.setPosition(position);
}

void Harvester::setupHarvester()
{
	if (!m_unitTexture.loadFromFile("Assets\\Images\\HarvesterLogo.png"))
	{
		std::cout << "Error - Loading harvester Texture" << std::endl;
	}
	m_unitSprite.setTexture(m_unitTexture);
	m_unitSprite.setPosition(m_position);
	//m_unitSprite.setTextureRect(sf::IntRect(1, 229, 56, 75));
	m_unitSprite.setOrigin(m_unitSprite.getLocalBounds().width / 2, m_unitSprite.getLocalBounds().height / 2);
	//m_unitSprite.setScale(1, 1);
}
