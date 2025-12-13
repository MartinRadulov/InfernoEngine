#include "../../include/weapons/bow.h"

void Bow::Attack(float x, float y, int dirX, int dirY, std::vector<Projectile>& bulletList){
    if(!CanAttack()) return;

    Projectile arrow(x, y, dirX, dirY);

    arrow.SetTexture("rock");
    arrow.SetDestroyImpact(true);
    arrow.SetAttached(false, 0, 0);
    bulletList.push_back(arrow);

    ResetTimer();
}