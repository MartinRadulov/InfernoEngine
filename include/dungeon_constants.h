#pragma once

// Dungeon spatial constants
const int DUNGEON_GRID_SIZE = 12;      // 12x12 rooms
const int ROOM_TILE_WIDTH = 20;        // Each room is 20 tiles wide
const int ROOM_TILE_HEIGHT = 15;       // Each room is 15 tiles tall
const int TILE_SIZE = 64;              // 64 pixels per tile

const int ROOM_PIXEL_WIDTH = ROOM_TILE_WIDTH * TILE_SIZE;   // 1280 pixels
const int ROOM_PIXEL_HEIGHT = ROOM_TILE_HEIGHT * TILE_SIZE; // 960 pixels

// Helper: Convert dungeon grid to world pixels
inline int DungeonGridToWorldX(int gridX) {
    return gridX * ROOM_PIXEL_WIDTH;
}

inline int DungeonGridToWorldY(int gridY) {
    return gridY * ROOM_PIXEL_HEIGHT;
}

// Helper: Convert world pixels to dungeon grid
inline int WorldToDungeonGridX(int worldX) {
    return worldX / ROOM_PIXEL_WIDTH;
}

inline int WorldToDungeonGridY(int worldY) {
    return worldY / ROOM_PIXEL_HEIGHT;
}
