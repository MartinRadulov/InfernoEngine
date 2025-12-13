#include "../include/player.h"
#include <iostream> // For debugging if needed

Player::Player(float startX, float startY)
    : m_collider(S_ENEMY, M_ENEMY - (S_ENEMY / 2), (M_ENEMY - S_ENEMY) / 2, S_ENEMY / 2) 
 {
    m_x = startX;
    m_y = startY;
    m_lastDamageTime = 0;
    m_invTime = 1000;
    m_width = M_ENEMY;
    m_height = M_ENEMY;
    m_maxHealth = 100;
    m_health = m_maxHealth;
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

    m_collider.SetPosition(nextX, m_y);
    if(!m_collider.CheckMapCollision(level)){
        m_x = nextX;
    }
    m_collider.SetPosition(m_x, nextY);
    if(!m_collider.CheckMapCollision(level)){
        m_y = nextY;
    }

    m_collider.SetPosition(m_x, m_y);
}

void Player::Render(std::vector<RenderObject>& renderList) {
    bool isInvincible = (SDL_GetTicks() - m_lastDamageTime < m_invTime);
    bool blinkInvisible = (SDL_GetTicks() / 100) % 2 == 0;

    if (isInvincible && blinkInvisible) {
        return; // Don't add myself to the list!
    }

    // 2. Create the Render Object
    RenderObject pObj;
    pObj.textureID = "isaac";
    pObj.srcRect = {0, 0, SPRITE_SHEET_SIZE, SPRITE_SHEET_SIZE}; // Or calculate animation frame here
    pObj.destRect = { (int)m_x, (int)m_y, m_width, m_height };
    pObj.sortY = m_y + m_height; // Sort by feet
    pObj.flip = SDL_FLIP_NONE;

    // 3. Add to list
    renderList.push_back(pObj);
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

void Player::TakeDamage(float dmgNum){
    Uint32 currTime = SDL_GetTicks();
    if(currTime - m_lastDamageTime < m_invTime){
        return;
    }

    m_health -= dmgNum;
    m_lastDamageTime = currTime;

    std::cout << "OUCH! Player HP:" << m_health << std::endl;

    if(m_health <= 0){
        m_health = 0;
        m_isActive = false;
        std::cout << "DEAD" << std::endl;
    }
}