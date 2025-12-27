#include <iostream>
#include <vector>
#include <SDL2/SDL_image.h>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <memory>
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
#include "../include/camera.h"
#include "../include/room.h"
#include "../include/dungeon_constants.h"

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
    // Player will be positioned in the start room later
    Player player(80.0f, 80.0f);
    std::vector<Projectile> bullets;
    std::vector<std::unique_ptr<Enemy>> enemies;

    //TEST!!!!
    //enemies.push_back(new Zombie(300.0f, 300.0f));
    enemies.push_back(std::make_unique<Fly>(100.0f, 100.0f));
    enemies.push_back(std::make_unique<Stalker>(300.0f, 350.0f));

    bool debugMode = false;

    //DUNGEON TEST
    //std::srand(std::time(nullptr));
    Dungeon testDungeon;
    testDungeon.GenerateDungeon(15);
    testDungeon.PrintMapToConsole();

    //Camera setup - start at the start room
    Camera camera;
    Room* startRoom = testDungeon.GetStartRoom();
    if (startRoom) {
        Point startCell = startRoom->GetCells()[0]; // Get first cell of start room
        camera.SetRoom(startCell.x, startCell.y);

        // Position player in the center of the start room (in world coordinates)
        int startWorldX = DungeonGridToWorldX(startCell.x);
        int startWorldY = DungeonGridToWorldY(startCell.y);
        // Place player near center of room (ROOM_PIXEL_WIDTH/2, ROOM_PIXEL_HEIGHT/2)
        player = Player(startWorldX + ROOM_PIXEL_WIDTH / 2,
                       startWorldY + ROOM_PIXEL_HEIGHT / 2);
    } else {
        camera.SetRoom(0, 0); // Fallback to (0, 0)
    }

    //Game running
    bool isRunning = true;
    SDL_Event event;
    while(isRunning){
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT) isRunning = false;
            else if(event.type == SDL_KEYDOWN){
                if(event.key.keysym.sym == SDLK_ESCAPE) isRunning = false;

                if(event.key.keysym.sym == SDLK_h) debugMode = !debugMode;

                // Disabled for now - will be used for dungeon regeneration later
                // if(event.key.keysym.sym == SDLK_r){
                //     int pRow = PixelToGrid(player.GetY());
                //     int pCol = PixelToGrid(player.GetX());
                //     currentLevel.Generate(pRow, pCol);
                // }

                player.HandleInput(event, bullets);
            }
        }

    //Updating the objects
    const Uint8* keys = SDL_GetKeyboardState(NULL);
    player.Update(keys, testDungeon);
    for(auto& bullet : bullets){
        bullet.Update(testDungeon, player.GetX(), player.GetY());
    }
    for(auto& enemy : enemies){
        enemy->Update(testDungeon, player.GetX(), player.GetY());
    }

    // Update camera based on player position
    camera.Update(player.GetX(), player.GetY());

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
    // Remove inactive bullets using erase-remove idiom
    bullets.erase(
        std::remove_if(bullets.begin(), bullets.end(),
            [](const Projectile& b) { return !b.GetIsActive(); }),
        bullets.end()
    );
    // Remove dead enemies using erase-remove idiom
    // Note: IsDead() has a bug (returns m_isActive instead of !m_isActive)
    // So we remove when IsDead() returns false (which means actually dead)
    enemies.erase(
        std::remove_if(enemies.begin(), enemies.end(),
            [](const std::unique_ptr<Enemy>& e) { return !e->IsDead(); }),
        enemies.end()
    );

    //Rendering part
    //---------------------------------------------------------------
    SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
    SDL_RenderClear(renderer);

    // Render the current room using camera offsets
    int currentGridX = camera.GetCurrentRoomGridX();
    int currentGridY = camera.GetCurrentRoomGridY();
    RoomLevel* currentRoom = testDungeon.GetRoomLevelAt(currentGridX, currentGridY);
    if (currentRoom) {
        currentRoom->RenderFloors(renderer, currentGridX, currentGridY,
                                  camera.GetOffsetX(), camera.GetOffsetY());
    }

    // Get camera offsets once
    int camOffsetX = camera.GetOffsetX();
    int camOffsetY = camera.GetOffsetY();

    std::vector<RenderObject> renderList;
    player.Render(renderList, camOffsetX, camOffsetY);
    for(auto& enemy : enemies){
        enemy->Render(renderList, camOffsetX, camOffsetY);
    }
    for(auto& bullet : bullets){
        bullet.Render(renderList, camOffsetX, camOffsetY);
    }
    // Old Level rock rendering removed - walls now rendered by RoomLevel

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
        // Get camera offsets for debug rendering
        int camOffsetX = camera.GetOffsetX();
        int camOffsetY = camera.GetOffsetY();

        // Debug hitboxes for walls in current room
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        if (currentRoom) {
            int roomWorldX = DungeonGridToWorldX(currentGridX);
            int roomWorldY = DungeonGridToWorldY(currentGridY);

            for(int row = 0; row < ROOM_TILE_HEIGHT; row++){
                for(int col = 0; col < ROOM_TILE_WIDTH; col++){
                    if(currentRoom->GetTile(row, col) == 1){
                        int worldX = roomWorldX + (col * TILE_SIZE);
                        int worldY = roomWorldY + (row * TILE_SIZE);
                        int screenX = worldX + camOffsetX;
                        int screenY = worldY + camOffsetY;
                        SDL_Rect wallRect = {screenX, screenY, TILE_SIZE, TILE_SIZE};
                        SDL_RenderDrawRect(renderer, &wallRect);
                    }
                }
            }
        }

        // Apply camera offset to enemy hitboxes
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        for(auto& enemy : enemies){
            Collider& col = enemy->GetCollider();
            SDL_Rect enemyRect = {col.box.x + camOffsetX, col.box.y + camOffsetY,
                                  col.box.w, col.box.h};
            SDL_RenderDrawRect(renderer, &enemyRect);
        }

        // Apply camera offset to bullet hitboxes
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        for(auto& bullet : bullets){
            Collider& col = bullet.GetCollider();
            SDL_Rect bulletRect = {col.box.x + camOffsetX, col.box.y + camOffsetY,
                                   col.box.w, col.box.h};
            SDL_RenderDrawRect(renderer, &bulletRect);
        }

        // Apply camera offset to player hitbox
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        Collider& playerCol = player.GetCollider();
        SDL_Rect playerRect = {playerCol.box.x + camOffsetX, playerCol.box.y + camOffsetY,
                               playerCol.box.w, playerCol.box.h};
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