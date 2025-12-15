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

    GenerateComplexRooms();

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

// int Dungeon::CountNeighbours(Point p){
//     std::set<int> uniqueNeighbors;
    
//     // Check 4 directions
//     Point dirs[4] = {{0,-1}, {0,1}, {-1,0}, {1,0}};
//     for(auto& d : dirs) {
//         if(RoomExists(p.x + d.x, p.y + d.y)) {
//             uniqueNeighbors.insert(m_grid[p.y + d.y][p.x + d.x].id);
//         }
//     }
    
//     return uniqueNeighbors.size();
// }

void Dungeon::PrintMapToConsole() {
    // 1. Configuration
    const int CELL_W = 4; // Width (e.g., "+---")
    const int CELL_H = 2; // Height (e.g., "|   ")
    
    int canvasW = DUNGEON_SIZE * CELL_W + 1;
    int canvasH = DUNGEON_SIZE * CELL_H + 1;

    // 2. Create Buffer (filled with empty spaces)
    std::vector<std::string> canvas(canvasH, std::string(canvasW, ' '));

    // 3. Draw the Map
    for (int y = 0; y < DUNGEON_SIZE; y++) {
        for (int x = 0; x < DUNGEON_SIZE; x++) {
            
            // --- DRAW CORNERS (+) ---
            if (m_grid[y][x].active) {
                int py = y * CELL_H;
                int px = x * CELL_W;
                int id = m_grid[y][x].id;

                // Helper to check if a neighbor has the same ID
                auto SameID = [&](int ny, int nx) {
                    if (ny < 0 || ny >= DUNGEON_SIZE || nx < 0 || nx >= DUNGEON_SIZE) return false;
                    return m_grid[ny][nx].active && m_grid[ny][nx].id == id;
                };

                // Top-Left Corner (Check Left and Up)
                if (! (SameID(y, x-1) && SameID(y-1, x) && SameID(y-1, x-1)) )
                    canvas[py][px] = '+';             

                // Top-Right Corner (Check Right and Up)
                if (! (SameID(y, x+1) && SameID(y-1, x) && SameID(y-1, x+1)) )
                    canvas[py][px + CELL_W] = '+';    

                // Bottom-Left Corner (Check Left and Down)
                if (! (SameID(y, x-1) && SameID(y+1, x) && SameID(y+1, x-1)) )
                    canvas[py + CELL_H][px] = '+';    

                // Bottom-Right Corner (Check Right and Down)
                if (! (SameID(y, x+1) && SameID(y+1, x) && SameID(y+1, x+1)) )
                    canvas[py + CELL_H][px + CELL_W] = '+'; 
            }

            // --- DRAW TOP WALLS (---) or DOORS (.) ---
            int topY = y * CELL_H;
            int topX_Start = x * CELL_W + 1;

            bool currActive = m_grid[y][x].active;
            bool upActive = (y > 0) ? m_grid[y - 1][x].active : false;
            
            bool isSameRoom = (y > 0 && currActive && upActive && m_grid[y][x].id == m_grid[y-1][x].id);
            bool isDoor = (currActive && m_grid[y][x].doorTop) || (upActive && m_grid[y-1][x].doorBottom);

            if (!currActive && !upActive) {
                // Empty void: do nothing
            } else if (isSameRoom) {
                // BIG ROOM MERGE: Leave as ' ' to look clean
            } else if (isDoor) {
                // DOOR: Draw a dot so we can see the connection!
                canvas[topY][topX_Start + 1] = '.'; 
            } else {
                // WALL: Draw '---'
                for(int k=0; k<3; k++) canvas[topY][topX_Start + k] = '-';
            }

            // --- DRAW LEFT WALLS (|) or DOORS (.) ---
            int midY = y * CELL_H + 1;
            int leftX = x * CELL_W;

            bool leftActive = (x > 0) ? m_grid[y][x - 1].active : false;
            
            isSameRoom = (x > 0 && currActive && leftActive && m_grid[y][x].id == m_grid[y][x-1].id);
            isDoor = (currActive && m_grid[y][x].doorLeft) || (leftActive && m_grid[y][x-1].doorRight);

            if (!currActive && !leftActive) {
                // Empty void
            } else if (isSameRoom) {
                // BIG ROOM MERGE: Leave as ' '
            } else if (isDoor) {
                // DOOR: Draw a dot
                canvas[midY][leftX] = '.';
            } else {
                // WALL: Draw '|'
                canvas[midY][leftX] = '|';
            }

            // --- DRAW CENTER SYMBOL ---
            int centerY = y * CELL_H + 1;
            int centerX = x * CELL_W + 2;

            if (m_grid[y][x].active) {
                char symbol = '#';
                switch (m_grid[y][x].type) {
                    case RoomType::START:    symbol = 'S'; break;
                    case RoomType::BOSS:     symbol = 'B'; break;
                    case RoomType::TREASURE: symbol = 'T'; break;
                    case RoomType::SHOP:     symbol = '$'; break;
                    case RoomType::NORMAL:   symbol = '#'; break;
                    default:                 symbol = '?'; break;
                }
                canvas[centerY][centerX] = symbol;
            } else {
                canvas[centerY][centerX] = '.'; // Faint grid background
            }
        }
    }

    // 4. Print the Buffer
    for (const auto& line : canvas) {
        std::cout << line << "\n";
    }
}

