#include <iostream>
#include <vector>
#include <SDL2/SDL_image.h>
#include <algorithm>
#include <cstdlib>
#include <ctime>
//#include <SDL2/SDL_ttf.h> // (Later, for text)
#include "../include/level.h"
#include "../include/player.h"
#include "../include/TextureManager.h"
#include "../include/projectile.h"
#include "../include/enemy.h"
#include "../include/utils.h"
#include "../include/enemies/zombie.h"
#include "../include/enemies/fly.h"
#include "../include/enemies/stalker.h"
#include "../include/dungeon.h"

// Cross-platform include for SDL
#if defined(_WIN32)
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

int main(int argc, char* argv[]) {
    //Initialize SDL
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    if(!InitializeSDL(window, renderer));

    //Load All Assets
    TextureManager::GetInstance()->Load("assets/isaac.png", "isaac", renderer);
    TextureManager::GetInstance()->Load("assets/rock.png", "rock", renderer);
    TextureManager::GetInstance()->Load("assets/floor.png", "floor", renderer);
    TextureManager::GetInstance()->Load("assets/fly.png", "fly", renderer);
    TextureManager::GetInstance()->Load("assets/zombie.png", "zombie", renderer);

    //Creating the objects
    Level currentLevel;
    Player player(80.0f, 80.0f);
    std::vector<Projectile> bullets;
    std::vector<Enemy*> enemies;

    //TEST!!!!
    //enemies.push_back(new Zombie(300.0f, 300.0f));
    enemies.push_back(new Fly(100.0f, 100.0f));
    enemies.push_back(new Stalker(300.0f, 350.0f));

    bool debugMode = false;

    //DUNGEON TEST
    std::srand(std::time(nullptr));
    Dungeon testDungeon;
    testDungeon.GenerateDungeon(15);
    testDungeon.PrintMapToConsole();

    //Game running
    bool isRunning = true;
    SDL_Event event;
    while(isRunning){
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT) isRunning = false;
            else if(event.type == SDL_KEYDOWN){
                if(event.key.keysym.sym == SDLK_ESCAPE) isRunning = false;

                if(event.key.keysym.sym == SDLK_h) debugMode = !debugMode;

                if(event.key.keysym.sym == SDLK_r){
                    int pRow = PixelToGrid(player.GetY());
                    int pCol = PixelToGrid(player.GetX());
                    currentLevel.Generate(pRow, pCol);
                }

                player.HandleInput(event, bullets);
            }
        }

    //Updating the objects 
    const Uint8* keys = SDL_GetKeyboardState(NULL);
    player.Update(keys, currentLevel);
    for(auto& bullet : bullets){
        bullet.Update(currentLevel, player.GetX(), player.GetY());
    }
    for(auto& enemy : enemies){
        enemy->Update(currentLevel, player.GetX(), player.GetY());
    }

    //Check collisions
    for(auto& bullet : bullets){
        if(!bullet.GetIsActive()) continue;
        for(auto& enemy : enemies){
            if(bullet.GetCollider().CheckOverlap(enemy->GetCollider())){
                if(bullet.GetDestroyImpact()){
                    bullet.Deactivate();
                }
                    enemy->TakeDamage(player.GetDmg());
                }
        }
    }

    for(auto& enemy : enemies){
        if(player.GetCollider().CheckOverlap(enemy->GetCollider())){
                player.TakeDamage(enemy->GetDmg());
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
        if(!enemies[i]->IsDead()){
            enemies.erase(enemies.begin() + i);
            i--;
        }
    }

    //Rendering part
    //---------------------------------------------------------------
    SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
    SDL_RenderClear(renderer);

    currentLevel.RenderFloors(renderer);

    std::vector<RenderObject> renderList;
    player.Render(renderList);
    for(auto& enemy : enemies){
        enemy->Render(renderList);
    }
    for(auto& bullet : bullets){
        bullet.Render(renderList);
    }
    for (int row = 0; row < MAP_ROWS; row++) {
        for (int col = 0; col < MAP_COLS; col++) {
            if (currentLevel.GetTile(row, col) == 1) {
                RenderObject rObj;
                rObj.textureID = "rock";
                rObj.srcRect = {0, 0, SPRITE_SHEET_SIZE, SPRITE_SHEET_SIZE};
                rObj.destRect = {col * TILE_SIZE, row * TILE_SIZE, TILE_SIZE, TILE_SIZE};
                rObj.sortY = (row * TILE_SIZE) + TILE_SIZE;
                renderList.push_back(rObj);
            }
        }
    }

    std::sort(renderList.begin(), renderList.end());

    for (auto& obj : renderList) {
        TextureManager::GetInstance()->DrawFrameRect(
            obj.textureID.c_str(), 
            obj.srcRect, 
            obj.destRect, 
            renderer, 
            obj.flip
        );
    }

    //Debug Mode
    if(debugMode){
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        for(int row = 0; row < MAP_ROWS; row++){
            for(int col = 0; col < MAP_COLS; col++){
                if(currentLevel.GetTile(row, col) == 1){
                    SDL_Rect wallRect = {col * TILE_SIZE, row * TILE_SIZE, TILE_SIZE, TILE_SIZE};
                    SDL_RenderDrawRect(renderer, &wallRect);
                }
            }
        }
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        for(auto& enemy : enemies){
            SDL_Rect enemyRect = enemy->GetCollider().box;
            SDL_RenderDrawRect(renderer, &enemyRect);
            }
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        for(auto& bullet : bullets){
            SDL_Rect bulletRect = bullet.GetCollider().box;
            SDL_RenderDrawRect(renderer, &bulletRect);
            }
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_Rect playerRect = player.GetCollider().box;
            SDL_RenderDrawRect(renderer, &playerRect);
    }

    SDL_RenderPresent(renderer);
    }

    IMG_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}