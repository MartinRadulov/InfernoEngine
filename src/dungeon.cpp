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
    ClearAll();
    GenerateMainPath(maxRooms);
    GenerateComplexRooms();
    AutoConnectNormalRooms();
    PlaceSpecialRoom(RoomType::BOSS);
    PlaceSpecialRoom(RoomType::SHOP);
    PlaceSpecialRoom(RoomType::TREASURE);
    PlaceConditionalSpecialRoom(RoomType::CURSE, 100);
    PlaceConditionalSpecialRoom(RoomType::SACRIFICIAL, 60);
    PlaceConditionalSpecialRoom(RoomType::CHALLENGE, 60);
    PlaceConditionalSpecialRoom(RoomType::BOSS_CHALLENGE, 50);
    PlaceConditionalSpecialRoom(RoomType::LIBRARY, 40);
    PlaceConditionalSpecialRoom(RoomType::ARCADE, 40);
    PlaceConditionalSpecialRoom(RoomType::KEY, 30);
    PlaceConditionalSpecialRoom(RoomType::DICE, 20);
    PlaceConditionalSpecialRoom(RoomType::BEDROOM, 20);
    PlaceSecretRoom();
    PlaceSuperSecretRoom();
    CalculateStepDistances();
    UpdateDoorStates(); // Sync rendering state
}

void Dungeon::ClearAll(){
    // Clear rooms
    m_rooms.clear();
    m_lastRoomID = 0;
    m_startRoomID = -1;
    
    // Clear grid
    for(int y = 0; y < DUNGEON_SIZE; y++){
        for(int x = 0; x < DUNGEON_SIZE; x++){
            m_gridRoomIDs[y][x] = -1;
            m_doors[y][x] = DoorState();
        }
    }
}

Room* Dungeon::CreateRoom(RoomType type, RoomShape shape, const std::vector<Point>& cells){
    int id = m_lastRoomID++;
    Room room(id, type, shape);
    
    // Add cells to room
    for(const auto& cell : cells){
        room.AddCell(cell);
        // Update grid
        if(cell.x >= 0 && cell.x < DUNGEON_SIZE && cell.y >= 0 && cell.y < DUNGEON_SIZE){
            m_gridRoomIDs[cell.y][cell.x] = id;
        }
    }
    
    // Store room
    m_rooms[id] = room;
    
    // Track start room
    if(type == RoomType::START){
        m_startRoomID = id;
    }
    
    return &m_rooms[id];
}

void Dungeon::ConnectRooms(Room* a, Room* b){
    if(!a || !b || a->GetID() == b->GetID()) return;
    
    // Bidirectional connection
    a->ConnectTo(b->GetID());
    b->ConnectTo(a->GetID());
}

int Dungeon::GetRoomIDAt(int x, int y) const {
    if(!IsPositionValid(x, y)) return -1;
    return m_gridRoomIDs[y][x];
}

bool Dungeon::IsPositionValid(int x, int y) const {
    return x >= 0 && x < DUNGEON_SIZE && y >= 0 && y < DUNGEON_SIZE;
}

Room* Dungeon::GetRoomAt(int x, int y){
    int id = GetRoomIDAt(x, y);
    return (id >= 0) ? GetRoomByID(id) : nullptr;
}

Room* Dungeon::GetRoomByID(int id){
    auto it = m_rooms.find(id);
    return (it != m_rooms.end()) ? &it->second : nullptr;
}

Room* Dungeon::GetStartRoom(){
    return GetRoomByID(m_startRoomID);
}

std::vector<Room*> Dungeon::GetAllRooms(){
    std::vector<Room*> rooms;
    for(auto& [id, room] : m_rooms){
        rooms.push_back(&room);
    }
    return rooms;
}

std::vector<Room*> Dungeon::GetSpecialRooms(){
    std::vector<Room*> special;
    for(auto& [id, room] : m_rooms){
        if(room.IsSpecialRoom()){
            special.push_back(&room);
        }
    }
    return special;
}

std::vector<Room*> Dungeon::GetConnectedRooms(int roomID){
    std::vector<Room*> connected;
    Room* room = GetRoomByID(roomID);
    if(!room) return connected;
    
    for(int connectedID : room->GetConnectedRoomIDs()){
        Room* connectedRoom = GetRoomByID(connectedID);
        if(connectedRoom){
            connected.push_back(connectedRoom);
        }
    }
    return connected;
}

