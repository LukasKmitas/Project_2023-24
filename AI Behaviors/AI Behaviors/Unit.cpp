#include "Unit.h"

Unit::Unit()
{
    initView();
    initHealthBar();
    initShader();
}

Unit::~Unit()
{
}

void Unit::update(sf::Time t_deltaTime, std::vector<Unit*>& allyUnits)
{ 
    m_viewCircleShape.setPosition(m_unitSprite.getPosition().x, m_unitSprite.getPosition().y);
    avoidCollisionsWithUnits(allyUnits);
    avoidCollisionsWithWalls();
    for (auto& bullet : m_bullets)
    {
        bullet.update(t_deltaTime);
    }
    /*for (auto& missile : missiles) 
    {
        missile.update(t_deltaTime);
    }*/

    m_particleSystem.update(t_deltaTime);

    m_healthBarBackground.setPosition(m_position.x, m_position.y - 20);
    m_healthBarForeground.setPosition(m_healthBarBackground.getPosition());

    float healthPercentage = m_health / 100;
    m_healthBarForeground.setSize(sf::Vector2f(40 * healthPercentage, 5));

    if (m_isGraduallySlowed) 
    {
        float elapsedTime = m_slowEffectClock.getElapsedTime().asSeconds() - m_slowDownStartTime;
        if (elapsedTime < m_slowEffectDuration)
        {
            // Gradually slow down
            float currentSpeedFactor = 1.0f - ((1.0f - m_minimumSpeedFactor) * (elapsedTime / m_slowEffectDuration));
            m_speed = m_originalSpeed * currentSpeedFactor;
        }
        else if (elapsedTime < m_slowEffectDuration + m_postSlowWaitDuration)
        {
            // Keep the unit stopped
            m_speed = m_originalSpeed * m_minimumSpeedFactor;
            m_inPostSlowWait = true;
        }
        else
        {
            // Reset to original speed
            m_speed = m_originalSpeed;
            m_isGraduallySlowed = false;
            m_inPostSlowWait = false;
        }
    }
}

/// <summary>
/// Renders the units and anything related to it
/// </summary>
/// <param name="m_window"></param>
void Unit::render(sf::RenderWindow& m_window)
{
    m_particleSystem.render(m_window);
    m_window.draw(m_unitSprite);
    m_window.draw(m_weaponSprite);
    m_window.draw(m_viewCircleShape);

    m_window.draw(m_healthBarBackground);
    m_window.draw(m_healthBarForeground);

    sf::VertexArray line(sf::Lines, 2);
    line[0].position = m_unitSprite.getPosition();
    line[1].position = m_targetPosition;
    line[0].color = sf::Color::Red;
    line[1].color = sf::Color::Red;
    m_window.draw(line);
  
    for (auto& bullet : m_bullets) 
    {
        bullet.render(m_window, m_glowShader);
    }
    for (const auto& missile : m_missiles)
    {
        missile.render(m_window);
    }

    for (const auto& ray : m_debugRays)
    {
        sf::VertexArray line(sf::Lines, 2);
        line[0].position = m_position;
        line[1].position = m_position + ray;
        line[0].color = sf::Color::Yellow;
        line[1].color = sf::Color::Red;
        m_window.draw(line);
    }
}

/// <summary>
/// its to avoid colliding with each other
/// </summary>
/// <param name="m_allyUnits"></param>
void Unit::avoidCollisionsWithUnits(std::vector<Unit*>& m_allyUnits)
{
    const float minSeparation = 250.0f;
    sf::Vector2f separationForce(0.0f, 0.0f);
    int closeUnits = 0;

    for (auto& unit : m_allyUnits)
    {
        if (unit == this) continue;

        if ((m_unitTypeClass == UnitTypeClass::Air && (unit->m_unitTypeClass == UnitTypeClass::Vehicle || unit->m_unitTypeClass == UnitTypeClass::Infantry)) ||
            (unit->m_unitTypeClass == UnitTypeClass::Air && (m_unitTypeClass == UnitTypeClass::Vehicle || m_unitTypeClass == UnitTypeClass::Infantry)))
        {
            continue; 
        }

        float dist = distance(this->getPosition(), unit->getPosition());
        if (dist < m_viewRadius - minSeparation)
        {
            sf::Vector2f pushAway = this->getPosition() - unit->getPosition();
            pushAway = normalize(pushAway);
            separationForce += pushAway / dist;
            closeUnits++;
        }
    }

    if (closeUnits > 0) 
    {
        separationForce /= static_cast<float>(closeUnits);
        separationForce = normalize(separationForce);
        m_targetPosition += separationForce;
    }
}

