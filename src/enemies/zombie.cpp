#include "../../include/enemies/zombie.h"

Zombie::Zombie(float x, float y) : Enemy(x, y, M_ENEMY, M_ENEMY){
    m_health = 10.0f;
    m_speed = 1.0f;
    m_textureID = "zombie";
    m_damage = 15.0f;
}

void Zombie::Update(const Dungeon& dungeon, float playerX, float playerY){
    float velX = 0;
    float velY = 0;

    if(playerX > m_x) velX = m_speed;
    if(playerX < m_x) velX = -m_speed;
    if(playerY > m_y) velY = m_speed;
    if(playerY < m_y) velY = -m_speed;

    MoveWithCollision(velX, velY, dungeon);
}