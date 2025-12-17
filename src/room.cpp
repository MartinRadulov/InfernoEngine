#include "../include/room.h"
#include "../include/dungeon.h"
#include <algorithm>

Room::Room() 
    : m_id(0), 
      m_type(RoomType::NONE), 
      m_shape(RoomShape::Dim1x1),
      m_stepDistance(-1),
      m_cleared(false),
      m_discovered(false),
      m_locked(false) {
}

Room::Room(int id, RoomType type, RoomShape shape)
    : m_id(id),
      m_type(type),
      m_shape(shape),
      m_stepDistance(-1),
      m_cleared(false),
      m_discovered(false),
      m_locked(false) {
    // Reserve space for cells
    m_cells.reserve(4); // Max 4 cells for 2x2 rooms
}

void Room::AddCell(Point cell) {
    // Avoid duplicates
    if (!ContainsCell(cell)) {
        m_cells.push_back(cell);
    }
}

void Room::AddCell(int x, int y) {
    AddCell({x, y});
}

bool Room::ContainsCell(Point cell) const {
    return std::find(m_cells.begin(), m_cells.end(), cell) != m_cells.end();
}

bool Room::ContainsCell(int x, int y) const {
    return ContainsCell({x, y});
}

Point Room::GetFirstCell() const {
    if (m_cells.empty()) {
        return {-1, -1}; // Invalid point
    }
    return m_cells[0];
}

Point Room::GetCenterCell() const {
    if (m_cells.empty()) {
        return {-1, -1};
    }
    
    // Calculate center of all cells
    int sumX = 0, sumY = 0;
    for (const auto& cell : m_cells) {
        sumX += cell.x;
        sumY += cell.y;
    }
    
    return {sumX / static_cast<int>(m_cells.size()), 
            sumY / static_cast<int>(m_cells.size())};
}

void Room::ConnectTo(int roomID) {
    if (roomID != m_id) { // Don't connect to self
        m_connectedRoomIDs.insert(roomID);
    }
}

void Room::DisconnectFrom(int roomID) {
    m_connectedRoomIDs.erase(roomID);
}

bool Room::IsConnectedTo(int roomID) const {
    return m_connectedRoomIDs.find(roomID) != m_connectedRoomIDs.end();
}

bool Room::IsSpecialRoom() const {
    return m_type == RoomType::BOSS || 
           m_type == RoomType::TREASURE || 
           m_type == RoomType::SHOP ||
           m_type == RoomType::CURSE ||
           m_type == RoomType::SACRIFICIAL ||
           m_type == RoomType::CHALLENGE ||
           m_type == RoomType::BOSS_CHALLENGE ||
           m_type == RoomType::LIBRARY ||
           m_type == RoomType::ARCADE ||
           m_type == RoomType::KEY ||
           m_type == RoomType::DICE ||
           m_type == RoomType::BEDROOM;
}