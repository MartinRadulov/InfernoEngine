#pragma once
#if defined(_WIN32)
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h> 
#endif

const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;
const int TILE_SIZE = 64;
const int MAP_ROWS = 15;
const int MAP_COLS = 20;
const int S_ENEMY = 32;
const int M_ENEMY = 64;
const int L_ENEMY = 128;
const int SPRITE_SHEET_SIZE = 256;
const int SWORD_LIFE = 10;
const int SWORD_SIZE = 48;
// Sword spawn distances - different for horizontal/vertical due to asymmetric player collider (32w x 48h)
const int SWORD_SPAWN_DISTANCE_HORIZONTAL = 32;  // For left/right attacks
const int SWORD_SPAWN_DISTANCE_VERTICAL = 40;    // For up/down attacks (32 + 8 for taller collider)
const int ARROW_SPAWN_DISTANCE = 20;  // Distance from player collider center to arrow center


inline int PixelToGrid(float pixelCoord) {
    return (int)(pixelCoord) / TILE_SIZE;
}

inline bool InitializeSDL(SDL_Window*& window, SDL_Renderer*& renderer) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return false;

    // Init Image
    int flags = IMG_INIT_PNG | IMG_INIT_JPG | IMG_INIT_TIF; 
    if((IMG_Init(flags) & flags) != flags) return false;

    // Create Window
    window = SDL_CreateWindow("InfernoEngine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) return false;

    // Create Renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) return false;

    return true;
}