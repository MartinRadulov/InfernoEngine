#pragma once\

#if defined(_WIN32)
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#include "TextureManager.h"

class Projectile{
public:
    Projectile(float x, float y, float velX, float velY);

    void Update();

    void Render(SDL_Renderer* renderer);

    bool isOffScreen();

private:
    float m_x, m_y;
    float m_velX, m_velY;
    float m_speed = 10.0f;
    int m_width = 16;
    int m_height = 16;
};