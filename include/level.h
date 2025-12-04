#pragma once // Prevents circular include errors

// Cross-platform SDL include
#if defined(_WIN32)
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

// Constants (Moved from main.cpp)
const int TILE_SIZE = 40;
const int MAP_ROWS = 15;
const int MAP_COLS = 20;

class Level {
public:
    // Constructor
    Level();

    // Functions
    void Generate();
    void Render(SDL_Renderer* renderer);
    
    // We will need this later for collision!
    // It allows the player to ask "Is this spot a wall?"
    int GetTile(int row, int col);

private:
    // The actual data (Hidden from the outside world)
    int m_mapData[MAP_ROWS][MAP_COLS];
};