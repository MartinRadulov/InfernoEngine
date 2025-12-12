#pragma once
#if defined(_WIN32)
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#include "../enemy.h"

class Stalker : public Enemy{
public:
    Stalker(float x, float y);

    void Update(Level& level, float playerX, float playerY) override;
private:
    EnemyState m_currentState;
    int m_moveTimer;
    float m_dirX;
    float m_dirY;

    void Wander(Level& level);

    void Chase(Level& level, float playerX, float playerY);

    bool HasLineOfSight(Level& level, float playerX, float playerY);
};