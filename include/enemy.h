#pragma once
#if defined(_WIN32)
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#include "level.h"
#include "TextureManager.h"

class Enemy{
public:
    Enemy(float x, float y);

    void Update(float playerX, float playerY, Level& level);

    void Render(SDL_Renderer* renderer);

    void TakeDamage();

    bool IsDead() const {return m_isActive;} 

    float GetX() const {return m_x;}
    float GetY() const {return m_y;}
    int GetWidth() const {return m_width;}
    int GetHeight() const {return m_height;}
private:
    float m_x;
    float m_y;
    int m_height = 32;
    int m_width = 32;
    float m_speed = 1.5f;
    int m_health;
    bool m_isActive = true;

    bool CheckCollision(float newX, float newY, Level& level);
};