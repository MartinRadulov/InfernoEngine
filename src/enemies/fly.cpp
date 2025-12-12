#include "../../include/enemies/fly.h"

Fly::Fly(float x, float y) : Enemy(x, y, S_ENEMY, S_ENEMY){
    m_health = 1.0f;
    m_speed = 2.0f;
    m_textureID = "fly";
}

void Fly::Update(Level& level, float playerX, float playerY){
    if(playerX > m_x) m_x += m_speed;
    if(playerX < m_x) m_x -= m_speed;
    if(playerY > m_y) m_y += m_speed;
    if(playerY < m_y) m_y -= m_speed;
}