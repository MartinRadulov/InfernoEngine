#include "../../include/enemies/stalker.h"
#include <cmath>
#include <cstdlib>

Stalker::Stalker(float x, float y) : Enemy(x, y, M_ENEMY, M_ENEMY){
    m_health = 10.0f;
    m_speed = 1.0f;
    m_textureID = "zombie";
    m_currentState = EnemyState::WANDERING;
    m_moveTimer = 0;
    m_dirX = 0;
    m_dirY = 0;
    m_damage = 15.0f;
}

void Stalker::Update(Level& level, float playerX, float playerY){
    float distanceX = std::abs(playerX - m_x);
    float distanceY = std::abs(playerY - m_y);
    if(distanceX > 300 || distanceY > 300){
        m_currentState = EnemyState::WANDERING;
    }

    switch(m_currentState){
        case EnemyState::WANDERING:
            if(!HasLineOfSight(level, playerX, playerY)){
                Wander(level);
            }
            else{
               m_currentState = EnemyState::CHASING;
            }
            break;

        case EnemyState::CHASING:
            Chase(level, playerX, playerY);
            break;
    }
}

void Stalker::Chase(Level& level, float playerX, float playerY){
    float nextX = m_x;
    float nextY = m_y;

    if(playerX > m_x) nextX += m_speed;
    if(playerX < m_x) nextX -= m_speed;
    if(playerY > m_y) nextY += m_speed;
    if(playerY < m_y) nextY -= m_speed;

    if(!CheckCollision(nextX, m_y, level)) m_x = nextX;
    if(!CheckCollision(m_x, nextY, level)) m_y = nextY;
}

void Stalker::Wander(Level& level){
    if(m_moveTimer > 0) m_moveTimer--;

    if(m_moveTimer <= 0){
        m_moveTimer = 60 + (std::rand() % 120);

        m_dirX = (std::rand() % 3) - 1;
        m_dirY = (std::rand() % 3) - 1;
    }

    float wanderSpeed = m_speed * 0.5f;

    float nextX = m_x + (m_dirX * wanderSpeed);
    float nextY = m_y + (m_dirY * wanderSpeed);

    if(!CheckCollision(nextX, m_y, level)) m_x = nextX;
    if(!CheckCollision(m_x, nextY, level)) m_y = nextY;
}

bool Stalker::HasLineOfSight(Level& level, float playerX, float playerY){
    float dx = playerX - m_x;
    float dy = playerY - m_y;
    float dist = std::sqrt(dx*dx + dy*dy);

    if(dist > 300) return false;

    float stepX = dx / dist;
    float stepY = dy / dist;

    float currX = m_x;
    float currY = m_y;

    for(float t = 0; t < dist; t += TILE_SIZE / 2){
        currX += stepX * (TILE_SIZE/2);
        currY += stepY * (TILE_SIZE/2);

        int col = PixelToGrid(currX);
        int row = PixelToGrid(currY);

        if(level.GetTile(row, col) == 1){
            return false;
        }
    }

    return true;
}