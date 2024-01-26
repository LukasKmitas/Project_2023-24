#pragma once
#include "Unit.h"

class Vehicle : public Unit
{
public:
    Vehicle();
    virtual ~Vehicle();

    virtual void attack(Unit* target) override;
};
