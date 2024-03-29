#include "RiflemanSquad.h"

RiflemanSquad::RiflemanSquad()
{
    initRifleman();
    initSquad();
    previousPosition = m_position;
    m_cost = 100;
    m_health = 80;
    m_viewRadius = 300;
    m_speed = 90;
    m_maxForce = 100;
}

RiflemanSquad::~RiflemanSquad()
{
}

void RiflemanSquad::update(sf::Time t_deltaTime, std::vector<Unit*>& allyUnits)
{
    InfantryUnit::update(t_deltaTime, allyUnits);

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

    updateFormation();

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

void RiflemanSquad::updateFormation()
{
    for (int i = 0; i < m_entities.size(); i++) 
    {
        m_entities[i].setPosition(m_unitSprite.getPosition() + m_offsets[i]);
        m_entities[i].setRotation(m_unitSprite.getRotation());
    }
}
