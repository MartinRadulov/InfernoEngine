#include "../include/camera.h"
#include "../include/utils.h"

Camera::Camera()
    : m_currentRoomGridX(0)
    , m_currentRoomGridY(0)
    , m_offsetX(0)
    , m_offsetY(0)
{
    // Initialize camera to origin room (0, 0)
    CalculateOffset();
}

void Camera::SetRoom(int gridX, int gridY) {
    m_currentRoomGridX = gridX;
    m_currentRoomGridY = gridY;
    CalculateOffset();
}

void Camera::Update(float playerWorldX, float playerWorldY) {
    // Convert player world position to dungeon grid coordinates
    int playerGridX = WorldToDungeonGridX(static_cast<int>(playerWorldX));
    int playerGridY = WorldToDungeonGridY(static_cast<int>(playerWorldY));

    // Check if player moved to a different room
    if (playerGridX != m_currentRoomGridX || playerGridY != m_currentRoomGridY) {
        // Update to new room
        m_currentRoomGridX = playerGridX;
        m_currentRoomGridY = playerGridY;
        CalculateOffset();
    }
}

void Camera::CalculateOffset() {
    // Get the world pixel position of the current room's top-left corner
    int roomWorldX = DungeonGridToWorldX(m_currentRoomGridX);
    int roomWorldY = DungeonGridToWorldY(m_currentRoomGridY);

    // Calculate padding to center the room on screen
    // Screen: 1920x1080, Room: 1280x960
    // PADDING_X = (1920 - 1280) / 2 = 320
    // PADDING_Y = (1080 - 960) / 2 = 60
    constexpr int PADDING_X = (SCREEN_WIDTH - ROOM_PIXEL_WIDTH) / 2;
    constexpr int PADDING_Y = (SCREEN_HEIGHT - ROOM_PIXEL_HEIGHT) / 2;

    // Calculate offsets
    // To convert world coordinates to screen coordinates:
    // screenX = worldX + offsetX
    // We want: roomWorldX + offsetX = PADDING_X
    // Therefore: offsetX = PADDING_X - roomWorldX
    m_offsetX = PADDING_X - roomWorldX;
    m_offsetY = PADDING_Y - roomWorldY;
}
