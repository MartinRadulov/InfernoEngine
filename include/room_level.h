#pragma once
#include "dungeon_constants.h"
#include "room.h"

// Need TILE_SIZE from utils.h (can't avoid this dependency)
#ifndef TILE_SIZE
    #include "utils.h"
#endif

// Forward declare to avoid circular dependency
struct DoorState;

#if defined(_WIN32)
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

class RoomLevel{
public:
    RoomLevel();
    RoomLevel(Room* room, int cellIndex);

    void Generate(Room* room, int cellIndex);
    void PlaceDoors(const DoorState& doors);

    void RenderFloors(SDL_Renderer* renderer, int gridX, int gridY, int cameraOffsetX, int cameraOffsetY);

    int GetTile(int row, int col) const;

    bool HasTopDoor() const {return m_hasTopDoor;}
    bool HasBottomDoor() const {return m_hasBottomDoor;}
    bool HasLeftDoor() const {return m_hasLeftDoor;}
    bool HasRightDoor() const {return m_hasRightDoor;}

private:
    // Use ROOM_TILE_HEIGHT and ROOM_TILE_WIDTH from dungeon_constants.h
    int m_mapData[ROOM_TILE_HEIGHT][ROOM_TILE_WIDTH];

    int m_cellIndex;
    bool m_isPartOfBigRoom;

    bool m_hasTopDoor;
    bool m_hasBottomDoor;
    bool m_hasLeftDoor;
    bool m_hasRightDoor;

    void GenerateNormalRoom();
    void GenerateBossRoom();
    void GenerateTreasureRoom();
    void GenerateSecretRoom();
    void RemoveInternalWalls(Room* room, int cellIndex);
    bool CellInRoom(Point cell, const std::vector<Point>& roomCells);
    //more when i need them
};