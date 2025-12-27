#pragma once
#if defined(_WIN32)
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#include "../enemy.h"

class Zombie : public Enemy{
public:
    Zombie(float x, float y);

    void Update(const Dungeon& dungeon, float playerX, float playerY) override;
};