/// <summary>
/// To avoid walls for units
/// </summary>
void Unit::avoidCollisionsWithWalls() 
{
    m_debugRays.clear();

    if (!m_tiles || m_velocity == sf::Vector2f(0, 0))
    {
        return;
    }

    const float m_distanceAhead = 60.0f; // Distance to check ahead for walls
    const float m_distanceSides = 30.0f;  // Distance to check to the sides
    sf::Vector2f m_currentPosition = getPosition();

    std::vector<float> checkAngles = { 0, 30.0f, -30.0f, 60.0f, -60.0f, 90.0f, -90.0f };

    for (float angle : checkAngles) 
    {
        sf::Vector2f testDirection = rotateVector(normalize(m_velocity), angle);
        float checkDistance = angle == 0 ? m_distanceAhead : m_distanceSides;
        sf::Vector2f newPosition = m_currentPosition + testDirection * checkDistance;

        m_debugRays.push_back(testDirection * checkDistance); 

        int newTileX = static_cast<int>(newPosition.x / (*m_tiles)[0][0].m_tileSize);
        int newTileY = static_cast<int>(newPosition.y / (*m_tiles)[0][0].m_tileSize);

        if (newTileX >= 0 && newTileX < (*m_tiles)[0].size() && newTileY >= 0 && newTileY < (*m_tiles).size() && (*m_tiles)[newTileY][newTileX].m_isWall)
        {
            sf::Vector2f avoidanceDirection = findAvoidanceDirection(m_currentPosition, m_distanceAhead);
            if (avoidanceDirection != sf::Vector2f(0, 0)) 
            {
                m_velocity = lerp(m_velocity, normalize(avoidanceDirection) * m_speed, 0.1f); // Apply smoothing
                break;
            }
        }
    }
}

/// <summary>
/// it just orientates the sprite a bit
/// </summary>
/// <param name="t_deltaTime"></param>
void Unit::orientSpriteToMovement(sf::Time t_deltaTime)
{
    if (magnitude(m_velocity) > 0.0f)
    {
        float targetAngleRadians = std::atan2(m_velocity.y, m_velocity.x);
        float targetAngleDegrees = toDegrees(targetAngleRadians) + 90;

        float currentAngleDegrees = fmod(m_unitSprite.getRotation(), 360.0f);
        if (currentAngleDegrees < 0)
        {
            currentAngleDegrees += 360.0f;
        }
        float difference = targetAngleDegrees - currentAngleDegrees;
        if (difference > 180.0f)
        {
            difference -= 360.0f;
        }
        if (difference < -180.0f) 
        {
            difference += 360.0f;
        }

        // Determine rotation speed and apply a fraction of the difference
        float step = m_rotationSpeed * t_deltaTime.asSeconds();
        if (std::abs(difference) < step)
        {
            step = std::abs(difference); // Clamp to avoid overshooting
        }

        m_unitSprite.setRotation(currentAngleDegrees + (difference > 0 ? step : -step));
    }
}

void Unit::squadEntityRemoval()
{
}

void Unit::squadEntityRegain()
{
}

/// <summary>
/// sets target position for the start of being created
/// </summary>
/// <param name="m_targetPos"></param>
void Unit::setTargetPosition(const sf::Vector2f& m_targetPos)
{
    m_targetPosition = m_targetPos;
    isOrbiting = false;
}

/// <summary>
/// this is how it takes damage takes away health
/// </summary>
/// <param name="m_damageAmount"></param>
void Unit::takeDamage(float m_damageAmount)
{
    m_health -= m_damageAmount;
    if (m_health < 0)
    {
        m_health = 0;
    }
    squadEntityRemoval();
}

/// <summary>
/// Adds health to the unit
/// </summary>
/// <param name="m_healthAmount"></param>
void Unit::addHealth(float m_healthAmount)
{
    m_health += m_healthAmount;
    if (m_health > m_maxHealth)
    {
        m_health = m_maxHealth;
    }
    squadEntityRegain();
}

