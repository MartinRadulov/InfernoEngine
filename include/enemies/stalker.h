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

    void Update(const Dungeon& dungeon, float playerX, float playerY) override;
private:
    EnemyState m_currentState;
    int m_moveTimer;
    float m_dirX;
    float m_dirY;

    void Wander(const Dungeon& dungeon);

    void Chase(const Dungeon& dungeon, float playerX, float playerY);

    bool HasLineOfSight(const Dungeon& dungeon, float playerX, float playerY);
};