#include "Firehawk.h"

Firehawk::Firehawk()
{
	setupFirehawk();
	m_cost = 1500;
	m_speed = 130;
	m_health = 160;
	m_orbitRadius = 50.0f;
	m_orbitSpeed = 80.0f;
	m_viewRadius = 500;
}

Firehawk::~Firehawk()
{
}

void Firehawk::update(sf::Time t_deltaTime, std::vector<Unit*>& allUnits)
{
	if (!isOrbiting) 
	{
		approachTarget(m_targetPosition, t_deltaTime);
	}
	else
	{
		orbitTarget(t_deltaTime);
	}
	m_unitSprite.setPosition(m_position);

	if (reloadCooldown > 0.0f)
	{
		reloadCooldown -= t_deltaTime.asSeconds();
		if (reloadCooldown <= 0.0f) 
		{
			missilesFiredInBurst = 0;
		}
	}

	// Check for enemy units in range and if directly aligned and then fires
	for (auto& enemyUnit : allUnits) 
	{
		float distanceToEnemy = magnitude(enemyUnit->getPosition() - m_position);
		if (distanceToEnemy <= m_viewRadius)
		{
			// Check if Firehawk is facing the enemy directly
			sf::Vector2f directionToEnemy = normalize(enemyUnit->getPosition() - m_position);
			float angleToEnemy = angleFromVector(directionToEnemy);
			float firehawkAngle = m_unitSprite.getRotation() - 90;

			if (std::abs(firehawkAngle - angleToEnemy) < 10.0f)
			{ 
				fireMissileAtTarget(enemyUnit->getPosition());
				break;
			}
		}
	}
	for (auto& missile : missiles)
	{
		missile.update(t_deltaTime);
	}
}

void Firehawk::approachTarget(const sf::Vector2f& targetPos, sf::Time t_deltaTime)
{
	sf::Vector2f direction = targetPos - m_position;
	float distanceToTarget = magnitude(direction);

	if (distanceToTarget < m_orbitRadius)
	{
		isOrbiting = true;
	}
	else
	{
		direction = normalize(direction);
		m_velocity = direction * m_speed;
		m_position += m_velocity * t_deltaTime.asSeconds();

		float desiredAngle = angleFromVector(direction); 
		float currentAngle = m_unitSprite.getRotation() - 90;

		float angleDiff = fmod(desiredAngle - currentAngle + 540, 360) - 180;
		float newAngle = currentAngle + std::min(std::max(angleDiff, -m_rotationSpeed * t_deltaTime.asSeconds()), m_rotationSpeed * t_deltaTime.asSeconds());
		m_unitSprite.setRotation(newAngle + 90);
	}
}

void Firehawk::orbitTarget(sf::Time t_deltaTime) 
{
	m_currentOrbitAngle += m_orbitSpeed * t_deltaTime.asSeconds();
	if (m_currentOrbitAngle >= 360.f)
	{
		m_currentOrbitAngle -= 360.f;
	}

	m_position.x = m_targetPosition.x + m_orbitRadius * cos(m_currentOrbitAngle * (PI / 180.f));
	m_position.y = m_targetPosition.y + m_orbitRadius * sin(m_currentOrbitAngle * (PI / 180.f));

	sf::Vector2f direction = m_position - m_targetPosition;
	float desiredAngle = atan2(direction.y, direction.x) * (180 / PI) + 90;
    float currentAngle = m_unitSprite.getRotation() - 90; 

    float angleDiff = fmod(desiredAngle - currentAngle + 540, 360) - 180;
    float newAngle = currentAngle + std::min(std::max(angleDiff, -m_rotationSpeed * t_deltaTime.asSeconds()), m_rotationSpeed * t_deltaTime.asSeconds());
    m_unitSprite.setRotation(newAngle + 90); 
}

void Firehawk::setupFirehawk()
{
	if (!m_unitTexture.loadFromFile("Assets\\Images\\Units\\Firehawk.png"))
	{
		std::cout << "Error - Loading Firehawk Texture" << std::endl;
	}
	m_unitSprite.setTexture(m_unitTexture);
	m_unitSprite.setPosition(m_position);
	m_unitSprite.setOrigin(m_unitSprite.getLocalBounds().width / 2, m_unitSprite.getLocalBounds().height / 2);
	m_unitSprite.setScale(0.1, 0.1);

	if (!missileTexture.loadFromFile("Assets/Images/Units/Missile.png"))
	{
		std::cout << "Error - Loading Missile Texture" << std::endl;
	}
}

void Firehawk::fireMissileAtTarget(const sf::Vector2f& targetPos)
{
	if (reloadCooldown <= 0.0f && missilesFiredInBurst < burstSize)
	{
		float spray = (std::rand() % 10 - 5) * (3.14159265 / 180.0f);
		sf::Vector2f direction = normalize(targetPos - m_position);
		float angle = atan2(direction.y, direction.x) + spray;
		sf::Vector2f missileDirection = sf::Vector2f(cos(angle), sin(angle));

		missiles.emplace_back(m_position, missileDirection, 200.0f, missileTexture);
		missilesFiredInBurst++;

		if (missilesFiredInBurst >= burstSize)
		{
			reloadCooldown = reloadCooldownTime;
		}
	}
}