void Dungeon::GenerateMainPath(int maxRooms){
    Point startPos = {DUNGEON_SIZE / 2, DUNGEON_SIZE / 2};
    
    // Create start room
    Room* startRoom = CreateRoom(RoomType::START, RoomShape::Dim1x1, {startPos});
    
    int roomCount = 1;
    std::vector<Room*> frontier;
    frontier.push_back(startRoom);
    
    while(roomCount < maxRooms && !frontier.empty()){
        int idx = std::rand() % frontier.size();
        Room* current = frontier[idx];
        Point currentCell = current->GetFirstCell();
        
        std::vector<Point> neighbors = GetValidEmptyNeighbors(currentCell);
        
        if(neighbors.empty()){
            frontier.erase(frontier.begin() + idx);
            continue;
        }
        
        Point newPos = neighbors[std::rand() % neighbors.size()];
        Room* newRoom = CreateRoom(RoomType::NORMAL, RoomShape::Dim1x1, {newPos});
        ConnectRooms(current, newRoom);
        roomCount++;
        
        frontier.push_back(newRoom);
        
        // Occasionally stop expanding from this room
        if(std::rand() % 100 < 30){
            frontier.erase(frontier.begin() + idx);
        }
    }
}

std::vector<Point> Dungeon::GetValidEmptyNeighbors(Point p) const {
    std::vector<Point> neighbors;
    
    const Point directions[4] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};
    
    for(const auto& dir : directions){
        Point neighbor = {p.x + dir.x, p.y + dir.y};
        
        // Stay away from edges
        if(neighbor.x < 1 || neighbor.x >= DUNGEON_SIZE - 1 ||
           neighbor.y < 1 || neighbor.y >= DUNGEON_SIZE - 1){
            continue;
        }
        
        if(IsCellFree(neighbor.x, neighbor.y)){
            neighbors.push_back(neighbor);
        }
    }
    
    return neighbors;
}

bool Dungeon::IsCellFree(int x, int y) const {
    // Check if cell is in valid range (not at edges)
    if(x < 1 || x >= DUNGEON_SIZE - 1 || y < 1 || y >= DUNGEON_SIZE - 1){
        return false;
    }
    return IsPositionValid(x, y) && m_gridRoomIDs[y][x] == -1;
}

int Dungeon::CountOccupiedNeighbours(Point p) const {
    int count = 0;
    if(!IsCellFree(p.x, p.y - 1)) count++;
    if(!IsCellFree(p.x, p.y + 1)) count++;
    if(!IsCellFree(p.x - 1, p.y)) count++;
    if(!IsCellFree(p.x + 1, p.y)) count++;
    return count;
}

bool Dungeon::HasConflictingNeighbour(Point p) const {
    auto isConflict = [this](int x, int y) {
        Room* room = const_cast<Dungeon*>(this)->GetRoomAt(x, y);
        return room && room->IsSpecialRoom();
    };
    
    return isConflict(p.x, p.y - 1) || isConflict(p.x, p.y + 1) ||
           isConflict(p.x - 1, p.y) || isConflict(p.x + 1, p.y);
}

void Dungeon::GenerateComplexRooms(){
    std::vector<Room*> normalRooms;
    for(auto& [id, room] : m_rooms){
        if(room.GetType() == RoomType::NORMAL && room.GetShape() == RoomShape::Dim1x1){
            normalRooms.push_back(&room);
        }
    }
    
    std::random_shuffle(normalRooms.begin(), normalRooms.end());
    
    for(Room* room : normalRooms){
        if(room->GetShape() != RoomShape::Dim1x1) continue;
        
        int roll = std::rand() % 100;
        
        if(roll < 10){
            if(TryCreateBigRoom(room)) continue;
        }
        
        if(roll < 25){
            if(TryCreateLRoom(room)) continue;
        }
        
        if(roll < 40){
            if(TryCreateSkinnyRoom(room)) continue;
        }
    }
    
    // CRITICAL: Remove invalid connections after shape changes
    CleanupInvalidConnections();
}

bool Dungeon::TryCreateBigRoom(Room* room){
    Point origin = room->GetFirstCell();
    int x = origin.x, y = origin.y;
    
    // Try all 4 orientations for 2x2
    std::vector<std::vector<Point>> patterns = {
        {{x, y}, {x+1, y}, {x, y+1}, {x+1, y+1}},     // bottom-right
        {{x, y}, {x-1, y}, {x, y+1}, {x-1, y+1}},     // bottom-left
        {{x, y}, {x-1, y}, {x, y-1}, {x-1, y-1}},     // top-left
        {{x, y}, {x+1, y}, {x, y-1}, {x+1, y-1}}      // top-right
    };
    
    for(const auto& pattern : patterns){
        bool allFree = true;
        for(size_t i = 1; i < pattern.size(); i++){ // Skip first (already exists)
            if(!IsCellFree(pattern[i].x, pattern[i].y)){
                allFree = false;
                break;
            }
        }
        
        if(allFree){
            // Expand room
            room->SetShape(RoomShape::Dim2x2);
            for(size_t i = 1; i < pattern.size(); i++){
                room->AddCell(pattern[i]);
                m_gridRoomIDs[pattern[i].y][pattern[i].x] = room->GetID();
            }
            return true;
        }
    }
    
    return false;
}

