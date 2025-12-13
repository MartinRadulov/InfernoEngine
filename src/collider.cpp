#include "../include/collider.h"

Collider::Collider(int w, int h, int offX, int offY){
    box = {0, 0, w, h};
    offsetX = offX;
    offsetY = offY;
}

void Collider::SetPosition(float x, float y){
        box.x = (int)(x + offsetX);
        box.y = (int)(y + offsetY);
}

bool Collider::CheckMapCollision(Level& level){
    if(box.x < offsetX || box.y < offsetY) return true;

    if (box.x + box.w + offsetX > SCREEN_WIDTH  || box.y + box.h + offsetY > SCREEN_HEIGHT) return true;

    int leftCol   = PixelToGrid(box.x);
    int topRow    = PixelToGrid(box.y);
    int rightCol  = PixelToGrid(box.x + box.w - 1);
    int bottomRow = PixelToGrid(box.y + box.h - 1);

    if(level.GetTile(topRow, leftCol) == 1) return true;
    if(level.GetTile(topRow, rightCol) == 1) return true;
    if(level.GetTile(bottomRow, leftCol) == 1) return true;
    if(level.GetTile(bottomRow, rightCol) == 1) return true;

    return false;
}

bool Collider::CheckOverlap(Collider& other){
    if(box.x + box.w < other.box.x || other.box.x + other.box.w < box.x) return false;
    if(box.y + box.h < other.box.y || other.box.y + other.box.h < box.y) return false;
    return true;
}