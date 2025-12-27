#include "../../include/weapons/bow.h"

void Bow::Attack(float x, float y, int dirX, int dirY, std::vector<Projectile>& bulletList){
    if(!CanAttack()) return;

    // Calculate spawn position from player collider center with directional offset
    // Player collider: 32x48 with offset (16, 16) -> center at (x+32, y+40)
    const float playerCenterX = 32.0f;  // x + 16 + (32/2) = x + 32
    const float playerCenterY = 40.0f;  // y + 16 + (48/2) = y + 40
    const float arrowCenterOffset = S_ENEMY / 2.0f;   // 16 pixels to arrow center

    // Start from player collider center
    float centerX = x + playerCenterX;
    float centerY = y + playerCenterY;

    // Offset in the shooting direction to prevent immediate wall collision
    float offsetX = dirX * ARROW_SPAWN_DISTANCE;
    float offsetY = dirY * ARROW_SPAWN_DISTANCE;

    // Convert back to top-left corner coordinates (subtract arrow's center offset)
    float spawnX = centerX + offsetX - arrowCenterOffset;
    float spawnY = centerY + offsetY - arrowCenterOffset;

    Projectile arrow(spawnX, spawnY, dirX, dirY);

    arrow.SetTexture("rock");
    arrow.SetDestroyImpact(true);
    arrow.SetAttached(false, 0, 0);
    bulletList.push_back(arrow);

    ResetTimer();
}