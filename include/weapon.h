#pragma once
#if defined(_WIN32)
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#include <vector>
#include "collider.h"
#include "projectile.h"

class Weapon{
public:
    Weapon(int fRate, float damage) : m_fRate(fRate), m_damage(damage), m_lastAttackTime(0) {}

    virtual void Attack(float x, float y, int dirX, int dirY, std::vector<Projectile>& bulletList) = 0;

    bool CanAttack() {return SDL_GetTicks() - m_lastAttackTime >= m_fRate;}

    void SetFireRate(int fr) {m_fRate = fr;}
    void SetDamage(int dmg) {m_damage = dmg;}
protected:
    int m_fRate;
    float m_damage;
    Uint32 m_lastAttackTime;

    void ResetTimer() {m_lastAttackTime = SDL_GetTicks();}
};
