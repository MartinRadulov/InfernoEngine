#pragma once
#if defined(_WIN32)
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#include "../enemy.h"

class Fly : public Enemy{
public:
    Fly(float x, float y);

    void Update(Level& level, float playerX, float playerY) override;
};