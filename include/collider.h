#pragma once
#include "TextureManager.h"

#if defined(_WIN32)
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#include "utils.h"
#include "level.h"

struct Collider{
    SDL_Rect box;

    int offsetX;
    int offsetY;

    Collider(int w, int h, int offX, int offY);

    void SetPosition(float x, float y);

    bool CheckMapCollision(Level& level);

    bool CheckOverlap(Collider& other);
};