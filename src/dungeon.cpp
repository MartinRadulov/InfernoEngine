#include "../include/dungeon.h"
#include <cstdlib>
#include <ctime>
#include <algorithm>

Dungeon::Dungeon(){
    static bool seeded = false;
    if(!seeded){
        std::srand(static_cast<unsigned>(std::time(nullptr)));
        seeded = true;
    }
}

void Dungeon::GenerateDungeon(int maxRooms){
    ClearGrid();

    GenerateMainPath(maxRooms);

    UpdateNormalDoors();

    PlaceSpecialRoom(RoomType::BOSS);
    PlaceSpecialRoom(RoomType::SHOP);
    PlaceSpecialRoom(RoomType::TREASURE);

    //UpdateAllDoors();
}

bool Dungeon::RoomExists(int x, int y){
    if(x < 0 || x >= DUNGEON_SIZE || y < 0 || y >= DUNGEON_SIZE) return false;
    return m_grid[y][x].active;
}

int Dungeon::CountNeighbours(Point p){
    int count = 0;
    if(RoomExists(p.x, p.y - 1)) count++;
    if(RoomExists(p.x, p.y + 1)) count++;
    if(RoomExists(p.x - 1, p.y)) count++;
    if(RoomExists(p.x + 1, p.y)) count++;
    return count;
}

void Dungeon::PrintMapToConsole() {
    for (int y = 0; y < DUNGEON_SIZE; y++) {
        // Row 1: Top doors
        for (int x = 0; x < DUNGEON_SIZE; x++) {
            std::cout << "   ";
            if (m_grid[y][x].active && m_grid[y][x].doorTop) 
                std::cout << "|";
            else 
                std::cout << " ";
            std::cout << "   ";
        }
        std::cout << "\n";
        
        // Row 2: Room with left/right doors
        for (int x = 0; x < DUNGEON_SIZE; x++) {
            // Left door
            if (m_grid[y][x].active && m_grid[y][x].doorLeft) 
                std::cout << " - ";
            else 
                std::cout << "   ";
            
            // Room symbol
            if (!m_grid[y][x].active) {
                std::cout << ".";
            } else {
                switch (m_grid[y][x].type) {
                    case RoomType::START:    std::cout << "S"; break;
                    case RoomType::BOSS:     std::cout << "B"; break;
                    case RoomType::TREASURE: std::cout << "T"; break;
                    case RoomType::SHOP:     std::cout << "M"; break;
                    default:                 std::cout << "#"; break;
                }
            }
            
            // Right door
            if (m_grid[y][x].active && m_grid[y][x].doorRight) 
                std::cout << " - ";
            else 
                std::cout << "   ";
        }
        std::cout << "\n";
    }
}

void Dungeon::PlaceSpecialRoom(RoomType type) {
    CalculateStepDistance();

    std::vector<Point> candidates;

    for(int y = 0; y < DUNGEON_SIZE; y++){
        for(int x = 0; x < DUNGEON_SIZE; x++){
            if(m_grid[y][x].active && m_grid[y][x].type == RoomType::NORMAL &&
                m_grid[y][x].stepDistance >= 0){
                candidates.push_back({x, y});
            }
        }
    }

    std::sort(candidates.begin(), candidates.end(),
        [this](const Point& a, const Point& b){
            return m_grid[a.y][a.x].stepDistance > m_grid[b.y][b.x].stepDistance;
        });

    for(const auto& candidate : candidates){
        if(CountNeighbours(candidate) == 1 && !HasConflictingNeighbour(candidate)){
            m_grid[candidate.y][candidate.x].type = type;
            return;
        }
    }

    for(const auto& candidate : candidates){
        if(HasConflictingNeighbour(candidate)) continue;

        std::vector<Point> emptyNeighbors = GetValidEmptyNeighbors(candidate);
        for(const auto& spot : emptyNeighbors){
            int newRoomNeighborCount = CountNeighbours(spot);
            if(newRoomNeighborCount == 1 && !HasConflictingNeighbour(spot)){
                m_grid[spot.y][spot.x].active = true;
                m_grid[spot.y][spot.x].type = type;

                ConnectRooms(spot, candidate);

                return;
            }
        }
    }

    std::cout << "Warning: Could not place " << 
        (type == RoomType::BOSS ? "BOSS" : 
         type == RoomType::TREASURE ? "TREASURE" : "SHOP") 
        << " room!" << std::endl;
}

void Dungeon::CalculateStepDistance(){
    for(int y = 0; y < DUNGEON_SIZE; y++){
        for(int x = 0; x < DUNGEON_SIZE; x++){
            m_grid[y][x].stepDistance = -1;
        }
    }

    Point start = {DUNGEON_SIZE / 2, DUNGEON_SIZE / 2};

    std::queue<Point> q;
    m_grid[start.y][start.x].stepDistance = 0;
    q.push(start);

    while(!q.empty()){
        Point current = q.front();
        q.pop();

        int dist = m_grid[current.y][current.x].stepDistance;

        struct {int dx, dy; bool RoomData::*door;} directions[4] = {
            {0 , -1, &RoomData::doorTop},
            {0 , 1, &RoomData::doorBottom},
            {-1 , 0, &RoomData::doorLeft},
            {1 , 0, &RoomData::doorRight}
        };

        for(const auto& dir : directions){
            int nx = current.x + dir.dx;
            int ny = current.y + dir.dy;

            if(nx < 0 || nx >= DUNGEON_SIZE || ny < 0 || ny >= DUNGEON_SIZE) continue;

            if(m_grid[current.y][current.x].*dir.door && m_grid[ny][nx].stepDistance == -1){
                m_grid[ny][nx].stepDistance = dist + 1;
                q.push({nx, ny});
            }
        }
    }
}