bool Dungeon::TryCreateSkinnyRoom(Room* room){
    Point origin = room->GetFirstCell();
    int x = origin.x, y = origin.y;
    
    if(std::rand() % 2 == 0){
        // Horizontal (2x1) - BOTH cells can't have vertical neighbors
        bool originNeedsVertical = !IsCellFree(x, y-1) || !IsCellFree(x, y+1);
        if(originNeedsVertical) return false; // Origin already has vertical neighbors
        
        std::vector<Point> expansions = {{x+1, y}, {x-1, y}};
        for(const auto& exp : expansions){
            if(!IsCellFree(exp.x, exp.y)) continue;
            
            // Check if expansion cell ALSO has no vertical neighbors
            bool expNeedsVertical = !IsCellFree(exp.x, exp.y-1) || !IsCellFree(exp.x, exp.y+1);
            if(expNeedsVertical) continue; // Expansion has vertical neighbors, skip
            
            // Both cells are clear! Create horizontal room
            room->SetShape(RoomShape::Dim2x1);
            room->AddCell(exp);
            m_gridRoomIDs[exp.y][exp.x] = room->GetID();
            return true;
        }
    } else {
        // Vertical (1x2) - BOTH cells can't have horizontal neighbors
        bool originNeedsHorizontal = !IsCellFree(x-1, y) || !IsCellFree(x+1, y);
        if(originNeedsHorizontal) return false; // Origin already has horizontal neighbors
        
        std::vector<Point> expansions = {{x, y+1}, {x, y-1}};
        for(const auto& exp : expansions){
            if(!IsCellFree(exp.x, exp.y)) continue;
            
            // Check if expansion cell ALSO has no horizontal neighbors
            bool expNeedsHorizontal = !IsCellFree(exp.x-1, exp.y) || !IsCellFree(exp.x+1, exp.y);
            if(expNeedsHorizontal) continue; // Expansion has horizontal neighbors, skip
            
            // Both cells are clear! Create vertical room
            room->SetShape(RoomShape::Dim1x2);
            room->AddCell(exp);
            m_gridRoomIDs[exp.y][exp.x] = room->GetID();
            return true;
        }
    }
    
    return false;
}

bool Dungeon::TryCreateLRoom(Room* room){
    Point origin = room->GetFirstCell();
    int x = origin.x, y = origin.y;
    
    std::vector<std::vector<Point>> patterns = {
        {{x, y}, {x+1, y}, {x, y+1}, {x+1, y+1}},
        {{x, y}, {x-1, y}, {x, y+1}, {x-1, y+1}},
        {{x, y}, {x-1, y}, {x, y-1}, {x-1, y-1}},
        {{x, y}, {x+1, y}, {x, y-1}, {x+1, y-1}}
    };
    
    for(const auto& pattern : patterns){
        bool allFree = true;
        for(size_t i = 1; i < pattern.size(); i++){
            if(!IsCellFree(pattern[i].x, pattern[i].y)){
                allFree = false;
                break;
            }
        }
        
        if(allFree){
            // Pick 3 of 4 cells randomly
            int skip = std::rand() % 3;
            room->SetShape(RoomShape::LShape);
            
            for(size_t i = 1; i < pattern.size(); i++){
                if(static_cast<int>(i-1) == skip) continue;
                
                room->AddCell(pattern[i]);
                m_gridRoomIDs[pattern[i].y][pattern[i].x] = room->GetID();
            }
            return true;
        }
    }
    
    return false;
}

