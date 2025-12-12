#pragma once
#if defined(_WIN32)
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#include "level.h"
#include "TextureManager.h"
#include "utils.h"
#include <string>

class Enemy{
public:
    Enemy(float x, float y, int width, int height);

    virtual ~Enemy() {}

    Enemy(const Enemy&) = delete;

    Enemy& operator= (const Enemy&) = delete;

    virtual void Update(Level& level, float playerX, float playerY) = 0;

    virtual void Render(SDL_Renderer* renderer);

    void TakeDamage(float dmgNum);

    bool IsDead() const {return m_isActive;} 

    float GetX() const {return m_x;}
    float GetY() const {return m_y;}
    int GetWidth() const {return m_width;}
    int GetHeight() const {return m_height;}
protected:
    float m_x;
    float m_y;
    int m_height = M_ENEMY;
    int m_width = M_ENEMY;
    float m_speed;
    float m_health;
    bool m_isActive = true;
    std::string m_textureID;

    bool CheckCollision(float newX, float newY, Level& level);
}; 