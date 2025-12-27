#pragma once

// Dungeon spatial constants
inline constexpr int DUNGEON_GRID_SIZE = 12;      // 12x12 rooms
inline constexpr int ROOM_TILE_WIDTH = 20;        // Each room is 20 tiles wide
inline constexpr int ROOM_TILE_HEIGHT = 15;       // Each room is 15 tiles tall
// TILE_SIZE defined in utils.h - do not redefine here

inline constexpr int ROOM_PIXEL_WIDTH = ROOM_TILE_WIDTH * 64;   // 1280 pixels (using 64 directly)
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