void Dungeon::PlaceSpecialRoom(RoomType type){
    CalculateStepDistances(); // Ensure distances are current
    
    // Get all normal rooms sorted by distance
    std::vector<Room*> candidates;
    for(auto& [id, room] : m_rooms){
        if(room.GetType() == RoomType::NORMAL && room.GetStepDistance() >= 0){
            candidates.push_back(&room);
        }
    }
    
    std::sort(candidates.begin(), candidates.end(),
        [](const Room* a, const Room* b){
            return a->GetStepDistance() > b->GetStepDistance();
        });
    
    // STRATEGY 1: Create new 1x1 branch off far room
    // Check ALL cells of each candidate room for valid expansion spots
    for(Room* candidate : candidates){
        if(HasConflictingNeighbour(candidate->GetFirstCell())) continue;
        
        // Try EVERY cell in this room (not just first)
        for(const Point& cell : candidate->GetCells()){
            std::vector<Point> emptyNeighbors = GetValidEmptyNeighbors(cell);
            
            for(const auto& spot : emptyNeighbors){
                // Must be a dead-end position (only 1 neighbor)
                int neighborCount = CountOccupiedNeighbours(spot);
                if(neighborCount != 1) continue;
                
                // Must not be next to special rooms
                if(HasConflictingNeighbour(spot)) continue;
                
                // Check if THIS cell's room shape allows connection in this direction
                int dx = spot.x - cell.x;
                int dy = spot.y - cell.y;
                bool isVertical = (dx == 0);
                bool isHorizontal = (dy == 0);
                
                // Horizontal skinny rooms (2x1) can't have vertical doors
                if(candidate->GetShape() == RoomShape::Dim2x1 && isVertical) continue;
                // Vertical skinny rooms (1x2) can't have horizontal doors  
                if(candidate->GetShape() == RoomShape::Dim1x2 && isHorizontal) continue;
                
                // VALID SPOT! Create 1x1 special room
                Room* newRoom = CreateRoom(type, RoomShape::Dim1x1, {spot});
                ConnectRooms(candidate, newRoom);
                return; // SUCCESS
            }
        }
    }
    
    // STRATEGY 2: Convert existing 1x1 dead-end (preserves rules)
    for(Room* candidate : candidates){
        // MUST be 1x1
        if(candidate->GetShape() != RoomShape::Dim1x1) continue;
        
        // MUST be dead-end (exactly 1 connection)
        if(!candidate->IsDeadEnd()) continue;
        
        // MUST not neighbor special rooms
        Point candidateCell = candidate->GetFirstCell();
        if(HasConflictingNeighbour(candidateCell)) continue;
        
        // Convert it
        candidate->SetType(type);
        return; // SUCCESS
    }
    
    // STRATEGY 3: Try ALL normal rooms (not just far ones) for branching
    for(auto& [id, room] : m_rooms){
        if(room.GetType() != RoomType::NORMAL) continue;
        
        // Try every cell of this room
        for(const Point& cell : room.GetCells()){
            std::vector<Point> emptyNeighbors = GetValidEmptyNeighbors(cell);
            
            for(const auto& spot : emptyNeighbors){
                int neighborCount = CountOccupiedNeighbours(spot);
                if(neighborCount != 1) continue;
                if(HasConflictingNeighbour(spot)) continue;
                
                // Check shape compatibility
                int dx = spot.x - cell.x;
                int dy = spot.y - cell.y;
                bool isVertical = (dx == 0);
                bool isHorizontal = (dy == 0);
                
                if(room.GetShape() == RoomShape::Dim2x1 && isVertical) continue;
                if(room.GetShape() == RoomShape::Dim1x2 && isHorizontal) continue;
                
                Room* newRoom = CreateRoom(type, RoomShape::Dim1x1, {spot});
                ConnectRooms(&room, newRoom);
                return; // SUCCESS
            }
        }
    }
    
    // If we get here, the dungeon is too dense. This should be extremely rare.
    std::cout << "ERROR: Could not place " << 
        (type == RoomType::BOSS ? "BOSS" : 
         type == RoomType::TREASURE ? "TREASURE" : "SHOP") 
        << " room while following placement rules!" << std::endl;
    std::cout << "Dungeon may be too compact. Try generating with fewer rooms." << std::endl;
}

void Dungeon::CalculateStepDistances(){
    // Reset all distances
    for(auto& [id, room] : m_rooms){
        room.SetStepDistance(-1);
    }
    
    Room* startRoom = GetStartRoom();
    if(!startRoom) return;
    
    // BFS from start room
    std::queue<int> q;
    std::set<int> visited;
    
    startRoom->SetStepDistance(0);
    q.push(startRoom->GetID());
    visited.insert(startRoom->GetID());
    
    while(!q.empty()){
        int currentID = q.front();
        q.pop();
        
        Room* current = GetRoomByID(currentID);
        if(!current) continue;
        
        int currentDist = current->GetStepDistance();
        
        for(int neighborID : current->GetConnectedRoomIDs()){
            if(visited.find(neighborID) != visited.end()) continue;
            
            Room* neighbor = GetRoomByID(neighborID);
            if(neighbor){
                neighbor->SetStepDistance(currentDist + 1);
                q.push(neighborID);
                visited.insert(neighborID);
            }
        }
    }
}

