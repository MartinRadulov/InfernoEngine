#include "../../include/weapons/sword.h"

void Sword::Attack(float x, float y, int dirX, int dirY, std::vector<Projectile>& bulletList){
    if(!CanAttack()) return;

    // Calculate spawn position from player collider center
    // Player collider: 32x48 with offset (16, 16) -> center at (x+32, y+40)
    const float playerCenterX = 32.0f;  // x + 16 + (32/2) = x + 32
    const float playerCenterY = 40.0f;  // y + 16 + (48/2) = y + 40
    const float swordCenterOffset = SWORD_SIZE / 2.0f;  // 24 pixels to sword center

    // Start from player collider center
    float centerX = x + playerCenterX;
    float centerY = y + playerCenterY;

    // Use direction-specific spawn distance (player collider is asymmetric: 32w x 48h)
    float spawnDistanceX = (dirX != 0) ? SWORD_SPAWN_DISTANCE_HORIZONTAL : 0;
    float spawnDistanceY = (dirY != 0) ? SWORD_SPAWN_DISTANCE_VERTICAL : 0;

    // Offset in the attack direction from player center
    float offsetX = dirX * spawnDistanceX;
    float offsetY = dirY * spawnDistanceY;

    // Convert back to top-left corner coordinates (subtract sword's center offset)
    float spawnX = centerX + offsetX - swordCenterOffset;
    float spawnY = centerY + offsetY - swordCenterOffset;

    Projectile slash(spawnX, spawnY, 0, 0);

    slash.SetDimensions(SWORD_SIZE, SWORD_SIZE);
    slash.SetLifeTime(SWORD_LIFE);
    slash.SetTexture("rock");
    slash.SetDestroyImpact(false);

    // Attachment offset must be relative to player top-left (ownerX/ownerY in Projectile::Update)
    // offsetX/offsetY are currently relative to player center, so we need to convert:
    // - Add playerCenterX/Y to convert from "offset from top-left" to "offset from center"
    // - Subtract swordCenterOffset to account for sword being positioned by its top-left
    slash.SetAttached(true,
                     offsetX + playerCenterX - swordCenterOffset,
                     offsetY + playerCenterY - swordCenterOffset);

    bulletList.push_back(slash);

    ResetTimer();
}