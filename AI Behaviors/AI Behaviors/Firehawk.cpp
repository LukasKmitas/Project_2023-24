#include "Firehawk.h"

Firehawk::Firehawk()
{
	initFirehawk();
	m_cost = 1500;
	m_speed = 130;
	m_health = 160;
	m_orbitRadius = 50.0f;
	m_orbitSpeed = 80.0f;
	m_viewRadius = 500;
	m_damage = 40;
}

Firehawk::~Firehawk()
{
}

void Firehawk::update(sf::Time t_deltaTime, std::vector<Unit*>& m_allyUnits)
{
	AircraftUnit::update(t_deltaTime, m_allyUnits);

	if (!isOrbiting) 
	{
		approachTarget(m_targetPosition, t_deltaTime);
	}
	else
	{
		orbitTarget(t_deltaTime);
	}
	m_unitSprite.setPosition(m_position);

	if (m_phase == 1 && m_phaseTimer > 0.0f)
	{
		m_phaseTimer -= t_deltaTime.asSeconds();
	}

	if (m_reloadCooldown > 0.0f)
	{
		m_reloadCooldown -= t_deltaTime.asSeconds();
		if (m_reloadCooldown <= 0.0f) 
		{
			m_missilesFiredInBurst = 0;
		}
	}

	bool targetFound = false;
	for (auto& enemyUnit : *this->m_enemyUnits)
	{
		if (isTargetWithinRange(enemyUnit->getPosition()))
		{
			fireMissileAtTarget(enemyUnit->getPosition());
			targetFound = true;
			break;
		}
	}

	if (!targetFound && this->m_enemyBuildings)
	{
		for (auto& building : *this->m_enemyBuildings)
		{
			if (isTargetWithinRange(building->getPosition()))
			{
				fireMissileAtTarget(building->getPosition());
				break;
			}
		}
	}

	for (auto& missile : m_missiles)
	{
		missile.update(t_deltaTime);
	}
}

/// <summary>
/// makes the unit move 
/// </summary>
/// <param name="m_targetPos"></param>
/// <param name="t_deltaTime"></param>
void Firehawk::approachTarget(const sf::Vector2f& m_targetPos, sf::Time t_deltaTime)
{
	sf::Vector2f direction = m_targetPos - m_position;
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

/// <summary>
/// Makes the unit orbit around an area
/// </summary>
/// <param name="t_deltaTime"></param>
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

/// <summary>
/// initializes firehawk unit
/// </summary>
void Firehawk::initFirehawk()
{
	if (!m_unitTexture.loadFromFile("Assets\\Images\\Units\\Firehawk.png"))
	{
		std::cout << "Error - Loading Firehawk Texture" << std::endl;
	}
	m_unitSprite.setTexture(m_unitTexture);
	m_unitSprite.setPosition(m_position);
	m_unitSprite.setOrigin(m_unitSprite.getLocalBounds().width / 2, m_unitSprite.getLocalBounds().height / 2);
	m_unitSprite.setScale(0.1, 0.1);

	if (!m_missileTexture.loadFromFile("Assets/Images/Units/Missile.png"))
	{
		std::cout << "Error - Loading Missile Texture" << std::endl;
	}
}

/// <summary>
/// shoots missles in a 2 phases 2 rockets and then 2 more after a while
/// </summary>
/// <param name="m_targetPos"></param>
void Firehawk::fireMissileAtTarget(const sf::Vector2f& m_targetPos)
{
	if (m_reloadCooldown <= 0.0f && m_missilesFiredInBurst < m_burstSize)
	{
		if (m_missilesFiredInBurst == 0)
		{
			m_phase = 0;
			m_phaseTimer = m_phaseDelay;
		}

		// First phase of firing
		if (m_phase == 0 && m_missilesFiredInBurst < 2)
		{
			launchMissile(m_targetPos);
			if (m_missilesFiredInBurst == 2)
			{
				m_phase = 1;
			}
		}
		// Second phase after the delay
		else if (m_phase == 1 && m_phaseTimer <= 0.0f)
		{
			launchMissile(m_targetPos);
		}

		if (m_missilesFiredInBurst >= m_burstSize)
		{
			m_reloadCooldown = m_reloadCooldownTime;
			m_missilesFiredInBurst = 0;
		}
	}
}

/// <summary>
/// The target needs to be in line of sight to be able to shoot
/// </summary>
/// <param name="m_targetPos"></param>
/// <returns></returns>
bool Firehawk::isTargetWithinRange(const sf::Vector2f& m_targetPos)
{
	float distanceToTarget = magnitude(m_targetPos - m_position);

	if (distanceToTarget <= m_viewRadius - 10)
	{
		sf::Vector2f directionToTarget = normalize(m_targetPos - m_position);
		float angleToTarget = angleFromVector(directionToTarget);
		float firehawkAngle = m_unitSprite.getRotation() - 90;

		if (std::abs(firehawkAngle - angleToTarget) < 10.0f)
		{
			return true;
		}
	}
	return false;
}

/// <summary>
/// shoots out the missles
/// </summary>
/// <param name="targetPos"></param>
void Firehawk::launchMissile(const sf::Vector2f& m_targetPos)
{
	float spray = (std::rand() % 10 - 5) * (PI / 180.0f);
	sf::Vector2f direction = normalize(m_targetPos - m_position);
	float angle = atan2(direction.y, direction.x) + spray;
	sf::Vector2f missileDirection = sf::Vector2f(cos(angle), sin(angle));

	m_missiles.emplace_back(m_position, missileDirection, 200.0f, m_missileTexture);
	SoundManager::getInstance().playSound("RocketFired");
	m_missilesFiredInBurst++;
}