void Dungeon::CleanupInvalidConnections(){
    for(auto& [id, room] : m_rooms){
        // Only need to check skinny rooms
        if(room.GetShape() != RoomShape::Dim2x1 && room.GetShape() != RoomShape::Dim1x2){
            continue;
        }
        
        // Get all connections
        std::vector<int> connectionsToRemove;
        for(int connectedID : room.GetConnectedRoomIDs()){
            Room* connected = GetRoomByID(connectedID);
            if(!connected) continue;
            
            // Check if connection is valid based on room shape
            bool isValidConnection = false;
            
            // Check all pairs of cells to determine direction
            for(const Point& myCell : room.GetCells()){
                for(const Point& theirCell : connected->GetCells()){
                    int dx = theirCell.x - myCell.x;
                    int dy = theirCell.y - myCell.y;
                    
                    // They're adjacent?
                    if(std::abs(dx) + std::abs(dy) == 1){
                        bool isVerticalConnection = (dx == 0); // same column
                        bool isHorizontalConnection = (dy == 0); // same row
                        
                        // Check if this connection is allowed
                        if(room.GetShape() == RoomShape::Dim2x1){
                            // Horizontal room - only horizontal connections allowed
                            if(isHorizontalConnection){
                                isValidConnection = true;
                            }
                        } else if(room.GetShape() == RoomShape::Dim1x2){
                            // Vertical room - only vertical connections allowed
                            if(isVerticalConnection){
                                isValidConnection = true;
                            }
                        }
                    }
                }
            }
            
            if(!isValidConnection){
                connectionsToRemove.push_back(connectedID);
            }
        }
        
        // Remove invalid connections (bidirectional)
        for(int badID : connectionsToRemove){
            room.DisconnectFrom(badID);
            Room* other = GetRoomByID(badID);
            if(other){
                other->DisconnectFrom(room.GetID());
            }
        }
    }
}

void Dungeon::AutoConnectNormalRooms(){
    // Auto-connect adjacent Normal/Start rooms (like in Binding of Isaac)
    for(int y = 0; y < DUNGEON_SIZE; y++){
        for(int x = 0; x < DUNGEON_SIZE; x++){
            Room* room = GetRoomAt(x, y);
            if(!room) continue;
            
            // Only auto-connect normal and start rooms
            if(room->GetType() != RoomType::NORMAL && room->GetType() != RoomType::START){
                continue;
            }
            
            // Check all 4 neighbors
            struct {int dx, dy; bool isHorizontal;} dirs[4] = {
                {0, -1, false}, {0, 1, false}, {-1, 0, true}, {1, 0, true}
            };
            
            for(const auto& dir : dirs){
                Room* neighbor = GetRoomAt(x + dir.dx, y + dir.dy);
                if(!neighbor) continue;
                
                // Only connect to Normal/Start rooms
                if(neighbor->GetType() != RoomType::NORMAL && 
                   neighbor->GetType() != RoomType::START){
                    continue;
                }
                
                // Don't connect if already same room (merged cells)
                if(room->GetID() == neighbor->GetID()) continue;
                
                // Check shape compatibility
                // Horizontal rooms (2x1) can't have vertical doors
                // Vertical rooms (1x2) can't have horizontal doors
                if(room->GetShape() == RoomShape::Dim2x1 && !dir.isHorizontal) continue;
                if(room->GetShape() == RoomShape::Dim1x2 && dir.isHorizontal) continue;
                if(neighbor->GetShape() == RoomShape::Dim2x1 && !dir.isHorizontal) continue;
                if(neighbor->GetShape() == RoomShape::Dim1x2 && dir.isHorizontal) continue;
                
                // Connect!
                ConnectRooms(room, neighbor);
            }
        }
    }
}

void Dungeon::UpdateDoorStates(){
    // Clear all doors
    for(int y = 0; y < DUNGEON_SIZE; y++){
        for(int x = 0; x < DUNGEON_SIZE; x++){
            m_doors[y][x] = DoorState();
        }
    }
    
    // Set doors based on room connections
    for(int y = 0; y < DUNGEON_SIZE; y++){
        for(int x = 0; x < DUNGEON_SIZE; x++){
            Room* room = GetRoomAt(x, y);
            if(!room) continue;
            
            // Check all 4 directions
            Room* topRoom = GetRoomAt(x, y-1);
            Room* bottomRoom = GetRoomAt(x, y+1);
            Room* leftRoom = GetRoomAt(x-1, y);
            Room* rightRoom = GetRoomAt(x+1, y);
            
            // Same room = no door needed (merged cells)
            // Connected different rooms = door
            if(topRoom && topRoom->GetID() != room->GetID() && 
               room->IsConnectedTo(topRoom->GetID())){
                m_doors[y][x].top = true;
            }
            
            if(bottomRoom && bottomRoom->GetID() != room->GetID() && 
               room->IsConnectedTo(bottomRoom->GetID())){
                m_doors[y][x].bottom = true;
            }
            
            if(leftRoom && leftRoom->GetID() != room->GetID() && 
               room->IsConnectedTo(leftRoom->GetID())){
                m_doors[y][x].left = true;
            }
            
            if(rightRoom && rightRoom->GetID() != room->GetID() && 
               room->IsConnectedTo(rightRoom->GetID())){
                m_doors[y][x].right = true;
            }
        }
    }
}

