#include "../include/projectile.h"

Projectile::Projectile(float x, float y, float velX, float velY){
    m_x = x;
    m_y = y;

    m_velX = velX * m_speed;
    m_velY = velY * m_speed;
}

void Projectile::Update(){
    m_x += m_velX;
    m_y += m_velY;
}

void Projectile::Render(SDL_Renderer* rednerer){
    TextureManager::GetInstance()->Draw("rock", (int)m_x, (int)m_y, m_width, m_height, rednerer);
}

bool Projectile::isOffScreen(){
    //fix later with actual constraints
    return (m_x < -50 || m_y > 850 || m_y < -50 || m_y > 650);
}