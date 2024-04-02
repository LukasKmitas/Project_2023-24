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
    avoidCollisions(allyUnits);
    for (auto& bullet : bullets)
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

    if (isGraduallySlowed) 
    {
        float elapsedTime = slowEffectClock.getElapsedTime().asSeconds() - slowDownStartTime;
        if (elapsedTime < slowEffectDuration)
        {
            // Gradually slow down
            float currentSpeedFactor = 1.0f - ((1.0f - minimumSpeedFactor) * (elapsedTime / slowEffectDuration));
            m_speed = originalSpeed * currentSpeedFactor;
        }
        else if (elapsedTime < slowEffectDuration + postSlowWaitDuration)
        {
            // Keep the unit stopped
            m_speed = originalSpeed * minimumSpeedFactor;
            inPostSlowWait = true;
        }
        else
        {
            // Reset to original speed
            m_speed = originalSpeed;
            isGraduallySlowed = false;
            inPostSlowWait = false;
        }
    }
}

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
  
    for (auto& bullet : bullets) 
    {
        bullet.render(m_window, glowShader);
    }
    for (const auto& missile : missiles)
    {
        missile.render(m_window);
    }
}

void Unit::avoidCollisions(std::vector<Unit*>& allyUnits)
{
    const float minSeparation = 250.0f;
    sf::Vector2f separationForce(0.0f, 0.0f);
    int closeUnits = 0;

    for (auto& unit : allyUnits)
    {
        if (unit == this) continue;

        if ((m_unitType == UnitType::Air && (unit->m_unitType == UnitType::Vehicle || unit->m_unitType == UnitType::Infantry)) ||
            (unit->m_unitType == UnitType::Air && (m_unitType == UnitType::Vehicle || m_unitType == UnitType::Infantry)))
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
        float step = rotationSpeedDegreesPerSecond * t_deltaTime.asSeconds();
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

void Unit::setTargetPosition(const sf::Vector2f& targetPos)
{
    m_targetPosition = targetPos;
    isOrbiting = false;
}

void Unit::takeDamage(float damageAmount)
{
    m_health -= damageAmount;
    if (m_health < 0)
    {
        m_health = 0;
    }
    squadEntityRemoval();
}

void Unit::addHealth(float healthAmount)
{
    m_health += healthAmount;
    if (m_health > m_maxHealth)
    {
        m_health = m_maxHealth;
    }
    squadEntityRegain();
}

void Unit::applySlowEffect(float minSpeedFactor, float duration, float postSlowWait)
{
    isSlowed = true;
    isGraduallySlowed = true;
    minimumSpeedFactor = minSpeedFactor;
    slowEffectDuration = duration;
    postSlowWaitDuration = postSlowWait;
    slowDownStartTime = slowEffectClock.getElapsedTime().asSeconds();
    inPostSlowWait = false;
}

void Unit::setEnemyUnits(std::vector<Unit*>& enemyUnits)
{
    this->enemyUnits = &enemyUnits;
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

void Unit::moveTo(const sf::Vector2f& targetPos)
{
    m_targetPosition = targetPos;
    isOrbiting = false;
}

void Unit::setSelected(bool selected) 
{
    isSelected = selected;
    if (isSelected) 
    {
        m_unitSprite.setColor(sf::Color(100, 255, 100, 255));
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

void Unit::initView()
{
    m_viewCircleShape.setRadius(m_viewRadius);
    m_viewCircleShape.setFillColor(sf::Color(255, 255, 255, 0));
    m_viewCircleShape.setOrigin(m_viewRadius, m_viewRadius);
    m_viewCircleShape.setPosition(m_position);
}

void Unit::initHealthBar()
{
    m_healthBarBackground.setSize(sf::Vector2f(40, 5)); 
    m_healthBarBackground.setFillColor(sf::Color(50, 50, 50, 150));
    m_healthBarBackground.setOrigin(m_healthBarBackground.getSize().x / 2, m_healthBarBackground.getSize().y / 2);

    m_healthBarForeground.setSize(sf::Vector2f(40, 5));
    m_healthBarForeground.setFillColor(sf::Color(0, 255, 0, 200));
    m_healthBarForeground.setOrigin(m_healthBarForeground.getSize().x / 2, m_healthBarForeground.getSize().y / 2);
}

void Unit::initShader()
{
    if (!glowShader.loadFromFile("Assets\\Shaders\\glowShader.frag", sf::Shader::Fragment))
    {
        std::cout << "Error - Loading glow shader" << std::endl;
    }
  
    glowShader.setUniform("glowColor", sf::Glsl::Vec4(1.0, 0.0, 0.0, 1.0));
    glowShader.setUniform("glowRadius", 1.0f);
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

sf::Vector2f Unit::normalize(const sf::Vector2f source)
{
    float length = sqrt((source.x * source.x) + (source.y * source.y));
    if (length != 0)
    {
        return sf::Vector2f(source.x / length, source.y / length);
    }
    else
    {
        return source;
    }
}

sf::Vector2f Unit::steerTowards(sf::Vector2f target)
{
    sf::Vector2f desiredVelocity = target - m_position;
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

sf::Vector2f Unit::rotateVector(sf::Vector2f vector, float angleDegrees)
{
    float rad = angleDegrees * PI / 180.0f;
    return sf::Vector2f
    (
        vector.x * cos(rad) - vector.y * sin(rad),
        vector.x * sin(rad) + vector.y * cos(rad)
    );
}

float Unit::angleFromVector(const sf::Vector2f& vector) 
{
    return std::atan2(vector.y, vector.x) * 180 / PI;
}

float Unit::getViewRadius() const 
{
    return m_viewRadius;
}