void Dungeon::PrintMapToConsole(){
    const int CELL_W = 4;
    const int CELL_H = 2;
    
    int canvasW = DUNGEON_SIZE * CELL_W + 1;
    int canvasH = DUNGEON_SIZE * CELL_H + 1;
    
    std::vector<std::string> canvas(canvasH, std::string(canvasW, ' '));
    
    for(int y = 0; y < DUNGEON_SIZE; y++){
        for(int x = 0; x < DUNGEON_SIZE; x++){
            Room* room = GetRoomAt(x, y);
            
            if(room){
                int py = y * CELL_H;
                int px = x * CELL_W;
                int id = room->GetID();
                
                auto SameRoom = [&](int ny, int nx){
                    Room* neighbor = GetRoomAt(nx, ny);
                    return neighbor && neighbor->GetID() == id;
                };
                
                if(!(SameRoom(y, x-1) && SameRoom(y-1, x) && SameRoom(y-1, x-1)))
                    canvas[py][px] = '+';
                
                if(!(SameRoom(y, x+1) && SameRoom(y-1, x) && SameRoom(y-1, x+1)))
                    canvas[py][px + CELL_W] = '+';
                
                if(!(SameRoom(y, x-1) && SameRoom(y+1, x) && SameRoom(y+1, x-1)))
                    canvas[py + CELL_H][px] = '+';
                
                if(!(SameRoom(y, x+1) && SameRoom(y+1, x) && SameRoom(y+1, x+1)))
                    canvas[py + CELL_H][px + CELL_W] = '+';
            }
            
            int topY = y * CELL_H;
            int topX = x * CELL_W + 1;
            
            Room* currRoom = GetRoomAt(x, y);
            Room* upRoom = GetRoomAt(x, y-1);
            
            bool isSameRoom = currRoom && upRoom && currRoom->GetID() == upRoom->GetID();
            bool isDoor = (currRoom || upRoom) && m_doors[y][x].top;
            
            if(currRoom || upRoom){
                if(isSameRoom){
                    // Same room - leave blank
                } else if(isDoor){
                    canvas[topY][topX + 1] = '.';
                } else {
                    for(int k = 0; k < 3; k++) canvas[topY][topX + k] = '-';
                }
            }
            
            int midY = y * CELL_H + 1;
            int leftX = x * CELL_W;
            
            Room* leftRoom = GetRoomAt(x-1, y);
            
            isSameRoom = currRoom && leftRoom && currRoom->GetID() == leftRoom->GetID();
            isDoor = (currRoom || leftRoom) && m_doors[y][x].left;
            
            if(currRoom || leftRoom){
                if(isSameRoom){
                    // Same room - leave blank
                } else if(isDoor){
                    canvas[midY][leftX] = '.';
                } else {
                    canvas[midY][leftX] = '|';
                }
            }
            
            int centerY = y * CELL_H + 1;
            int centerX = x * CELL_W + 2;
            
            if(currRoom){
                char symbol = '#';
                switch(currRoom->GetType()){
                    case RoomType::START:         symbol = 'S'; break;
                    case RoomType::BOSS:          symbol = 'B'; break;
                    case RoomType::TREASURE:      symbol = 'T'; break;
                    case RoomType::SHOP:          symbol = '$'; break;
                    case RoomType::SECRET:        symbol = '?'; break;
                    case RoomType::SUPER_SECRET:  symbol = '!'; break;
                    case RoomType::CURSE:         symbol = 'C'; break;
                    case RoomType::SACRIFICIAL:   symbol = 'A'; break;
                    case RoomType::CHALLENGE:     symbol = 'H'; break;
                    case RoomType::BOSS_CHALLENGE:symbol = 'b'; break;
                    case RoomType::LIBRARY:       symbol = 'L'; break;
                    case RoomType::ARCADE:        symbol = '@'; break;
                    case RoomType::KEY:           symbol = 'K'; break;
                    case RoomType::DICE:          symbol = 'D'; break;
                    case RoomType::BEDROOM:       symbol = 'Z'; break;
                    case RoomType::NORMAL:        symbol = '#'; break;
                    default:                      symbol = '?'; break;
                }
                canvas[centerY][centerX] = symbol;
            } else {
                canvas[centerY][centerX] = '.';
            }
        }
    }
    
    for(const auto& line : canvas){
        std::cout << line << "\n";
    }
    
    // Print legend
    std::cout << "\nLegend:\n";
    std::cout << "S = Start       B = Boss        T = Treasure    $ = Shop\n";
    std::cout << "? = Secret      ! = SuperSecret C = Curse       A = Sacrificial\n";
    std::cout << "H = Challenge   b = BossChall   L = Library     @ = Arcade\n";
    std::cout << "K = Key         D = Dice        Z = Bedroom     # = Normal\n";
}


