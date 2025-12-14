#pragma once
#include <vector>
#include <iostream>
#include <queue>
#include "utils.h"

enum class RoomType{
    NONE,
    NORMAL,
    START,
    BOSS,
    TREASURE,
    SHOP
};

struct RoomData{
    RoomType type = RoomType::NONE;
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
    static const int DUNGEON_SIZE = 10;
    RoomData m_grid[DUNGEON_SIZE][DUNGEON_SIZE];

    struct Point{
    int x, y;
    };
    
    int CountNeighbours(Point p);

    void PlaceSpecialRoom(RoomType type);

    void CalculateStepDistance();

    void UpdateNormalDoors();

    bool HasConflictingNeighbour(Point p);

    void ClearGrid();

    void GenerateMainPath(int maxRooms);

    void ConnectRooms(Point a, Point b);

    std::vector<Point> GetValidEmptyNeighbors(Point p);
};