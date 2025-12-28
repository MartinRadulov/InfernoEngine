#include "../include/camera.h"
#include "../include/utils.h"

Camera::Camera()
    : m_currentRoomGridX(0)
    , m_currentRoomGridY(0)
    , m_offsetX(0)
    , m_offsetY(0)
    , m_currentRoom(nullptr)
    , m_isInBigRoom(false)
    , m_bigRoomMinX(0)
    , m_bigRoomMinY(0)
    , m_bigRoomMaxX(0)
    , m_bigRoomMaxY(0)
{
    // Initialize camera to origin room (0, 0)
    CalculateOffset();
}

void Camera::SetRoom(int gridX, int gridY) {
    m_currentRoomGridX = gridX;
    m_currentRoomGridY = gridY;
    CalculateOffset();
}

void Camera::Update(float playerWorldX, float playerWorldY, Room* currentRoom) {
    // Convert player world position to dungeon grid coordinates
    int playerGridX = WorldToDungeonGridX(static_cast<int>(playerWorldX));
    int playerGridY = WorldToDungeonGridY(static_cast<int>(playerWorldY));

    bool roomChanged = false;
    if(!m_currentRoom || !currentRoom || m_currentRoom->GetID() != currentRoom->GetID()){
        m_currentRoom = currentRoom;
        m_isInBigRoom = currentRoom && currentRoom->IsMultiCell();
        roomChanged = true;

        if(m_isInBigRoom){
            CalculateBigRoomBounds(currentRoom);
        }
    }

    if(!m_isInBigRoom){
        // Small room: lock to center
        if(roomChanged){
            SetRoom(playerGridX, playerGridY);
        }
    }
    else{
        // Big room: follow player with clamping
        UpdateForBigRoom(playerWorldX, playerWorldY);
    }

    // // Check if player moved to a different room
    // if (playerGridX != m_currentRoomGridX || playerGridY != m_currentRoomGridY) {
    //     // Update to new room
    //     m_currentRoomGridX = playerGridX;
    //     m_currentRoomGridY = playerGridY;
    //     CalculateOffset();
    // }
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

void Camera::CalculateBigRoomBounds(Room* room){
    const auto& cells = room->GetCells();

    int minGridX = cells[0].x, maxGridX = cells[0].x;
    int minGridY = cells[0].y, maxGridY = cells[0].y;

    for(const auto& cell : cells){
        if(cell.x < minGridX) minGridX = cell.x;
        if(cell.x > maxGridX) maxGridX = cell.x;
        if(cell.y < minGridY) minGridY = cell.y;
        if(cell.y > maxGridY) maxGridY = cell.y;
    }

    m_bigRoomMinX = DungeonGridToWorldX(minGridX);
    m_bigRoomMinY = DungeonGridToWorldY(minGridY);
    m_bigRoomMaxX = DungeonGridToWorldX(maxGridX + 1);
    m_bigRoomMaxY = DungeonGridToWorldY(maxGridY + 1);
}

void Camera::UpdateForBigRoom(float playerWorldX, float playerWorldY) {
    // Camera aims to center player on screen
    int desiredCameraX = static_cast<int>(playerWorldX) - (SCREEN_WIDTH / 2);
    int desiredCameraY = static_cast<int>(playerWorldY) - (SCREEN_HEIGHT / 2);

    // Clamp to big room bounds (don't show outside the room)
    int clampedCameraX = Clamp(desiredCameraX,
                               m_bigRoomMinX,
                               m_bigRoomMaxX - SCREEN_WIDTH);
    int clampedCameraY = Clamp(desiredCameraY,
                               m_bigRoomMinY,
                               m_bigRoomMaxY - SCREEN_HEIGHT);

    // Convert to rendering offset
    // screenX = worldX + offsetX
    // We want cameraX at screenX = 0, so offsetX = -cameraX
    m_offsetX = -clampedCameraX;
    m_offsetY = -clampedCameraY;

    // Update current grid position (for room rendering)
    m_currentRoomGridX = WorldToDungeonGridX(static_cast<int>(playerWorldX));
    m_currentRoomGridY = WorldToDungeonGridY(static_cast<int>(playerWorldY));
}

int Camera::Clamp(int value, int min, int max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}
