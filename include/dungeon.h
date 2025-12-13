#pragma once
#include <vector>
#include <iostream>
#include "utils.h"

struct Vector21{
    int x, y;
};

struct RoomCoordinate{
    int x, y;
    int distance;
};

enum class RoomType{
    NONE,
    NORMAL,
    START,
    BOSS,
    TREASURE
};

struct RoomData{
    RoomType type = RoomType::NONE;
    bool active = false;
    bool doorTop = false;
    bool doorBottom = false;
    bool doorLeft = false;
    bool doorRight = false;
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
    
    int CountNeighbours(int x, int y);

    bool IsValidEmptySpot(int x, int y);

    void PlaceSpecialRoom(RoomType type);
};