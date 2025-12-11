#include "../include/enemy.h"
#include <iostream> // For debugging if needed

Enemy::Enemy(float x, float y) {
    m_x = x;
    m_y = y;
}

void Enemy::Update(float playerX, float playerY, Level& level) {
    float nextX = m_x;
    float nextY = m_y;

    if(playerX > m_x) nextX += m_speed;
    if(playerX < m_x) nextX -= m_speed;
    if(playerY > m_y) nextY += m_speed;
    if(playerY < m_y) nextY -= m_speed;

    if (!CheckCollision(nextX, m_y, level)) {
        m_x = nextX;
    }

    if (!CheckCollision(m_x, nextY, level)) {
        m_y = nextY;
    }
}

void Enemy::Render(SDL_Renderer* renderer) {
    TextureManager::GetInstance()->Draw("isaac", (int)m_x, (int)m_y, m_width, m_height, renderer);
}

bool Enemy::CheckCollision(float newX, float newY, Level& level) {
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

void Enemy::TakeDamage(){
    
}