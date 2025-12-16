#pragma once
#include <vector>
#include <iostream>
#include <queue>
#include <set>
#include <map>
#include "room.h"

enum class RoomType{
    NONE,
    NORMAL,
    START,
    BOSS,
    TREASURE,
    SHOP,
    SECRET,
    SUPER_SECRET
};

enum class RoomShape{
    Dim1x1,
    Dim2x2,
    Dim1x2,
    Dim2x1,
    LShape
};

// Lightweight struct for door rendering
struct DoorState {
    bool top = false;
    bool bottom = false;
    bool left = false;
    bool right = false;
};

class Dungeon{
public:
    Dungeon();
    void GenerateDungeon(int maxRooms);
    void PrintMapToConsole();
    
    // Spatial queries (grid-based)
    int GetRoomIDAt(int x, int y) const;
    bool IsPositionValid(int x, int y) const;
    
    // Room queries (Room-based) - PRIMARY INTERFACE
    Room* GetRoomAt(int x, int y);
    Room* GetRoomByID(int id);
    Room* GetStartRoom();
    std::vector<Room*> GetAllRooms();
    std::vector<Room*> GetSpecialRooms();
    std::vector<Room*> GetConnectedRooms(int roomID);
    
    static const int DUNGEON_SIZE = 12;
    
private:
    // LIGHTWEIGHT grid - just stores room IDs for spatial lookup
    int m_gridRoomIDs[DUNGEON_SIZE][DUNGEON_SIZE];
    
    // Door state for rendering (could be computed on-demand but cached for performance)
    DoorState m_doors[DUNGEON_SIZE][DUNGEON_SIZE];
    
    // PRIMARY DATA: Room objects
    std::map<int, Room> m_rooms;
    int m_lastRoomID = 0;
    int m_startRoomID = -1;
    
    // Generation helpers
    Room* CreateRoom(RoomType type, RoomShape shape, const std::vector<Point>& cells);
    void ConnectRooms(Room* a, Room* b);
    void UpdateDoorStates(); // Sync door rendering state with room connections
    
    // Generation phases
    void ClearAll();
    void GenerateMainPath(int maxRooms);
    void GenerateComplexRooms();
    void CleanupInvalidConnections(); // Remove connections that violate shape rules
    void AutoConnectNormalRooms(); // Auto-connect adjacent normal/start rooms
    void PlaceSpecialRoom(RoomType type);
    void CalculateStepDistances();
    void PlaceSecretRoom();
    void PlaceSuperSecretRoom();
    
    // Room shape creation
    bool TryCreateBigRoom(Room* room);
    bool TryCreateSkinnyRoom(Room* room);
    bool TryCreateLRoom(Room* room);
    
    // Utilities
    bool IsCellFree(int x, int y) const;
    int CountOccupiedNeighbours(Point p) const;
    bool HasConflictingNeighbour(Point p) const;
    std::vector<Point> GetValidEmptyNeighbors(Point p) const;
    Room* GetRoomAtCell(Point p);
};
