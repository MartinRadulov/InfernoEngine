#include "../include/projectile.h"
#include "../include/dungeon.h"

Projectile::Projectile(float x, float y, float velX, float velY)
    : m_collider(S_ENEMY, S_ENEMY,0 , 0){
    m_x = x;
    m_y = y;

    m_velX = velX * m_speed;
    m_velY = velY * m_speed;
}

void Projectile::Update(const Dungeon& dungeon, float ownerX, float ownerY){
    if(m_isAttached){
        m_x = ownerX + m_attachedOffX;
        m_y = ownerY + m_attachedOffY;
    }
    else{
        m_x += m_velX;
        m_y += m_velY;
    }

    //Life time logic
    if(m_lifeTime > 0){
        m_lifeTime--;
        if(m_lifeTime == 0){
            m_isActive = false;
        }
    }

    // Sync the Collider to the new position
    m_collider.SetPosition(m_x, m_y);

    // Wall Collision Check
    if (m_collider.CheckMapCollision(dungeon)) {
        if(m_destroyOnImpact){
            m_isActive = false;
        }
    }

    if(isOffScreen()){
        if(m_destroyOnImpact){
            m_isActive = false;
        }
    }
}

void Projectile::Render(std::vector<RenderObject>& renderList, int camOffsetX, int camOffsetY){
    RenderObject bObj;
    bObj.textureID = m_textureID; // Using rock texture for bullets per your code
    bObj.srcRect = {0, 0, SPRITE_SHEET_SIZE, SPRITE_SHEET_SIZE};
    bObj.destRect = {(int)m_x + camOffsetX, (int)m_y + camOffsetY, m_width, m_height};
    bObj.sortY = m_y + m_height;
    bObj.flip = SDL_FLIP_NONE;
    renderList.push_back(bObj);
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