/// <summary>
/// This slows down the enemy slowly to a stop
/// </summary>
/// <param name="m_speedFactor"></param>
/// <param name="m_duration"></param>
/// <param name="m_postSlowWait"></param>
void Unit::applySlowEffect(float m_speedFactor, float m_duration, float m_postSlowWait)
{
    m_isSlowed = true;
    m_isGraduallySlowed = true;
    m_minimumSpeedFactor = m_speedFactor;
    m_slowEffectDuration = m_duration;
    m_postSlowWaitDuration = m_postSlowWait;
    m_slowDownStartTime = m_slowEffectClock.getElapsedTime().asSeconds();
    m_inPostSlowWait = false;
}

/// <summary>
/// sets the enemies 
/// </summary>
/// <param name="m_enemyUnits"></param>
void Unit::setEnemyUnits(std::vector<Unit*>& m_enemyUnits)
{
    this->m_enemyUnits = &m_enemyUnits;
}

/// <summary>
/// sets the buildings
/// </summary>
/// <param name="m_enemyBuildings"></param>
void Unit::setEnemyBuildings(std::vector<Building*>& m_enemyBuildings)
{
    this->m_enemyBuildings = &m_enemyBuildings;
}

/// <summary>
/// Sets the tiles
/// </summary>
/// <param name="m_tiles"></param>
void Unit::setTiles(const std::vector<std::vector<Tile>>& m_tiles)
{
    this->m_tiles = &m_tiles;
}

const sf::Sprite& Unit::getSprite() const
{
    return m_unitSprite;
}

void Unit::setPosition(const sf::Vector2f& position) 
{
    m_position = position;
    m_unitSprite.setPosition(position);
}

/// <summary>
/// Stes health for laoding
/// </summary>
/// <param name="m_setHealth"></param>
void Unit::setHealth(float m_setHealth)
{
    m_health = m_setHealth;
}

/// <summary>
/// Gives the target position
/// </summary>
/// <param name="m_targetPos"></param>
void Unit::moveTo(const sf::Vector2f& m_targetPos)
{
    m_targetPosition = m_targetPos;
    isOrbiting = false;
}

/// <summary>
/// Checks if its selected
/// </summary>
/// <param name="m_selected"></param>
void Unit::setSelected(bool m_selected)
{
    isSelected = m_selected;
    if (isSelected) 
    {
        m_unitSprite.setColor(sf::Color(120, 255, 120, 255));
    }
    else 
    {
        m_unitSprite.setColor(sf::Color(255, 255, 255, 255));
    }
}

float Unit::distance(const sf::Vector2f& a, const sf::Vector2f& b) 
{
    sf::Vector2f diff = a - b;
    return std::sqrt(diff.x * diff.x + diff.y * diff.y);
}

float Unit::magnitude(const sf::Vector2f& v) const
{
    return std::sqrt(v.x * v.x + v.y * v.y);
}

float Unit::getHealth() const
{
    return m_health;
}

float Unit::toDegrees(float radians)
{
    return radians * 180.0f / PI;
}

float Unit::angleBetweenVectors(sf::Vector2f vec1, sf::Vector2f vec2)
{
    float dot = vec1.x * vec2.x + vec1.y * vec2.y;
    float det = vec1.x * vec2.y - vec1.y * vec2.x;
    float angle = atan2(det, dot) * (180 / PI);
    return angle;
}

float Unit::getDamage() const
{
    return m_damage;
}

/// <summary>
/// initializes view
/// </summary>
void Unit::initView()
{
    m_viewCircleShape.setRadius(m_viewRadius);
    m_viewCircleShape.setFillColor(sf::Color(255, 255, 255, 0));
    m_viewCircleShape.setOrigin(m_viewRadius, m_viewRadius);
    m_viewCircleShape.setPosition(m_position);
}

/// <summary>
/// initializes health bar
/// </summary>
void Unit::initHealthBar()
{
    m_healthBarBackground.setSize(sf::Vector2f(40, 5)); 
    m_healthBarBackground.setFillColor(sf::Color(50, 50, 50, 150));
    m_healthBarBackground.setOrigin(m_healthBarBackground.getSize().x / 2, m_healthBarBackground.getSize().y / 2);

    m_healthBarForeground.setSize(sf::Vector2f(40, 5));
    m_healthBarForeground.setFillColor(sf::Color(0, 255, 0, 200));
    m_healthBarForeground.setOrigin(m_healthBarForeground.getSize().x / 2, m_healthBarForeground.getSize().y / 2);
}

