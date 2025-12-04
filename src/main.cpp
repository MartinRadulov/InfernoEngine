#include <iostream>

// Cross-platform include for SDL
#if defined(_WIN32)
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

int main(int argc, char* argv[]) {
    // 1. Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "CRITICAL: SDL could not initialize! Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    std::cout << "InfernoEngine is starting..." << std::endl;

    // 2. Create the Window
    SDL_Window* window = SDL_CreateWindow(
        "InfernoEngine v0.1",       // Title
        SDL_WINDOWPOS_CENTERED,     // X
        SDL_WINDOWPOS_CENTERED,     // Y
        800, 600,                   // W, H
        SDL_WINDOW_SHOWN            // Flags
    );

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (!window) {
        std::cout << "CRITICAL: Window creation failed! Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    bool isRunning = true;
    SDL_Event event;

    while(isRunning){
         // A. Handle Input (Check if user pressed X)
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                isRunning = false;
            }
        }

        // B. Update Game Logic (Nothing here yet)

        // C. Render (Draw to screen)
        // 1. Pick color: Dark Grey (R=40, G=40, B=40)
        SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
        
        // 2. Clear screen
        SDL_RenderClear(renderer);
        
        // 3. Show it
        SDL_RenderPresent(renderer);
    }

    // 4. Cleanup
    std::cout << "InfernoEngine shutting down..." << std::endl;
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}