#pragma once
#include "TextureManager.h"

#if defined(_WIN32)
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

// We need to know about the Level to check for walls!
#include "level.h" 

class Player{
public:
    Player(float startX, float startY);

    void Update(const Uint8* keyState, Level& level);

    void Render(SDL_Renderer* renderer);

    float GetX() const {return m_x;}
    float GetY() const {return m_y;}

    bool CanShoot();
    void Shoot();
    void SetFireRate(int delay);
private:
    float m_x;
    float m_y;

    int m_height = 32;
    int m_width = 32;

    float m_speed = 2.5f;

    int m_fireRate;
    Uint32 m_lastShotTime;

    bool CheckCollision(float newX, float newY, Level& level);
};