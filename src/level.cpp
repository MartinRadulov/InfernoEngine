#include "../include/level.h" // Go up one folder, then into include
#include <ctime>
#include <cstdlib>

Level::Level() {
    // When the level is created, generate it immediately
    Generate(2, 2);
}

void Level::Generate(int pRow, int pCol) {
    std::srand(std::time(nullptr));

    for (int row = 0; row < MAP_ROWS; row++) {
        for (int col = 0; col < MAP_COLS; col++) {
            
            // Edge Walls
            if (row == 0 || row == MAP_ROWS - 1 || col == 0 || col == MAP_COLS - 1) {
                m_mapData[row][col] = 1;
            }
            

            int distRow = std::abs(row - pRow);
            int distCol = std::abs(col - pCol);
            if(distRow <= 2 && distCol <= 2){
                m_mapData[row][col] = 0;
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

void Level::RenderFloors(SDL_Renderer* renderer) {
    TextureManager* tm = TextureManager::GetInstance();
    for (int row = 0; row < MAP_ROWS; row++) {
        for (int col = 0; col < MAP_COLS; col++) {
            int xPos = col * TILE_SIZE;
            int yPos = row * TILE_SIZE;

            tm->Draw("floor", xPos, yPos, TILE_SIZE, TILE_SIZE, renderer);

            //Grid outline
            SDL_Rect tileRect = {xPos, yPos, TILE_SIZE, TILE_SIZE};
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderDrawRect(renderer, &tileRect);
        }
    }

}

int Level::GetTile(int row, int col) {
    if(row < 0 || col < 0) return 1;

    if(row >= MAP_ROWS || col >= MAP_COLS) return 1;

    return m_mapData[row][col];
}