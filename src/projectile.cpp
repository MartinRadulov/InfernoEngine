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

void Projectile::Render(SDL_Renderer* renderer){
    int currentRow = 0; 
    int currentFrame = 0; // We will animate this later

    TextureManager::GetInstance()->DrawFrame(
        "rock", 
        (int)m_x, (int)m_y, 
        m_width, m_height,       // DESTINATION (e.g., 64x64)
        SPRITE_SHEET_SIZE, SPRITE_SHEET_SIZE, // SOURCE (256x256)
        currentRow, currentFrame, 
        renderer
    );
}

bool Projectile::isOffScreen(){
    //fix later with actual constraints
    return (m_x < 0 || m_y > SCREEN_HEIGHT || m_y < 0 || m_x > SCREEN_WIDTH);
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