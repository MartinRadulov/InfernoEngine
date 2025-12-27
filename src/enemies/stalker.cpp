#include "../../include/enemies/stalker.h"
#include "../../include/dungeon.h"
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

void Stalker::Update(const Dungeon& dungeon, float playerX, float playerY){
    float distanceX = std::abs(playerX - m_x);
    float distanceY = std::abs(playerY - m_y);
    if(distanceX > 300 || distanceY > 300){
        m_currentState = EnemyState::WANDERING;
    }

    switch(m_currentState){
        case EnemyState::WANDERING:
            if(!HasLineOfSight(dungeon, playerX, playerY)){
                Wander(dungeon);
            }
            else{
               m_currentState = EnemyState::CHASING;
            }
            break;

        case EnemyState::CHASING:
            Chase(dungeon, playerX, playerY);
            break;
    }
}

void Stalker::Chase(const Dungeon& dungeon, float playerX, float playerY){
    float velX = 0;
    float velY = 0;

    if(playerX > m_x) velX = m_speed;
    if(playerX < m_x) velX = -m_speed;
    if(playerY > m_y) velY = m_speed;
    if(playerY < m_y) velY = -m_speed;

    MoveWithCollision(velX, velY, dungeon);
}

void Stalker::Wander(const Dungeon& dungeon){
    if(m_moveTimer > 0) m_moveTimer--;

    if(m_moveTimer <= 0){
        m_moveTimer = 60 + (std::rand() % 120);

        m_dirX = (std::rand() % 3) - 1;
        m_dirY = (std::rand() % 3) - 1;
    }

    float wanderSpeed = m_speed * 0.5f;

    float velX = m_dirX * wanderSpeed;
    float velY = m_dirY * wanderSpeed;

    MoveWithCollision(velX, velY, dungeon);
}

bool Stalker::HasLineOfSight(const Dungeon& dungeon, float playerX, float playerY){
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

        // Use dungeon collision check instead of Level
        if(dungeon.CheckTileCollision((int)currX, (int)currY)){
            return false;
        }
    }

    return true;
}