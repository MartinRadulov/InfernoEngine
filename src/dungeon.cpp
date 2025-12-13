#include "../include/dungeon.h"
#include <cstdlib>
#include <ctime>

Dungeon::Dungeon(){

}

void Dungeon::GenerateDungeon(int maxRooms){
    for(int y = 0; y < DUNGEON_SIZE; y++){
        for(int x = 0; x < DUNGEON_SIZE; x++){
            m_grid[y][x] = RoomData();
        }
    }

    int currX = DUNGEON_SIZE / 2;
    int currY = DUNGEON_SIZE / 2;
    int roomCount = 0;
    m_grid[currY][currX].active = true;
    m_grid[currY][currX].type = RoomType::START;
    roomCount++;

    while(roomCount < maxRooms){
        int dir = std::rand() % 4;

        int tempX = currX;
        int tempY = currY;

        if(dir == 0) tempY--; //Up
        if(dir == 1) tempY++; //Down
        if(dir == 2) tempX--; //Left
        if(dir == 3) tempX++; //Right

        if(tempX > 0 && tempX < DUNGEON_SIZE - 1 && tempY > 0 && tempY < DUNGEON_SIZE - 1){
            currX = tempX;
            currY = tempY;
        }

        if(!m_grid[currY][currX].active){
            m_grid[currX][currY].active = true;
            m_grid[currX][currY].type = RoomType::NORMAL;
            roomCount++;
        }
    }


    for(int y = 0; y < DUNGEON_SIZE; y++){
        for(int x = 0; x < DUNGEON_SIZE; x++){
            if(m_grid[y][x].active){
                if(RoomExists(x, y - 1)) m_grid[y][x].doorTop = true;
                if(RoomExists(x, y + 1)) m_grid[y][x].doorBottom = true;
                if(RoomExists(x - 1, y)) m_grid[y][x].doorLeft = true;
                if(RoomExists(x + 1, y)) m_grid[y][x].doorRight = true;
            }
        }
    }

    PlaceSpecialRoom(RoomType::BOSS);
    PlaceSpecialRoom(RoomType::TREASURE);
}

bool Dungeon::RoomExists(int x, int y){
    if(x < 0 || x >= DUNGEON_SIZE || y < 0 || y >= DUNGEON_SIZE) return false;
    return m_grid[y][x].active;
}

int Dungeon::CountNeighbours(int x, int y){
    int count = 0;
    if(RoomExists(x, y - 1)) count++;
    if(RoomExists(x, y + 1)) count++;
    if(RoomExists(x - 1, y)) count++;
    if(RoomExists(x + 1, y)) count++;
    return count;
}

void Dungeon::PrintMapToConsole() {
    for(int y = 0; y < DUNGEON_SIZE; y++) {
        for(int x = 0; x < DUNGEON_SIZE; x++) {
            if (!m_grid[y][x].active) {
                std::cout << ". "; // Empty
            } else {
                if (m_grid[y][x].type == RoomType::START) std::cout << "S ";
                else if (m_grid[y][x].type == RoomType::BOSS) std::cout << "B ";
                else if (m_grid[y][x].type == RoomType::TREASURE) std::cout << "T ";
                else std::cout << "# "; // Normal
            }
        }
        std::cout << std::endl;
    }
}

bool Dungeon::IsValidEmptySpot(int x, int y){
    if (x < 1 || x >= DUNGEON_SIZE - 1 || y < 1 || y >= DUNGEON_SIZE - 1) {
        return false;
    }
    return !m_grid[y][x].active;
}

void Dungeon::PlaceSpecialRoom(RoomType type){
    std::vector<RoomCoordinate> candidates;
    int startX = DUNGEON_SIZE / 2;
    int startY = DUNGEON_SIZE / 2;

    for(int y = 0; y < DUNGEON_SIZE; y++) {
        for(int x = 0; x < DUNGEON_SIZE; x++) {
            if (m_grid[y][x].active && m_grid[y][x].type == RoomType::NORMAL) {
                int dist = std::abs(x - startX) + std::abs(y - startY);
                candidates.push_back({x, y, dist});
            }
        }
    }

    for (size_t i = 0; i < candidates.size(); i++) {
        for (size_t j = i + 1; j < candidates.size(); j++) {
            if (candidates[j].distance > candidates[i].distance) {
                RoomCoordinate temp = candidates[i];
                candidates[i] = candidates[j];
                candidates[j] = temp;
            }
        }
    }

    bool placed = false;
    for(const auto& candidate : candidates){
        int cx = candidate.x;
        int cy = candidate.y;

        if(CountNeighbours(cx, cy) == 1){
            m_grid[cy][cx].type = type;
            placed = true;
            break;
        }

    }

    if (!placed) {
        for (const auto& candidate : candidates) {
            int cx = candidate.x;
            int cy = candidate.y;

            // Check Neighbors: Up, Down, Left, Right
            // If we find an empty spot, Create the Room there.
            
            // Try UP
            if (IsValidEmptySpot(cx, cy - 1)) {
                m_grid[cy - 1][cx].active = true;
                m_grid[cy - 1][cx].type = type;
                m_grid[cy - 1][cx].doorBottom = true; // Connect to parent
                m_grid[cy][cx].doorTop = true;        // Connect parent to me
                placed = true;
                break;
            }
            // Try DOWN
            if (IsValidEmptySpot(cx, cy + 1)) {
                m_grid[cy + 1][cx].active = true;
                m_grid[cy + 1][cx].type = type;
                m_grid[cy + 1][cx].doorTop = true;
                m_grid[cy][cx].doorBottom = true;
                placed = true;
                break;
            }
            // Try LEFT
            if (IsValidEmptySpot(cx - 1, cy)) {
                m_grid[cy][cx - 1].active = true;
                m_grid[cy][cx - 1].type = type;
                m_grid[cy][cx - 1].doorRight = true;
                m_grid[cy][cx].doorLeft = true;
                placed = true;
                break;
            }
            // Try RIGHT
            if (IsValidEmptySpot(cx + 1, cy)) {
                m_grid[cy][cx + 1].active = true;
                m_grid[cy][cx + 1].type = type;
                m_grid[cy][cx + 1].doorLeft = true;
                m_grid[cy][cx].doorRight = true;
                placed = true;
                break;
            }
        }
    }
}