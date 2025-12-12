#pragma once
#if defined(_WIN32)
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int TILE_SIZE = 40;
const int MAP_ROWS = 15;
const int MAP_COLS = 20;

inline bool CheckOverlap(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2){
    if(x1 + w1 < x2 || x2 + w2 < x1) return false;
    if(y1 + h1 < y2 || y2 + h2 < y1) return false;
    
    return true;
}

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