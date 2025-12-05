#include <iostream>
#include "../include/level.h"
#include "../include/player.h"

// Cross-platform include for SDL
#if defined(_WIN32)
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

const int SCREEN_HEIGHT = 800;
const int SCREEN_WIDTH = 600;

int main(int argc, char* argv[]) {
    // 1. Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return -1;

    SDL_Window* window = SDL_CreateWindow("InfernoEngine - Collision", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
    SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    Level currentLevel;
    Player player(80.0f, 80.0f);

    bool isRunning = true;
    SDL_Event event;

    //why is this a while
    while(isRunning){
        //explain why is this needed and how do we go to the next loop
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT) isRunning = false;
            else if(event.type = SDL_KEYDOWN){
                if(event.key.keysym.sym == SDLK_ESCAPE) isRunning = false; //what is sdlk
                if(event.key.keysym.sym == SDLK_r){
                    float px = player.GetX();
                    float py = player.GetY();

                    int pRow = (int)(px) / TILE_SIZE;
                    int pCol = (int)(py) / TILE_SIZE;

                    currentLevel.Generate(pRow, pCol);
                }
            }
        }

        //what are we doing here
    const Uint8* keys = SDL_GetKeyboardState(NULL);

    player.Update(keys, currentLevel);

    SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
    SDL_RenderClear(renderer);

    currentLevel.Render(renderer);
    player.Render(renderer);

    SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}