#pragma once
#include <vector>
#include <set>

// Forward declarations
enum class RoomType;
enum class RoomShape;

struct Point {
    int x, y;
    
    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
};

class Room {
public:
    // Constructors
    Room();
    Room(int id, RoomType type, RoomShape shape);
    
    // Core Getters
    int GetID() const { return m_id; }
    RoomType GetType() const { return m_type; }
    RoomShape GetShape() const { return m_shape; }
    int GetStepDistance() const { return m_stepDistance; }
    const std::vector<Point>& GetCells() const { return m_cells; }
    const std::set<int>& GetConnectedRoomIDs() const { return m_connectedRoomIDs; }
    
    // Core Setters
    void SetType(RoomType type) { m_type = type; }
    void SetShape(RoomShape shape) { m_shape = shape; }
    void SetStepDistance(int distance) { m_stepDistance = distance; }
    
    // Cell management
    void AddCell(Point cell);
    void AddCell(int x, int y);
    bool ContainsCell(Point cell) const;
    bool ContainsCell(int x, int y) const;
    int GetCellCount() const { return m_cells.size(); }
    Point GetFirstCell() const;
    Point GetCenterCell() const; // New: useful for spawning
    
    // Connection management
    void ConnectTo(int roomID);
    void DisconnectFrom(int roomID);
    bool IsConnectedTo(int roomID) const;
    int GetConnectionCount() const { return m_connectedRoomIDs.size(); }
    
    // Room queries
    bool IsSpecialRoom() const;
    bool IsDeadEnd() const { return m_connectedRoomIDs.size() == 1; }
    bool IsMultiCell() const { return m_cells.size() > 1; }
    
    // Game state (NEW - this is why Room class is useful!)
    bool IsCleared() const { return m_cleared; }
    void MarkCleared() { m_cleared = true; }
    
    bool IsDiscovered() const { return m_discovered; }
    void MarkDiscovered() { m_discovered = true; }
    
    bool IsLocked() const { return m_locked; }
    void SetLocked(bool locked) { m_locked = locked; }
    
private:
    // Core data
    int m_id;
    RoomType m_type;
    RoomShape m_shape;
    int m_stepDistance;
    std::vector<Point> m_cells;           // All grid positions this room occupies
    std::set<int> m_connectedRoomIDs;     // IDs of connected rooms
    
    // Game state
    bool m_cleared = false;     // Has player cleared enemies?
    bool m_discovered = false;  // Has player entered this room?
    bool m_locked = false;      // Is room locked (boss doors, etc)?
};
