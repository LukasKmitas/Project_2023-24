#pragma once
#include "Unit.h"

class Infantry : public Unit
{
public:
    Infantry();
    virtual ~Infantry();

    virtual void attack(Unit* target) override;
};