void Dungeon::PlaceSecretRoom(){
    // Normal Secret Room: Maximize neighbors
    // Can connect to: NORMAL, START, SHOP, TREASURE
    // Cannot connect to: BOSS, SECRET, SUPER_SECRET
    // CANNOT be physically adjacent to rooms it can't connect to (no walls between neighbors)
    
    struct Candidate {
        Point spot;
        std::vector<Room*> neighbors;
        int neighborCount;
    };
    
    std::vector<Candidate> candidates;
    
    // Check every empty spot in the dungeon
    for(int y = 1; y < DUNGEON_SIZE - 1; y++){
        for(int x = 1; x < DUNGEON_SIZE - 1; x++){
            if(!IsCellFree(x, y)) continue;
            
            Point spot = {x, y};
            std::vector<Room*> validNeighbors;
            int totalNeighborCount = 0;
            
            // Check all 4 directions for potential connections
            const Point dirs[4] = {{0,-1}, {0,1}, {-1,0}, {1,0}};
            
            for(const auto& dir : dirs){
                Room* neighbor = GetRoomAt(x + dir.x, y + dir.y);
                if(!neighbor) continue;
                
                totalNeighborCount++;
                
                // Can connect to NORMAL, START, SHOP, TREASURE
                // Cannot connect to BOSS, SECRET, SUPER_SECRET
                if(neighbor->GetType() == RoomType::BOSS || 
                   neighbor->GetType() == RoomType::SECRET ||
                   neighbor->GetType() == RoomType::SUPER_SECRET) {
                    continue; // Invalid neighbor - skip but count total
                }
                
                // Check shape compatibility
                bool isVertical = (dir.x == 0);
                bool isHorizontal = (dir.y == 0);
                
                if(neighbor->GetShape() == RoomShape::Dim2x1 && isVertical) continue;
                if(neighbor->GetShape() == RoomShape::Dim1x2 && isHorizontal) continue;
                
                validNeighbors.push_back(neighbor);
            }
            
            // CRITICAL: ALL neighbors must be valid (no Boss/Secret nearby even without doors)
            // totalNeighborCount == validNeighbors.size() means no invalid neighbors
            if(!validNeighbors.empty() && 
               totalNeighborCount == static_cast<int>(validNeighbors.size())){
                candidates.push_back({spot, validNeighbors, static_cast<int>(validNeighbors.size())});
            }
        }
    }
    
    if(candidates.empty()){
        std::cout << "Warning: Could not place SECRET room!" << std::endl;
        return;
    }
    
    // Find max neighbor count
    int maxNeighbors = 0;
    for(const auto& candidate : candidates){
        if(candidate.neighborCount > maxNeighbors){
            maxNeighbors = candidate.neighborCount;
        }
    }
    
    // Get all candidates with max neighbors
    std::vector<Candidate> bestCandidates;
    for(const auto& candidate : candidates){
        if(candidate.neighborCount == maxNeighbors){
            bestCandidates.push_back(candidate);
        }
    }
    
    // Pick random one
    Candidate chosen = bestCandidates[std::rand() % bestCandidates.size()];
    
    // Create secret room
    Room* secretRoom = CreateRoom(RoomType::SECRET, RoomShape::Dim1x1, {chosen.spot});
    
    // Connect to all valid neighbors
    for(Room* neighbor : chosen.neighbors){
        ConnectRooms(secretRoom, neighbor);
    }
}

void Dungeon::PlaceSuperSecretRoom(){
    // Super Secret Room: Dead-end, furthest from start, ONLY connects to NORMAL rooms
    // CANNOT be physically adjacent to ANY non-normal rooms (even without doors)
    
    CalculateStepDistances(); // Ensure distances are current
    
    struct Candidate {
        Point spot;
        Room* neighbor;
        int stepDistance;
    };
    
    std::vector<Candidate> candidates;
    
    // Check every empty spot
    for(int y = 1; y < DUNGEON_SIZE - 1; y++){
        for(int x = 1; x < DUNGEON_SIZE - 1; x++){
            if(!IsCellFree(x, y)) continue;
            
            Point spot = {x, y};
            Room* singleNeighbor = nullptr;
            int totalNeighborCount = 0;
            int normalNeighborCount = 0;
            
            // Check all 4 directions
            const Point dirs[4] = {{0,-1}, {0,1}, {-1,0}, {1,0}};
            
            for(const auto& dir : dirs){
                Room* neighbor = GetRoomAt(x + dir.x, y + dir.y);
                if(!neighbor) continue;
                
                totalNeighborCount++;
                
                // Check if it's a NORMAL room
                if(neighbor->GetType() == RoomType::NORMAL){
                    // Check shape compatibility
                    bool isVertical = (dir.x == 0);
                    bool isHorizontal = (dir.y == 0);
                    
                    if(neighbor->GetShape() == RoomShape::Dim2x1 && isVertical) continue;
                    if(neighbor->GetShape() == RoomShape::Dim1x2 && isHorizontal) continue;
                    
                    normalNeighborCount++;
                    singleNeighbor = neighbor;
                }
            }

            if(totalNeighborCount == 1 && normalNeighborCount == 1 && singleNeighbor){
                candidates.push_back({spot, singleNeighbor, singleNeighbor->GetStepDistance()});
            }
        }
    }
    
    if(candidates.empty()){
        std::cout << "Info: Could not place SUPER_SECRET room (this is OK)" << std::endl;
        return;
    }
    
    // Find furthest distance
    int maxDistance = -1;
    for(const auto& candidate : candidates){
        if(candidate.stepDistance > maxDistance){
            maxDistance = candidate.stepDistance;
        }
    }
    
    // Get all furthest candidates
    std::vector<Candidate> furthestCandidates;
    for(const auto& candidate : candidates){
        if(candidate.stepDistance == maxDistance){
            furthestCandidates.push_back(candidate);
        }
    }
    
    // Pick random furthest one
    Candidate chosen = furthestCandidates[std::rand() % furthestCandidates.size()];
    
    // Create super secret room
    Room* superSecretRoom = CreateRoom(RoomType::SUPER_SECRET, RoomShape::Dim1x1, {chosen.spot});
    ConnectRooms(superSecretRoom, chosen.neighbor);
}

