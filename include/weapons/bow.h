#pragma once
#include "../weapon.h"

class Bow : public Weapon{
public:
    Bow() : Weapon(500, 2.0f) {}

    void Attack(float x, float y, int dirX, int dirY, std::vector<Projectile>& bulletList) override;
};