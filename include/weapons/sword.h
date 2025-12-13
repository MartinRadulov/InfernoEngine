#pragma once
#include "../weapon.h"

class Sword : public Weapon{
public:
    Sword() : Weapon(250, 0.33f) {}

    void Attack(float x, float y, int dirX, int dirY, std::vector<Projectile>& bulletList) override;
};