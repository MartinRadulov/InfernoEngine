#include "../include/projectile.h"

Projectile::Projectile(float x, float y, float velX, float velY){
    m_x = x;
    m_y = y;

    m_velX = velX * m_speed;
    m_velY = velY * m_speed;
}

void Projectile::Update(Level& level){
    float nextX = m_x;
    float nextY = m_y;

    nextX += m_velX;
    nextY += m_velY;

    if(!CheckCollision(nextX, nextY, level)){
        m_x = nextX;
        m_y = nextY;
    }
    else{
        m_isActive = false;
    }

    if(isOffScreen()){
        m_isActive = false;
    }
}

void Projectile::Render(SDL_Renderer* rednerer){
    TextureManager::GetInstance()->Draw("rock", (int)m_x, (int)m_y, m_width, m_height, rednerer);
}

bool Projectile::isOffScreen(){
    //fix later with actual constraints
    return (m_x < -50 || m_y > 850 || m_y < -50 || m_y > 650);
}

bool Projectile::CheckCollision(float newX, float newY, Level& level){
    int leftTileCol   = (int)(newX) / TILE_SIZE;
    int topTileRow    = (int)(newY) / TILE_SIZE;

    // Right-Bottom corner
    int rightTileCol  = (int)(newX + m_width) / TILE_SIZE;
    int bottomTileRow = (int)(newY + m_height) / TILE_SIZE;

    // Check the Level array for walls (1) at these spots
    if (level.GetTile(topTileRow, leftTileCol) == 1) return true;      // Hit Top-Left
    if (level.GetTile(topTileRow, rightTileCol) == 1) return true;     // Hit Top-Right
    if (level.GetTile(bottomTileRow, leftTileCol) == 1) return true;   // Hit Bottom-Left
    if (level.GetTile(bottomTileRow, rightTileCol) == 1) return true;  // Hit Bottom-Right

    return false; // No wall hit
}

void Projectile::Deactivate(){
    m_isActive = false;
}