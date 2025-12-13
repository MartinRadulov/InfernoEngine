#include "../include/enemy.h"
#include <iostream> // For debugging if needed

Enemy::Enemy(float x, float y, int width, int height)
    : m_x(x), m_y(y), m_width(width), m_height(height), 
    m_collider(GetWidth(), GetHeight(), 0, 0) {}

void Enemy::Render(std::vector<RenderObject>& renderList) {
    RenderObject eObj;
    eObj.textureID = m_textureID; // Use my internal ID (Zombie/Fly)
    eObj.srcRect = {0, 0, SPRITE_SHEET_SIZE, SPRITE_SHEET_SIZE}; 
    eObj.destRect = { (int)m_x, (int)m_y, m_width, m_height };
    eObj.sortY = m_y + m_height;
    eObj.flip = SDL_FLIP_NONE;

    renderList.push_back(eObj);
}

void Enemy::TakeDamage(float dmgNum){
    m_health -= dmgNum;
    if(m_health <= 0){
        m_health = 0;
        m_isActive = false;
    }
}

void Enemy::MoveWithCollision(float velX, float velY, Level& level){
    float nextX = m_x + velX;
    float nextY = m_y + velY;

    m_collider.SetPosition(nextX, m_y);
    if(!m_collider.CheckMapCollision(level)){
        m_x = nextX;
    }
    m_collider.SetPosition(m_x, nextY);
    if(!m_collider.CheckMapCollision(level)){
        m_y = nextY;
    }

    m_collider.SetPosition(m_x, m_y);
}
void Enemy::MoveFlying(float velX, float velY){
    m_x += velX;
    m_y += velY;
    m_collider.SetPosition(m_x, m_y);
}