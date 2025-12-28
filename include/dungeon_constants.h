#pragma once

// Dungeon spatial constants
inline constexpr int DUNGEON_GRID_SIZE = 12;      // 12x12 rooms
inline constexpr int ROOM_TILE_WIDTH = 16;        // Each room is 16 tiles wide (reduced from 20)
inline constexpr int ROOM_TILE_HEIGHT = 15;       // Each room is 15 tiles tall
// TILE_SIZE defined in utils.h - do not redefine here

inline constexpr int ROOM_PIXEL_WIDTH = ROOM_TILE_WIDTH * 64;   // 1024 pixels (reduced from 1280)
inline constexpr int ROOM_PIXEL_HEIGHT = ROOM_TILE_HEIGHT * 64; // 960 pixels (using 64 directly)

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
