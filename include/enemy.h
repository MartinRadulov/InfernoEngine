#pragma once
#if defined(_WIN32)
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#include "level.h"
#include "TextureManager.h"
#include "utils.h"
#include "collider.h"
#include <string>

class Dungeon; // Forward declaration

enum class EnemyState{
    WANDERING,
    CHASING
};

class Enemy{
public:
    Enemy(float x, float y, int width, int height);

    virtual ~Enemy() {}

    Enemy(const Enemy&) = delete;

    Enemy& operator= (const Enemy&) = delete;

    virtual void Update(const Dungeon& dungeon, float playerX, float playerY) = 0;

    virtual void Render(std::vector<RenderObject>& renderList, int camOffsetX, int camOffsetY);

    void TakeDamage(float dmgNum);

    bool IsDead() const {return m_isActive;} 

    float GetX() const {return m_x;}
    float GetY() const {return m_y;}
    int GetWidth() const {return m_width;}
    int GetHeight() const {return m_height;}
    float GetDmg() const {return m_damage;}
    Collider& GetCollider() {return m_collider;}
    std::string GetTextureID() const {return m_textureID;}
protected:
    float m_x;
    float m_y;
    int m_height = M_ENEMY;
    int m_width = M_ENEMY;
    float m_speed;
    float m_health;
    float m_damage;
    bool m_isActive = true;
    std::string m_textureID;
    Collider m_collider;

    void MoveWithCollision(float velX, float velY, const Dungeon& dungeon);
    void MoveFlying(float velX, float velY);
}; 