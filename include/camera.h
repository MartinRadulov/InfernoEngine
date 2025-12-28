#pragma once
#include "dungeon_constants.h"
#include "room.h"

/// @brief Camera system for room-based rendering
/// @details Manages camera position to center rooms on screen.
/// Converts world coordinates to screen coordinates via offsets.
/// Screen: 1920x1080, Room: 1280x960 (20x15 tiles of 64px each)
class Camera {
public:
    Camera();

    /// @brief Initialize camera to specific room in dungeon grid
    /// @param gridX Dungeon grid X coordinate (0-11)
    /// @param gridY Dungeon grid Y coordinate (0-11)
    void SetRoom(int gridX, int gridY);

    /// @brief Update camera based on player's world position
    /// @param playerWorldX Player X position in world pixels
    /// @param playerWorldY Player Y position in world pixels
    /// @details Detects when player moves to a new room and updates camera accordingly
    void Update(float playerWorldX, float playerWorldY, Room* currentRoom);

    /// @brief Get camera X offset for rendering
    /// @return X offset to apply when rendering (worldX + offsetX = screenX)
    int GetOffsetX() const { return m_offsetX; }

    /// @brief Get camera Y offset for rendering
    /// @return Y offset to apply when rendering (worldY + offsetY = screenY)
    int GetOffsetY() const { return m_offsetY; }

    /// @brief Get current room grid X coordinate being viewed
    int GetCurrentRoomGridX() const { return m_currentRoomGridX; }

    /// @brief Get current room grid Y coordinate being viewed
    int GetCurrentRoomGridY() const { return m_currentRoomGridY; }

private:
    int m_currentRoomGridX;  // Current room grid X (0-11)
    int m_currentRoomGridY;  // Current room grid Y (0-11)
    int m_offsetX;           // Camera offset X (for rendering)
    int m_offsetY;           // Camera offset Y (for rendering)
    Room* m_currentRoom;
    bool m_isInBigRoom;
    int m_bigRoomMinX;
    int m_bigRoomMinY;
    int m_bigRoomMaxX;
    int m_bigRoomMaxY;

    /// @brief Calculate camera offsets to center current room on screen
    void CalculateOffset();
    void CalculateBigRoomBounds(Room* room);
    void UpdateForBigRoom(float playerWorldX, float playerWorldY);
    int Clamp(int value, int min, int max);
};
