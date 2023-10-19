#include "SideBar.h"

SideBar::SideBar()
{
    m_background.setSize(sf::Vector2f(200, 80));
    m_background.setFillColor(sf::Color(128, 128, 128));

    m_background.setPosition(0, 0);

    m_icons.clear();
}

void SideBar::render(sf::RenderWindow& window)
{
    window.draw(m_background);

    for (const auto& icon : m_icons) 
    {
        window.draw(icon);
    }
}

void SideBar::setPosition(float x, float y)
{
    m_background.setPosition(x, y);
}

void SideBar::addIcon(const sf::Texture& texture, const sf::Vector2f& position)
{
    sf::Sprite icon;
    icon.setTexture(texture);
    icon.setPosition(position);
    m_icons.push_back(icon);
}

int SideBar::handleMouseClick(const sf::Vector2f& mousePosition)
{
    for (int i = 0; i < m_icons.size(); ++i) {
        if (m_icons[i].getGlobalBounds().contains(mousePosition)) 
        {
            std::cout << "This Icon selected " << i << std::endl;
            return i;
        }
    }
    return -1;
}
