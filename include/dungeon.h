#pragma once
#include <vector>
#include <iostream>
#include <queue>
#include <set>
#include "utils.h"

enum class RoomType{
    NONE,
    NORMAL,
    START,
    BOSS,
    TREASURE,
    SHOP
};

enum class RoomShape{
    Dim1x1,
    Dim2x2,
    Dim1x2,
    Dim2x1,
    LShape
};

struct RoomData{
    RoomType type = RoomType::NONE;
    RoomShape shape = RoomShape::Dim1x1;
    int id = 0;
    bool active = false;
    bool doorTop = false;
    bool doorBottom = false;
    bool doorLeft = false;
    bool doorRight = false;
    int stepDistance = -1;
};

class Dungeon{
public:
    Dungeon();

    void GenerateDungeon(int maxRooms);
    bool RoomExists(int x, int y);
    void PrintMapToConsole();

    RoomData GetRoom(int x, int y);
private:
    static const int DUNGEON_SIZE = 12;
    RoomData m_grid[DUNGEON_SIZE][DUNGEON_SIZE];

    struct Point{
    int x, y;
    };

    int m_lastRoomID = 0;

    void GenerateComplexRooms();

    bool TryCreateBigRoom(int x, int y);
    bool TryCreateSkinnyRoom(int x, int y);
    bool TryCreateLRoom(int x, int y);
    
    int CountNeighbours(Point p);

    void PlaceSpecialRoom(RoomType type);

    void CalculateStepDistance();

    void UpdateNormalDoors();

    bool HasConflictingNeighbour(Point p);

    void ClearGrid();

    void GenerateMainPath(int maxRooms);

    void ConnectRooms(Point a, Point b);

    std::vector<Point> GetValidEmptyNeighbors(Point p);

    bool IsFree(int x, int y);
};