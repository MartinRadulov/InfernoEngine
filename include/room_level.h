#pragma once
#include "dungeon_constants.h"
#include "room.h"
#include "utils.h"

#if defined(_WIN32)
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

class RoomLevel{
public:
    RoomLevel();
    RoomLevel(Room* room);

    void Generate(Room* room);

    void RenderFloors(SDL_Renderer* renderer, int cameraOffsetX, int cameraOffsetY);

    int GetTile(int row, int col) const;

    bool HasTopDoor() const {return m_hasTopDoor;}
    bool HasBottomDoor() const {return m_hasBottomDoor;}
    bool HasLeftDoor() const {return m_hasLeftDoor;}
    bool HasRightDoor() const {return m_hasRightDoor;}

private:
    int m_mapData[MAP_ROWS][MAP_COLS];

    bool m_hasTopDoor;
    bool m_hasBottomDoor;
    bool m_hasLeftDoor;
    bool m_hasRightDoor;

    void GenerateNormalRoom();
    void GenerateBossRoom();
    void GenerateTreasureRoom();
    void GenerateSecretRoom();
    //more when i need them

    void PlaceDoors(Room* room);
};