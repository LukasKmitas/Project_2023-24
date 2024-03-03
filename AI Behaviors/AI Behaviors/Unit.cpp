#include "Unit.h"

Unit::Unit()
{
    initView();
    initShader();
}

Unit::~Unit()
{
}

void Unit::update(sf::Time t_deltaTime, std::vector<Unit*>& allUnits)
{ 
    m_viewCircleShape.setPosition(m_unitSprite.getPosition().x, m_unitSprite.getPosition().y);
    avoidCollisions(allUnits);
    for (auto& bullet : bullets)
    {
        bullet.update(t_deltaTime);
    }
    m_particleSystem.update(t_deltaTime);
}

void Unit::render(sf::RenderWindow& m_window)
{
    m_particleSystem.render(m_window);
    m_window.draw(m_unitSprite);
    m_window.draw(m_weaponSprite);
    m_window.draw(m_viewCircleShape);
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
}

void Unit::attack(Unit* target)
{
}

void Unit::avoidCollisions(std::vector<Unit*>& allUnits)
{
    const float minSeparation = 250.0f;
    sf::Vector2f separationForce(0.0f, 0.0f);
    int closeUnits = 0;

    for (auto& other : allUnits)
    {
        if (other == this) continue;

        if ((m_unitType == UnitType::Air && (other->m_unitType == UnitType::Vehicle || other->m_unitType == UnitType::Infantry)) ||
            (other->m_unitType == UnitType::Air && (m_unitType == UnitType::Vehicle || m_unitType == UnitType::Infantry)))
        {
            continue; 
        }

        float dist = distance(this->getPosition(), other->getPosition());
        if (dist < m_viewRadius - minSeparation)
        {
            sf::Vector2f pushAway = this->getPosition() - other->getPosition();
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

float Unit::magnitude(const sf::Vector2f& v) 
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

void Unit::initView()
{
    m_viewCircleShape.setRadius(m_viewRadius);
    m_viewCircleShape.setFillColor(sf::Color(255, 255, 255, 0));
    m_viewCircleShape.setOrigin(m_viewRadius, m_viewRadius);
    m_viewCircleShape.setPosition(m_position);
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
    if (Global::currency >= m_cost)
    {
        Global::currency -= m_cost;
        std::cout << m_cost << std::endl;
        return true;
    }
    else
    {
        std::cout << "Not enough currency to create unit." << std::endl;
        return false;
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

float Unit::angleFromVector(const sf::Vector2f& vector) 
{
    return std::atan2(vector.y, vector.x) * 180 / PI;
}

float Unit::getViewRadius() const 
{
    return m_viewRadius;
}