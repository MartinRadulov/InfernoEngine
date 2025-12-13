#include "../../include/weapons/sword.h"

void Sword::Attack(float x, float y, int dirX, int dirY, std::vector<Projectile>& bulletList){
    if(!CanAttack()) return;

    float offsetX = dirX * 20;
    float offsetY = dirY * 20;

    Projectile slash(x + offsetX, y + offsetY, 0, 0);

    slash.SetDimensions(SWORD_SIZE, SWORD_SIZE);
    slash.SetLifeTime(SWORD_LIFE);
    slash.SetTexture("rock");
    slash.SetDestroyImpact(false);
    slash.SetAttached(true, offsetX, offsetY);

    bulletList.push_back(slash);

    ResetTimer();
}