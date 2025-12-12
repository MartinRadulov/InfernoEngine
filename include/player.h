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
    float GetDmg() const {return m_damage;}

    bool CanShoot();
    void Shoot();
    void SetFireRate(int delay);
    void TakeDamage(float dmgNum);
    bool IsDead() const {return m_isActive;}
private:
    float m_x;
    float m_y;
    int m_height = 32;
    int m_width = 32;
    Uint32 m_lastShotTime;
    Uint32 m_lastDamageTime;
    int m_invTime;
    bool m_isActive = true;

    float m_speed = 2.5f;
    int m_fireRate;
    float m_damage = 3.5f;
    float m_health;
    float m_maxHealth;

    bool CheckCollision(float newX, float newY, Level& level);
};