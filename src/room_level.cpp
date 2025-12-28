#include "../include/room_level.h"
#include "../include/dungeon.h"  // For RoomType enum definition and DoorState
#include "../include/TextureManager.h"
#include <ctime>
#include <cstdlib>

RoomLevel::RoomLevel() {
    // Initialize all tiles to floor
    for (int row = 0; row < ROOM_TILE_HEIGHT; row++) {
        for (int col = 0; col < ROOM_TILE_WIDTH; col++) {
            m_mapData[row][col] = 0;
        }
    }

    // No doors by default
    m_hasTopDoor = false;
    m_hasBottomDoor = false;
    m_hasLeftDoor = false;
    m_hasRightDoor = false;
}

RoomLevel::RoomLevel(Room* room, int cellIndex) {
    Generate(room, cellIndex);
}

void RoomLevel::Generate(Room* room, int cellIndex) {
    // Initialize to all floors first
    for (int row = 0; row < ROOM_TILE_HEIGHT; row++) {
        for (int col = 0; col < ROOM_TILE_WIDTH; col++) {
            m_mapData[row][col] = 0;
        }
    }

    m_cellIndex = cellIndex;
    m_isPartOfBigRoom = room->IsMultiCell();

    // Reset door flags
    m_hasTopDoor = false;
    m_hasBottomDoor = false;
    m_hasLeftDoor = false;
    m_hasRightDoor = false;

    // Generate based on room type
    if (!room) {
        GenerateNormalRoom();
    } else {
        switch (room->GetType()) {
            case RoomType::BOSS:
                GenerateBossRoom();
                break;
            case RoomType::TREASURE:
                GenerateTreasureRoom();
                break;
            case RoomType::SECRET:
            case RoomType::SUPER_SECRET:
                GenerateSecretRoom();
                break;
            default:
                GenerateNormalRoom();
                break;
        }
    }
    if(m_isPartOfBigRoom){
        RemoveInternalWalls(room, cellIndex);
    }
}

void RoomLevel::GenerateNormalRoom() {
    // Create walls around the perimeter
    for (int row = 0; row < ROOM_TILE_HEIGHT; row++) {
        for (int col = 0; col < ROOM_TILE_WIDTH; col++) {
            // Top and bottom walls
            if (row == 0 || row == ROOM_TILE_HEIGHT - 1) {
                m_mapData[row][col] = 1;
            }
            // Left and right walls
            else if (col == 0 || col == ROOM_TILE_WIDTH - 1) {
                m_mapData[row][col] = 1;
            }
            // Interior is floor (already set to 0)

            // Add some random obstacles (10% chance)
            else if (std::rand() % 10 == 0) {
                m_mapData[row][col] = 1;
            }
        }
    }
}

void RoomLevel::GenerateBossRoom() {
    // Boss room - completely empty except for perimeter walls
    for (int row = 0; row < ROOM_TILE_HEIGHT; row++) {
        for (int col = 0; col < ROOM_TILE_WIDTH; col++) {
            // Only perimeter walls
            if (row == 0 || row == ROOM_TILE_HEIGHT - 1 ||
                col == 0 || col == ROOM_TILE_WIDTH - 1) {
                m_mapData[row][col] = 1;
            }
            // Everything else is floor
            else {
                m_mapData[row][col] = 0;
            }
        }
    }
}

void RoomLevel::GenerateTreasureRoom() {
    // Treasure room - completely empty except for perimeter walls
    for (int row = 0; row < ROOM_TILE_HEIGHT; row++) {
        for (int col = 0; col < ROOM_TILE_WIDTH; col++) {
            // Only perimeter walls
            if (row == 0 || row == ROOM_TILE_HEIGHT - 1 ||
                col == 0 || col == ROOM_TILE_WIDTH - 1) {
                m_mapData[row][col] = 1;
            }
            // Everything else is floor
            else {
                m_mapData[row][col] = 0;
            }
        }
    }
}

void RoomLevel::GenerateSecretRoom() {
    // Secret room - completely empty except for perimeter walls
    for (int row = 0; row < ROOM_TILE_HEIGHT; row++) {
        for (int col = 0; col < ROOM_TILE_WIDTH; col++) {
            // Only perimeter walls
            if (row == 0 || row == ROOM_TILE_HEIGHT - 1 ||
                col == 0 || col == ROOM_TILE_WIDTH - 1) {
                m_mapData[row][col] = 1;
            }
            // Everything else is floor
            else {
                m_mapData[row][col] = 0;
            }
        }
    }
}