/// <summary>
/// initializes shaders
/// </summary>
void Unit::initShader()
{
    if (!m_glowShader.loadFromFile("Assets\\Shaders\\glowShader.frag", sf::Shader::Fragment))
    {
        std::cout << "Error - Loading glow shader" << std::endl;
    }
  
    m_glowShader.setUniform("glowColor", sf::Glsl::Vec4(1.0, 0.0, 0.0, 1.0));
    m_glowShader.setUniform("glowRadius", 1.0f);
}

bool Unit::checkAffordability()
{
    if (!m_isEnemy)
    {
        if (Global::playerCurrency >= m_cost)
        {
            Global::playerCurrency -= m_cost;
            std::cout << m_cost << std::endl;
            return true;
        }
        else
        {
            std::cout << "Not enough currency to create unit." << std::endl;
            return false;
        }
    }
    else
    {
        if (Global::enemyCurrency >= m_cost)
        {
            Global::enemyCurrency -= m_cost;
            return true;
        }
    }
}

bool Unit::isActive() const
{
    return m_health > 0;
}

sf::Vector2f Unit::getPosition() const 
{
    return m_unitSprite.getPosition();
}

sf::Vector2f Unit::getTargetPosition() const
{
    return m_targetPosition;
}

sf::Vector2f Unit::normalize(const sf::Vector2f m_source)
{
    float length = sqrt((m_source.x * m_source.x) + (m_source.y * m_source.y));
    if (length != 0)
    {
        return sf::Vector2f(m_source.x / length, m_source.y / length);
    }
    else
    {
        return m_source;
    }
}

/// <summary>
/// Makes it slowly steer towards that direction
/// </summary>
/// <param name="m_target"></param>
/// <returns></returns>
sf::Vector2f Unit::steerTowards(sf::Vector2f m_target)
{
    sf::Vector2f desiredVelocity = m_target - m_position;
    float distance = magnitude(desiredVelocity);

    if (distance > 0.1f)
    {
        desiredVelocity = normalize(desiredVelocity);
    }

    if (distance < m_slowingRadius)
    {
        desiredVelocity *= m_speed * (distance / m_slowingRadius);
    }
    else
    {
        desiredVelocity *= m_speed;
    }

    sf::Vector2f steer = desiredVelocity - m_velocity;

    if (magnitude(steer) > m_maxForce)
    {
        steer = normalize(steer);
        steer *= m_maxForce;
    }

    return steer;
}

sf::Vector2f Unit::rotateVector(sf::Vector2f m_vector, float m_angleDegrees)
{
    float rad = m_angleDegrees * PI / 180.0f;
    return sf::Vector2f
    (
        m_vector.x * cos(rad) - m_vector.y * sin(rad),
        m_vector.x * sin(rad) + m_vector.y * cos(rad)
    );
}

float Unit::angleFromVector(const sf::Vector2f& m_vector)
{
    return std::atan2(m_vector.y, m_vector.x) * 180 / PI;
}

float Unit::getViewRadius() const 
{
    return m_viewRadius;
}

sf::Vector2f Unit::lerp(const sf::Vector2f& m_start, const sf::Vector2f& m_end, float m_time) 
{
    return m_start + (m_end - m_start) * m_time;
}

/// <summary>
/// This looks for a suitable direction to avoid walls when a direct path is blocked 
/// It tries various angles to find a clear path
/// </summary>
/// <param name="m_currentPosition"></param>
/// <param name="m_checkAheadDistance"></param>
/// <returns></returns>
sf::Vector2f Unit::findAvoidanceDirection(const sf::Vector2f& m_currentPosition, float m_checkAheadDistance)
{
    for (float angle : {30.0f, -30.0f, 60.0f, -60.0f, 120.0f, -120.0f}) 
    {
        sf::Vector2f testDirection = rotateVector(normalize(m_velocity), angle);
        sf::Vector2f newPosition = m_currentPosition + testDirection * m_checkAheadDistance;

        int newTileX = static_cast<int>(newPosition.x / (*m_tiles)[0][0].m_tileSize);
        int newTileY = static_cast<int>(newPosition.y / (*m_tiles)[0][0].m_tileSize);

        if (newTileX >= 0 && newTileX < (*m_tiles)[0].size() && newTileY >= 0 && newTileY < (*m_tiles).size() && !(*m_tiles)[newTileY][newTileX].m_isWall)
        {
            return testDirection;
        }
    }
    return sf::Vector2f(0, 0);
}