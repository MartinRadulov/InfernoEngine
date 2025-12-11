#include <iostream>
#include <vector>
#include <SDL2/SDL_image.h>
//#include <SDL2/SDL_ttf.h> // (Later, for text)
#include "../include/level.h"
#include "../include/player.h"
#include "../include/TextureManager.h"
#include "../include/projectile.h"
#include "../include/enemy.h"
#include "../include/utils.h"

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

    int flags = IMG_INIT_PNG | IMG_INIT_JPG | IMG_INIT_TIF; // Add all major flags
    if((IMG_Init(flags) & flags) != flags) {
        std::cout << "IMG_Init failed! Required flags not met: " << IMG_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    //Creating window
    SDL_Window* window = SDL_CreateWindow("InfernoEngine - Collision", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
    SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    //Load All Assets
    TextureManager::GetInstance()->Load("assets/isaac.png", "isaac", renderer);
    TextureManager::GetInstance()->Load("assets/rock.png", "rock", renderer);
    TextureManager::GetInstance()->Load("assets/floor.png", "floor", renderer);

    //Creating the objects
    Level currentLevel;
    Player player(80.0f, 80.0f);
    std::vector<Projectile> bullets;
    std::vector<Enemy> enemies;

    //TEST!!!!
    enemies.push_back(Enemy(300.0f, 300.0f));

    bool isRunning = true;
    SDL_Event event;

    //Game running
    while(isRunning){
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT) isRunning = false;
            else if(event.type = SDL_KEYDOWN){
                if(event.key.keysym.sym == SDLK_ESCAPE) isRunning = false;

                if(event.key.keysym.sym == SDLK_r){
                    float px = player.GetX();
                    float py = player.GetY();

                    int pRow = (int)(py) / TILE_SIZE;
                    int pCol = (int)(px) / TILE_SIZE;

                    currentLevel.Generate(pRow, pCol);
                }

                if(player.CanShoot()){
                    bool shotFired = false;
                    float px = player.GetX();
                    float py = player.GetY();

                    if(event.key.keysym.sym == SDLK_UP){

                        bullets.push_back(Projectile(px, py, 0, -1));
                        shotFired = true;
                    }
                    else if(event.key.keysym.sym == SDLK_DOWN){

                        bullets.push_back(Projectile(px, py, 0, 1));
                        shotFired = true;
                    }
                    else if(event.key.keysym.sym == SDLK_LEFT){

                        bullets.push_back(Projectile(px, py, -1, 0));
                        shotFired = true;
                    }
                    else if(event.key.keysym.sym == SDLK_RIGHT){

                        bullets.push_back(Projectile(px, py, 1, 0));
                        shotFired = true;
                    }

                    if(shotFired){
                        player.Shoot();
                    }
                }
            }
        }

    //Updating the objects 
    const Uint8* keys = SDL_GetKeyboardState(NULL);
    player.Update(keys, currentLevel);
    for(auto& bullet : bullets){
        bullet.Update(currentLevel);
    }
    for(auto& enemy : enemies){
        enemy.Update(player.GetX(), player.GetY(), currentLevel);
    }

    //Check collisions
    for(auto& bullet : bullets){
        for(auto& enemy : enemies){
            if(CheckOverlap(bullet.GetX(), bullet.GetY(), bullet.GetWidth(), bullet.GetHeight(),
                enemy.GetX(), enemy.GetY(), enemy.GetWidth(), enemy.GetHeight())){
                    bullet.Deactivate();
                    enemy.TakeDamage();
                }
        }
    }

    //Clean up
    for(int i = 0; i < bullets.size(); i++){
        if(!bullets[i].GetIsActive()){
            bullets.erase(bullets.begin() + i);
            i--;
        }
    }
    for(int i = 0; i < enemies.size(); i++){
        if(!enemies[i].IsDead()){
            enemies.erase(enemies.begin() + i);
            i--;
        }
    }

    //Rendering part
    //---------------------------------------------------------------
    SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
    SDL_RenderClear(renderer);

    currentLevel.Render(renderer);
    player.Render(renderer);
    for(auto& bullet : bullets){
        bullet.Render(renderer);
    }
    for(auto& enemy : enemies){
        enemy.Render(renderer);
    }



    SDL_RenderPresent(renderer);
    }

    IMG_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}