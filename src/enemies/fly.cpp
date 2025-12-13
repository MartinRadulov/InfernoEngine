#include "../../include/enemies/fly.h"

Fly::Fly(float x, float y) : Enemy(x, y, S_ENEMY, S_ENEMY){
    m_health = 1.0f;
    m_speed = 1.5f;
    m_textureID = "fly";
    m_damage = 15.0f;
}

void Fly::Update(Level& level, float playerX, float playerY){
    float velX = 0;
    float velY = 0;

    if(playerX > m_x) velX = m_speed;
    else velX = -m_speed;

    if(playerY > m_y) velY = m_speed;
    else velY = -m_speed;

    MoveFlying(velX, velY);
}