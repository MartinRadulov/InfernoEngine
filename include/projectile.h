#pragma once

#if defined(_WIN32)
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#include "TextureManager.h"
#include "level.h"
#include "collider.h"

class Dungeon; // Forward declaration

class Projectile{
public:
    Projectile(float x, float y, float velX, float velY);

    void Update(const Dungeon& dungeon, float owenerX, float ownerY);
    void Render(std::vector<RenderObject>& renderList, int camOffsetX, int camOffsetY);
    bool isOffScreen();
    bool GetIsActive() const {return m_isActive;}
    void Deactivate();


    void SetLifeTime(int frames) {m_lifeTime = frames;}
    void SetTexture(std::string id) {m_textureID = id;}
    void SetDimensions(int w, int h){
        m_width = w;
        m_height = h;
        m_collider = {w, h, 0, 0};
    }
    void SetDestroyImpact(bool destroy) {m_destroyOnImpact = destroy;}
    void SetAttached(bool attached, float offX, float offY){
        m_isAttached = attached;
        m_attachedOffX = offX;
        m_attachedOffY = offY;
    }

    float GetX() const {return m_x;}
    float GetY() const {return m_y;}
    int GetWidth() const {return m_width;}
    int GetHeight() const {return m_height;}
    Collider& GetCollider() {return m_collider;}
    bool GetDestroyImpact() const {return m_destroyOnImpact;}

private:
    bool m_isActive = true;
    float m_x, m_y;
    float m_velX, m_velY;
    float m_speed = 10.0f;
    int m_width = S_ENEMY;
    int m_height = S_ENEMY;
    Collider m_collider;
    int m_lifeTime = -1;
    std::string m_textureID = "rock";
    bool m_destroyOnImpact = true;
    bool m_isAttached = false;
    float m_attachedOffX = 0;
    float m_attachedOffY = 0;

    bool CheckCollision(float newX, float newY, Level& level);
};