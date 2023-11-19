#pragma once
#include "Building.h"

class Headquarters : public Building
{
public:
    Headquarters();
    ~Headquarters();

    void update(sf::Time deltaTime) override;
    void render(sf::RenderWindow& m_window) const override;

private:
    
    void setupSprite();
    

};