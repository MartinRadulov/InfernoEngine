#pragma once
#include "TextureManager.h"
#include "utils.h"

// Cross-platform SDL include
#if defined(_WIN32)
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

class Level {
public:
    // Constructor
    Level();

    // Functions
    void Generate(int pRow, int pCol);
    void RenderFloors(SDL_Renderer* renderer);
    
    // We will need this later for collision!
    // It allows the player to ask "Is this spot a wall?"
    int GetTile(int row, int col);

private:
    // The actual data (Hidden from the outside world)
    int m_mapData[MAP_ROWS][MAP_COLS];
};