void RoomLevel::PlaceDoors(const DoorState& doors) {
    // Update door flags
    m_hasTopDoor = doors.top;
    m_hasBottomDoor = doors.bottom;
    m_hasLeftDoor = doors.left;
    m_hasRightDoor = doors.right;

    // Door placement positions (middle of each wall)
    int topDoorCol = ROOM_TILE_WIDTH / 2;
    int bottomDoorCol = ROOM_TILE_WIDTH / 2;
    int leftDoorRow = ROOM_TILE_HEIGHT / 2;
    int rightDoorRow = ROOM_TILE_HEIGHT / 2;

    // Place top door (remove wall tile to make it walkable)
    if (doors.top) {
        m_mapData[0][topDoorCol] = 0;
    }

    // Place bottom door
    if (doors.bottom) {
        m_mapData[ROOM_TILE_HEIGHT - 1][bottomDoorCol] = 0;
    }

    // Place left door
    if (doors.left) {
        m_mapData[leftDoorRow][0] = 0;
    }

    // Place right door
    if (doors.right) {
        m_mapData[rightDoorRow][ROOM_TILE_WIDTH - 1] = 0;
    }
}

void RoomLevel::RenderFloors(SDL_Renderer* renderer, int gridX, int gridY, int cameraOffsetX, int cameraOffsetY) {
    TextureManager* tm = TextureManager::GetInstance();

    // Calculate the room's world position based on its grid position
    int roomWorldX = DungeonGridToWorldX(gridX);
    int roomWorldY = DungeonGridToWorldY(gridY);

    for (int row = 0; row < ROOM_TILE_HEIGHT; row++) {
        for (int col = 0; col < ROOM_TILE_WIDTH; col++) {
            // Calculate world position: room's world position + tile offset
            int worldX = roomWorldX + (col * TILE_SIZE);
            int worldY = roomWorldY + (row * TILE_SIZE);

            // Apply camera offset to get screen position
            // screenPos = worldPos + cameraOffset
            int screenX = worldX + cameraOffsetX;
            int screenY = worldY + cameraOffsetY;

            // Draw floor tile for all positions
            tm->Draw("floor", screenX, screenY, TILE_SIZE, TILE_SIZE, renderer);

            // Draw wall/rock if this is a wall tile
            if (m_mapData[row][col] == 1) {
                tm->Draw("rock", screenX, screenY, TILE_SIZE, TILE_SIZE, renderer);
            }

            // Optional: Draw grid outline for debugging
            SDL_Rect tileRect = {screenX, screenY, TILE_SIZE, TILE_SIZE};
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderDrawRect(renderer, &tileRect);
        }
    }
}

int RoomLevel::GetTile(int row, int col) const {
    // Bounds checking
    if (row < 0 || row >= ROOM_TILE_HEIGHT || col < 0 || col >= ROOM_TILE_WIDTH) {
        return 1; // Return wall for out-of-bounds
    }

    return m_mapData[row][col];
}

void RoomLevel::RemoveInternalWalls(Room* room, int cellIndex){
    Point myCell = room->GetCells()[cellIndex];
    const auto& allCells = room->GetCells();

    // Top neighbor - remove top wall (skip corners to preserve outer walls)
    if(CellInRoom({myCell.x, myCell.y - 1}, allCells)){
        for(int col = 1; col < ROOM_TILE_WIDTH - 1; col++){
            m_mapData[0][col] = 0;
        }
    }

    // Bottom neighbor - remove bottom wall (skip corners)
    if(CellInRoom({myCell.x, myCell.y + 1}, allCells)){
        for(int col = 1; col < ROOM_TILE_WIDTH - 1; col++){
            m_mapData[ROOM_TILE_HEIGHT - 1][col] = 0;
        }
    }

    // Left neighbor - remove left wall (skip corners)
    if(CellInRoom({myCell.x - 1, myCell.y}, allCells)){
        for(int row = 1; row < ROOM_TILE_HEIGHT - 1; row++){
            m_mapData[row][0] = 0;
        }
    }

    // Right neighbor - remove right wall (skip corners)
    if(CellInRoom({myCell.x + 1, myCell.y}, allCells)){
        for(int row = 1; row < ROOM_TILE_HEIGHT - 1; row++){
            m_mapData[row][ROOM_TILE_WIDTH - 1] = 0;
        }
    }
}

bool RoomLevel::CellInRoom(Point cell, const std::vector<Point>& roomCells){
    for(const auto& c : roomCells){
        if(c == cell) return true;
    }
    return false;
}