void Dungeon::PlaceSpecialRoom(RoomType type) {
    CalculateStepDistance();

    std::vector<Point> candidates;

    for(int y = 0; y < DUNGEON_SIZE; y++){
        for(int x = 0; x < DUNGEON_SIZE; x++){
            if(m_grid[y][x].active && m_grid[y][x].type == RoomType::NORMAL && m_grid[y][x].stepDistance >= 0){
                candidates.push_back({x, y});
            }
        }
    }

    std::sort(candidates.begin(), candidates.end(),
        [this](const Point& a, const Point& b){
            return m_grid[a.y][a.x].stepDistance > m_grid[b.y][b.x].stepDistance;
        });

    // STRATEGY 2: Create new branch
    for(const auto& candidate : candidates){
        if(HasConflictingNeighbour(candidate)) continue;

        std::vector<Point> emptyNeighbors = GetValidEmptyNeighbors(candidate);
        for(const auto& spot : emptyNeighbors){
            int newRoomNeighborCount = CountNeighbours(spot);
            if(newRoomNeighborCount == 1 && !HasConflictingNeighbour(spot)){
                m_grid[spot.y][spot.x].active = true;
                m_grid[spot.y][spot.x].type = type;
                m_grid[spot.y][spot.x].shape = RoomShape::Dim1x1;
                m_grid[spot.y][spot.x].id = m_lastRoomID++;

                ConnectRooms(spot, candidate);
                return;
            }
        }
    }

    for(const auto& candidate : candidates){
        if(m_grid[candidate.y][candidate.x].shape != RoomShape::Dim1x1) continue;
        
        if(CountNeighbours(candidate) == 1 && !HasConflictingNeighbour(candidate)){
            m_grid[candidate.y][candidate.x].type = type;
            return;
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

    int startRoomID = m_grid[start.y][start.x].id;
    for(int y = 0; y < DUNGEON_SIZE; y++){
        for(int x = 0; x < DUNGEON_SIZE; x++){
            if(m_grid[y][x].active && m_grid[y][x].id == startRoomID){
                m_grid[y][x].stepDistance = 0;
                q.push({x, y});
            }
        }
    }

    std::set<int> processedRooms;
    processedRooms.insert(startRoomID);

    while(!q.empty()){
        Point current = q.front();
        q.pop();

        int dist = m_grid[current.y][current.x].stepDistance;
        int currentID = m_grid[current.y][current.x].id;

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
            if(!m_grid[ny][nx].active) continue;

            int neighborID = m_grid[ny][nx].id;
            if(neighborID == currentID) continue;
            if(processedRooms.find(neighborID) != processedRooms.end()) continue;

            if(m_grid[current.y][current.x].*dir.door){
                // Mark entire neighbor room with distance + 1
                int newDist = dist + 1;
                
                for(int y = 0; y < DUNGEON_SIZE; y++){
                    for(int x = 0; x < DUNGEON_SIZE; x++){
                        if(m_grid[y][x].active && m_grid[y][x].id == neighborID){
                            m_grid[y][x].stepDistance = newDist;
                            q.push({x, y});
                        }
                    }
                }
                
                processedRooms.insert(neighborID);
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
                auto IsConnectable = [&](int nx, int ny, bool isHorizontal) {
                    if (!RoomExists(nx, ny)) return false;
                    
                    RoomData& current = m_grid[y][x];
                    RoomData& neighbor = m_grid[ny][nx];

                    if(current.id == neighbor.id) return true;

                    if(current.shape == RoomShape::Dim1x2 && isHorizontal) return false;
                    if(current.shape == RoomShape::Dim2x1 && !isHorizontal) return false;
                    if(neighbor.shape == RoomShape::Dim2x1 && !isHorizontal) return false;
                    if(neighbor.shape == RoomShape::Dim1x2 && isHorizontal) return false;

                    return true;
                };

                m_grid[y][x].doorTop    = IsConnectable(x, y - 1, false);
                m_grid[y][x].doorBottom = IsConnectable(x, y + 1, false);
                m_grid[y][x].doorLeft   = IsConnectable(x - 1, y, true);
                m_grid[y][x].doorRight  = IsConnectable(x + 1, y, true);
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

    m_lastRoomID = 1;
    m_grid[pos.y][pos.x].id = m_lastRoomID++;
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
        m_grid[newRoom.y][newRoom.x].shape = RoomShape::Dim1x1;
        m_grid[newRoom.y][newRoom.x].id = m_lastRoomID++;
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

void Dungeon::GenerateComplexRooms(){
    std::vector<Point> activeRooms;
    for(int y = 0; y < DUNGEON_SIZE; y++){
        for(int x = 0; x < DUNGEON_SIZE; x++){
            if(m_grid[y][x].active && m_grid[y][x].type == RoomType::NORMAL){
                activeRooms.push_back({x, y});
            }
        }
    }

    std::random_shuffle(activeRooms.begin(), activeRooms.end());

    for(Point room : activeRooms){
        if(m_grid[room.y][room.x].shape != RoomShape::Dim1x1) continue;

        int roll = std::rand() % 100;


        //10%
        if(roll < 10){
            if(TryCreateBigRoom(room.x, room.y)) continue;
        }

        //25%
        if(roll < 25){
            if(TryCreateLRoom(room.x, room.y)) continue;
        }

        //40%
        if(roll < 40){
            if(TryCreateSkinnyRoom(room.x, room.y)) continue;
        }
    }
}

bool Dungeon::IsFree(int x, int y){
    return (x >= 0 && x < DUNGEON_SIZE && y >= 0 && y < DUNGEON_SIZE && !m_grid[y][x].active);
}

bool Dungeon::TryCreateBigRoom(int x, int y){
    if(IsFree(x + 1, y) && IsFree(x, y + 1) && IsFree(x + 1, y + 1)){
        int id = m_grid[y][x].id;

        Point rooms[] = {{x, y}, {x + 1, y}, {x, y + 1}, {x + 1, y + 1}};
        for(Point room : rooms){
            m_grid[room.y][room.x].active = true;
            m_grid[room.y][room.x].type = RoomType::NORMAL;
            m_grid[room.y][room.x].id = id;
            m_grid[room.y][room.x].shape = RoomShape::Dim2x2;
        }
        return true;
    }
    else if(IsFree(x - 1, y) && IsFree(x, y + 1) && IsFree(x - 1, y + 1)){
        int id = m_grid[y][x].id;

        Point rooms[] = {{x, y}, {x - 1, y}, {x, y + 1}, {x - 1, y + 1}};
        for(Point room : rooms){
            m_grid[room.y][room.x].active = true;
            m_grid[room.y][room.x].type = RoomType::NORMAL;
            m_grid[room.y][room.x].id = id;
            m_grid[room.y][room.x].shape = RoomShape::Dim2x2;
        }
        return true;
    }
    else if(IsFree(x - 1, y) && IsFree(x, y - 1) && IsFree(x - 1, y - 1)){
        int id = m_grid[y][x].id;

        Point rooms[] = {{x, y}, {x - 1, y}, {x, y - 1}, {x - 1, y - 1}};
        for(Point room : rooms){
            m_grid[room.y][room.x].active = true;
            m_grid[room.y][room.x].type = RoomType::NORMAL;
            m_grid[room.y][room.x].id = id;
            m_grid[room.y][room.x].shape = RoomShape::Dim2x2;
        }
        return true;
    }
    else if(IsFree(x + 1, y) && IsFree(x, y - 1) && IsFree(x + 1, y - 1)){
        int id = m_grid[y][x].id;

        Point rooms[] = {{x, y}, {x + 1, y}, {x, y - 1}, {x + 1, y - 1}};
        for(Point room : rooms){
            m_grid[room.y][room.x].active = true;
            m_grid[room.y][room.x].type = RoomType::NORMAL;
            m_grid[room.y][room.x].id = id;
            m_grid[room.y][room.x].shape = RoomShape::Dim2x2;
        }
        return true;
    }
    return false;
}

bool Dungeon::TryCreateSkinnyRoom(int x, int y) {
    // Try Horizontal (2x1) -> Forbidden connections are Top (y-1) and Bottom (y+1)
    // We only create this if we DON'T need to connect Up or Down from the main cell or the expansion cell.
    if(std::rand() % 2 == 0) {
        bool needsVerticalConnection = RoomExists(x, y - 1) || RoomExists(x, y + 1);
        
        // Expand Right
        if(IsFree(x + 1, y)) {
            bool expansionNeedsVertical = RoomExists(x + 1, y - 1) || RoomExists(x + 1, y + 1);
            if(needsVerticalConnection || expansionNeedsVertical) return false; // ABORT to save the path

            int id = m_grid[y][x].id;
            m_grid[y][x].shape = RoomShape::Dim2x1;
            
            m_grid[y][x + 1].active = true;
            m_grid[y][x + 1].type = RoomType::NORMAL;
            m_grid[y][x + 1].id = id;
            m_grid[y][x + 1].shape = RoomShape::Dim2x1;
            return true;
        }
        // Expand Left
        else if(IsFree(x - 1, y)) {
            bool expansionNeedsVertical = RoomExists(x - 1, y - 1) || RoomExists(x - 1, y + 1);
            if(needsVerticalConnection || expansionNeedsVertical) return false; 

            int id = m_grid[y][x].id;
            m_grid[y][x].shape = RoomShape::Dim2x1;
            
            m_grid[y][x - 1].active = true;
            m_grid[y][x - 1].type = RoomType::NORMAL;
            m_grid[y][x - 1].id = id;
            m_grid[y][x - 1].shape = RoomShape::Dim2x1;
            return true;
        }
    }
    // Try Vertical (1x2) -> Forbidden connections are Left (x-1) and Right (x+1)
    else {
        bool needsHorizontalConnection = RoomExists(x - 1, y) || RoomExists(x + 1, y);

        // Expand Down
        if(IsFree(x, y + 1)) {
            bool expansionNeedsHorizontal = RoomExists(x - 1, y + 1) || RoomExists(x + 1, y + 1);
            if(needsHorizontalConnection || expansionNeedsHorizontal) return false;

            int id = m_grid[y][x].id;
            m_grid[y][x].shape = RoomShape::Dim1x2;
            
            m_grid[y + 1][x].active = true;
            m_grid[y + 1][x].type = RoomType::NORMAL;
            m_grid[y + 1][x].id = id;
            m_grid[y + 1][x].shape = RoomShape::Dim1x2;
            return true;
        }
        // Expand Up
        if(IsFree(x, y - 1)) {
            bool expansionNeedsHorizontal = RoomExists(x - 1, y - 1) || RoomExists(x + 1, y - 1);
            if(needsHorizontalConnection || expansionNeedsHorizontal) return false;

            int id = m_grid[y][x].id;
            m_grid[y][x].shape = RoomShape::Dim1x2;
            
            m_grid[y - 1][x].active = true;
            m_grid[y - 1][x].type = RoomType::NORMAL;
            m_grid[y - 1][x].id = id;
            m_grid[y - 1][x].shape = RoomShape::Dim1x2;
            return true;
        }
    }
    return false;
}

bool Dungeon::TryCreateLRoom(int x, int y) {
    if(IsFree(x + 1, y) && IsFree(x, y + 1) && IsFree(x + 1, y + 1)) {
        int skip = std::rand() % 3; // 0=Right, 1=Bottom, 2=Diagonal
        
        int id = m_grid[y][x].id;
        m_grid[y][x].shape = RoomShape::LShape;

        if(skip != 0) { // Fill Right
            m_grid[y][x+1] = m_grid[y][x]; 
            m_grid[y][x+1].id = id;
        }
        if(skip != 1) { // Fill Bottom
            m_grid[y+1][x] = m_grid[y][x]; 
            m_grid[y+1][x].id = id;
        }
        if(skip != 2) { // Fill Diagonal
            m_grid[y+1][x+1] = m_grid[y][x]; 
            m_grid[y+1][x+1].id = id;
        }
        return true;
    }
    else if(IsFree(x - 1, y) && IsFree(x, y + 1) && IsFree(x - 1, y + 1)) {
        int skip = std::rand() % 3; // 0=Right, 1=Bottom, 2=Diagonal
        
        int id = m_grid[y][x].id;
        m_grid[y][x].shape = RoomShape::LShape;

        if(skip != 0) { // Fill Right
            m_grid[y][x-1] = m_grid[y][x]; 
            m_grid[y][x-1].id = id;
        }
        if(skip != 1) { // Fill Bottom
            m_grid[y+1][x] = m_grid[y][x]; 
            m_grid[y+1][x].id = id;
        }
        if(skip != 2) { // Fill Diagonal
            m_grid[y+1][x-1] = m_grid[y][x]; 
            m_grid[y+1][x-1].id = id;
        }
        return true;
    }
    else if(IsFree(x - 1, y) && IsFree(x, y - 1) && IsFree(x - 1, y - 1)) {
        int skip = std::rand() % 3; // 0=Right, 1=Bottom, 2=Diagonal
        
        int id = m_grid[y][x].id;
        m_grid[y][x].shape = RoomShape::LShape;

        if(skip != 0) { // Fill Right
            m_grid[y][x-1] = m_grid[y][x]; 
            m_grid[y][x-1].id = id;
        }
        if(skip != 1) { // Fill Bottom
            m_grid[y-1][x] = m_grid[y][x]; 
            m_grid[y-1][x].id = id;
        }
        if(skip != 2) { // Fill Diagonal
            m_grid[y-1][x-1] = m_grid[y][x]; 
            m_grid[y-1][x-1].id = id;
        }
        return true;
    }
    else if(IsFree(x + 1, y) && IsFree(x, y - 1) && IsFree(x + 1, y - 1)) {
        int skip = std::rand() % 3; // 0=Right, 1=Bottom, 2=Diagonal
        
        int id = m_grid[y][x].id;
        m_grid[y][x].shape = RoomShape::LShape;

        if(skip != 0) { // Fill Right
            m_grid[y][x+1] = m_grid[y][x]; 
            m_grid[y][x+1].id = id;
        }
        if(skip != 1) { // Fill Bottom
            m_grid[y-1][x] = m_grid[y][x]; 
            m_grid[y-1][x].id = id;
        }
        if(skip != 2) { // Fill Diagonal
            m_grid[y-1][x+1] = m_grid[y][x]; 
            m_grid[y-1][x+1].id = id;
        }
        return true;
    }
    return false;
}