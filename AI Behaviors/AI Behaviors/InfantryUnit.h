#pragma once
#include "Unit.h"

class InfantryUnit : public Unit
{
public:
    InfantryUnit();
    ~InfantryUnit();

    void update(sf::Time t_deltaTime, std::vector<Unit*>& m_allyUnits) override;
    void render(sf::RenderWindow& m_window) override;

};
