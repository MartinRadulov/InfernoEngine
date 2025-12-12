#include "../../include/enemies/zombie.h"

Zombie::Zombie(float x, float y) : Enemy(x, y, M_ENEMY, M_ENEMY){
    m_health = 10.0f;
    m_speed = 1.0f;
    m_textureID = "zombie";
    m_damage = 15.0f;
}

void Zombie::Update(Level& level, float playerX, float playerY){
    float nextX = m_x;
    float nextY = m_y;

    if(playerX > m_x) nextX += m_speed;
    if(playerX < m_x) nextX -= m_speed;
    if(playerY > m_y) nextY += m_speed;
    if(playerY < m_y) nextY -= m_speed;

    if(!CheckCollision(nextX, m_y, level)) m_x = nextX;
    if(!CheckCollision(m_x, nextY, level)) m_y = nextY;
}