#pragma once\

#if defined(_WIN32)
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#include "TextureManager.h"
#include "level.h"
#include "collider.h"

class Projectile{
public:
    Projectile(float x, float y, float velX, float velY);

    void Update(Level& level);

    void Render(std::vector<RenderObject>& renderList);

    bool isOffScreen();

    bool GetIsActive() const {return m_isActive;}

    void Deactivate();

    float GetX() const {return m_x;}
    float GetY() const {return m_y;}
    int GetWidth() const {return m_width;}
    int GetHeight() const {return m_height;}
    Collider& GetCollider() {return m_collider;}

private:
    bool m_isActive = true;
    float m_x, m_y;
    float m_velX, m_velY;
    float m_speed = 10.0f;
    int m_width = S_ENEMY;
    int m_height = S_ENEMY;
    Collider m_collider;

    bool CheckCollision(float newX, float newY, Level& level);
};