void Dungeon::PlaceConditionalSpecialRoom(RoomType type, int createChancePercent){
    // STRATEGY 1: Look for existing dead-end NORMAL room (1x1, exactly 1 connection)
    std::vector<Room*> deadEndCandidates;
    
    for(auto& [id, room] : m_rooms){
        if(room.GetType() != RoomType::NORMAL) continue;
        if(room.GetShape() != RoomShape::Dim1x1) continue;
        if(!room.IsDeadEnd()) continue;
        
        Point cell = room.GetFirstCell();
        if(HasConflictingNeighbour(cell)) continue;
        
        deadEndCandidates.push_back(&room);
    }
    
    // If dead-end found, convert it
    if(!deadEndCandidates.empty()){
        Room* chosen = deadEndCandidates[std::rand() % deadEndCandidates.size()];
        chosen->SetType(type);
        return;
    }
    
    // STRATEGY 2: No dead-end found, roll chance to create new room
    int roll = std::rand() % 100;
    if(roll >= createChancePercent){
        return; // Failed roll, don't place
    }
    
    // Try to create new room (same logic as PlaceSpecialRoom)
    CalculateStepDistances();
    
    std::vector<Room*> candidates;
    for(auto& [id, room] : m_rooms){
        if(room.GetType() == RoomType::NORMAL && room.GetStepDistance() >= 0){
            candidates.push_back(&room);
        }
    }
    
    // Try to branch off far rooms first
    for(Room* candidate : candidates){
        if(HasConflictingNeighbour(candidate->GetFirstCell())) continue;
        
        for(const Point& cell : candidate->GetCells()){
            std::vector<Point> emptyNeighbors = GetValidEmptyNeighbors(cell);
            
            for(const auto& spot : emptyNeighbors){
                int neighborCount = CountOccupiedNeighbours(spot);
                if(neighborCount != 1) continue;
                
                if(HasConflictingNeighbour(spot)) continue;
                
                int dx = spot.x - cell.x;
                int dy = spot.y - cell.y;
                bool isVertical = (dx == 0);
                bool isHorizontal = (dy == 0);
                
                if(candidate->GetShape() == RoomShape::Dim2x1 && isVertical) continue;
                if(candidate->GetShape() == RoomShape::Dim1x2 && isHorizontal) continue;
                
                Room* newRoom = CreateRoom(type, RoomShape::Dim1x1, {spot});
                ConnectRooms(candidate, newRoom);
                return;
            }
        }
    }
    
    // Try all normal rooms
    for(auto& [id, room] : m_rooms){
        if(room.GetType() != RoomType::NORMAL) continue;
        
        for(const Point& cell : room.GetCells()){
            std::vector<Point> emptyNeighbors = GetValidEmptyNeighbors(cell);
            
            for(const auto& spot : emptyNeighbors){
                int neighborCount = CountOccupiedNeighbours(spot);
                if(neighborCount != 1) continue;
                if(HasConflictingNeighbour(spot)) continue;
                
                int dx = spot.x - cell.x;
                int dy = spot.y - cell.y;
                bool isVertical = (dx == 0);
                bool isHorizontal = (dy == 0);
                
                if(room.GetShape() == RoomShape::Dim2x1 && isVertical) continue;
                if(room.GetShape() == RoomShape::Dim1x2 && isHorizontal) continue;
                
                Room* newRoom = CreateRoom(type, RoomShape::Dim1x1, {spot});
                ConnectRooms(&room, newRoom);
                return;
            }
        }
    }
    
    // Could not place (dungeon too dense or bad luck)
}