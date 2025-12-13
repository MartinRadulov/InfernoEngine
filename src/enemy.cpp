#include "../include/enemy.h"
#include <iostream> // For debugging if needed

Enemy::Enemy(float x, float y, int width, int height) 
    : m_x(x), m_y(y), m_width(width), m_height(height) {}

void Enemy::Render(SDL_Renderer* renderer) {
    int row = 0;
    int frame = 0;

    TextureManager::GetInstance()->DrawFrame(
        m_textureID.c_str(), 
        (int)m_x, (int)m_y, 
        m_width, m_height,       // DESTINATION (Defined in Child Constructor)
        SPRITE_SHEET_SIZE, SPRITE_SHEET_SIZE, // SOURCE (High Res)
        row, frame, 
        renderer
    );
}

bool Enemy::CheckCollision(float newX, float newY, Level& level) {
    if (newX < 0 || newX + m_width > SCREEN_WIDTH ||
            newY < 0 || newY + m_height > SCREEN_HEIGHT) {
            return true;
        }

    int leftTileCol   = (int)(newX) / TILE_SIZE;
    int topTileRow    = (int)(newY) / TILE_SIZE;

    int rightTileCol  = (int)(newX + m_width) / TILE_SIZE;
    int bottomTileRow = (int)(newY + m_height) / TILE_SIZE;

    if (level.GetTile(topTileRow, leftTileCol) == 1) return true;      // Hit Top-Left
    if (level.GetTile(topTileRow, rightTileCol) == 1) return true;     // Hit Top-Right
    if (level.GetTile(bottomTileRow, leftTileCol) == 1) return true;   // Hit Bottom-Left
    if (level.GetTile(bottomTileRow, rightTileCol) == 1) return true;  // Hit Bottom-Right

    return false; // No wall hit
}

void Enemy::TakeDamage(float dmgNum){
    m_health -= dmgNum;
    if(m_health <= 0){
        m_health = 0;
        m_isActive = false;
    }
}