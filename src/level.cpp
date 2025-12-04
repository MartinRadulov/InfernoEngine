#include "../include/level.h" // Go up one folder, then into include
#include <ctime>
#include <cstdlib>

Level::Level() {
    // When the level is created, generate it immediately
    Generate();
}

void Level::Generate() {
    std::srand(std::time(nullptr));

    for (int row = 0; row < MAP_ROWS; row++) {
        for (int col = 0; col < MAP_COLS; col++) {
            
            // Edge Walls
            if (row == 0 || row == MAP_ROWS - 1 || col == 0 || col == MAP_COLS - 1) {
                m_mapData[row][col] = 1;
            } 
            // Random Rocks inside
            else {
                if (std::rand() % 10 == 0) {
                     m_mapData[row][col] = 1; 
                } else {
                     m_mapData[row][col] = 0; 
                }
            }
        }
    }
}

void Level::Render(SDL_Renderer* renderer) {
    for (int row = 0; row < MAP_ROWS; row++) {
        for (int col = 0; col < MAP_COLS; col++) {
            
            // Math: Calculate pixel position
            SDL_Rect tileRect = { 
                col * TILE_SIZE, 
                row * TILE_SIZE, 
                TILE_SIZE, 
                TILE_SIZE 
            };

            // Color logic
            if (m_mapData[row][col] == 1) {
                SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255); // Wall
            } else {
                SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255); // Floor
            }

            // Draw
            SDL_RenderFillRect(renderer, &tileRect);
            
            // Grid Outline
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); 
            SDL_RenderDrawRect(renderer, &tileRect);
        }
    }
}

int Level::GetTile(int row, int col) {
    // Safety check: Don't read outside the array!
    if (row >= 0 && row < MAP_ROWS && col >= 0 && col < MAP_COLS) {
        return m_mapData[row][col];
    }
    return 1; // If out of bounds, treat it as a wall
}