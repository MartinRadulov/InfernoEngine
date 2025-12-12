#include "../include/player.h"
#include <iostream> // For debugging if needed

Player::Player(float startX, float startY) {
    m_x = startX;
    m_y = startY;
    m_fireRate = 500;
    m_lastShotTime = 0;
}

void Player::Update(const Uint8* keyState, Level& level) {
    // 1. Calculate where we WANT to go
    float nextX = m_x;
    float nextY = m_y;

    if (keyState[SDL_SCANCODE_W]) nextY -= m_speed;
    if (keyState[SDL_SCANCODE_S]) nextY += m_speed;
    if (keyState[SDL_SCANCODE_A]) nextX -= m_speed;
    if (keyState[SDL_SCANCODE_D]) nextX += m_speed;

    // 2. Resolve X Axis Collision
    // We check X and Y separately. This allows "sliding" along walls.
    if (!CheckCollision(nextX, m_y, level)) {
        m_x = nextX;
    }

    // 3. Resolve Y Axis Collision
    if (!CheckCollision(m_x, nextY, level)) {
        m_y = nextY;
    }
}

void Player::Render(SDL_Renderer* renderer) {
    TextureManager::GetInstance()->Draw("isaac", (int)m_x, (int)m_y, m_width, m_height, renderer);
}

// THE MATH PART
//explain this whole part
bool Player::CheckCollision(float newX, float newY, Level& level) {
    if (newX < 0 || newX + m_width > SCREEN_WIDTH ||
        newY < 0 || newY + m_height > SCREEN_HEIGHT) {
        return true;
    }

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

bool Player::CanShoot(){
    Uint32 currentTime = SDL_GetTicks();
    if(currentTime - m_lastShotTime > m_fireRate) return true;

    return false;
}

void Player::Shoot(){
    m_lastShotTime = SDL_GetTicks();
}

void Player::SetFireRate(int delay){
    m_fireRate = delay;
}