void Dungeon::UpdateNormalDoors() {
    for(int y = 0; y < DUNGEON_SIZE; y++) {
        for(int x = 0; x < DUNGEON_SIZE; x++) {
            // IGNORE SPECIAL ROOMS! They are static.
            if (m_grid[y][x].active && 
               (m_grid[y][x].type == RoomType::NORMAL || m_grid[y][x].type == RoomType::START)) {
                
                // Only connect to other Normal/Start rooms automatically
                // This prevents accidental connections to existing special rooms
                auto IsConnectable = [&](int nx, int ny) {
                    if (!RoomExists(nx, ny)) return false;
                    RoomType t = m_grid[ny][nx].type;
                    return (t == RoomType::NORMAL || t == RoomType::START);
                };

                m_grid[y][x].doorTop    = IsConnectable(x, y - 1);
                m_grid[y][x].doorBottom = IsConnectable(x, y + 1);
                m_grid[y][x].doorLeft   = IsConnectable(x - 1, y);
                m_grid[y][x].doorRight  = IsConnectable(x + 1, y);
            }
        }
    }  
}

bool Dungeon::HasConflictingNeighbour(Point p) {
    auto isConflict = [this](int x, int y) {
        if (!RoomExists(x, y)) return false;
        RoomType t = m_grid[y][x].type;
        return (t == RoomType::BOSS || t == RoomType::TREASURE || t == RoomType::SHOP);
    };

    return isConflict(p.x, p.y - 1) || isConflict(p.x, p.y + 1) ||
        isConflict(p.x - 1, p.y) || isConflict(p.x + 1, p.y);
}

void Dungeon::ClearGrid(){
    for(int y = 0; y < DUNGEON_SIZE; y++){
        for(int x = 0; x < DUNGEON_SIZE; x++){
            m_grid[y][x] = RoomData();
        }
    }
}

void Dungeon::GenerateMainPath(int maxRooms){
    Point pos = {DUNGEON_SIZE / 2, DUNGEON_SIZE / 2};

    m_grid[pos.y][pos.x].active = true;
    m_grid[pos.y][pos.x].type = RoomType::START;

    int roomCount = 1;
    std::vector<Point> frontier;
    frontier.push_back(pos);

    while(roomCount < maxRooms && !frontier.empty()){
        int idx = std::rand() % frontier.size();
        Point current = frontier[idx];

        std::vector<Point> neighbors = GetValidEmptyNeighbors(current);

        if(neighbors.empty()){
            frontier.erase(frontier.begin() + idx);
            continue;
        }

        Point newRoom = neighbors[std::rand() % neighbors.size()];
        m_grid[newRoom.y][newRoom.x].active = true;
        m_grid[newRoom.y][newRoom.x].type = RoomType::NORMAL;
        roomCount++;

        frontier.push_back(newRoom);

        if(std::rand() % 100 < 30){
            frontier.erase(frontier.begin() + idx);
        }
    }
}

std::vector<Dungeon::Point> Dungeon::GetValidEmptyNeighbors(Point p){
    std::vector<Point> neighbors;

    const Point directions[4] = {
        {0, -1},
        {0, 1},
        {-1, 0},
        {1, 0}
    };

    for(const auto& dir : directions){
        Point neighbor = {p.x + dir.x, p.y + dir.y};

        if(neighbor.x < 1 || neighbor.x >= DUNGEON_SIZE - 1 ||
            neighbor.y < 1 || neighbor.y >= DUNGEON_SIZE - 1){
            continue;
        }

        if(!m_grid[neighbor.y][neighbor.x].active){
            neighbors.push_back(neighbor);
        }
    }

    return neighbors;
}

RoomData Dungeon::GetRoom(int x, int y){
    if(x < 0 || x >= DUNGEON_SIZE || y < 0 || y >= DUNGEON_SIZE) return RoomData();
    return m_grid[y][x];
}

void Dungeon::ConnectRooms(Point a, Point b){
    // A is Below B (A connects Top, B connects Bottom)
    if (a.y > b.y) { m_grid[a.y][a.x].doorTop = true;    m_grid[b.y][b.x].doorBottom = true; }
    // A is Above B
    if (a.y < b.y) { m_grid[a.y][a.x].doorBottom = true; m_grid[b.y][b.x].doorTop = true; }
    // A is Right of B
    if (a.x > b.x) { m_grid[a.y][a.x].doorLeft = true;   m_grid[b.y][b.x].doorRight = true; }
    // A is Left of B
    if (a.x < b.x) { m_grid[a.y][a.x].doorRight = true;  m_grid[b.y][b.x].doorLeft = true; }
}