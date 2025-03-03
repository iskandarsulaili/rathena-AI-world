#ifndef P2P_COMMON_TYPES_HPP
#define P2P_COMMON_TYPES_HPP

#include <cstdint>
#include <string>
#include <vector>
#include <chrono>

namespace rathena {
namespace p2p {

// Basic types
using host_id_t = uint32_t;
using map_id_t = uint32_t;
using player_id_t = uint32_t;
using timestamp_t = uint64_t;

// Position and movement
struct Position {
    float x;
    float y;
    float z;
    uint16_t dir;

    Position() : x(0), y(0), z(0), dir(0) {}
    Position(float x_, float y_, float z_ = 0, uint16_t dir_ = 0)
        : x(x_), y(y_), z(z_), dir(dir_) {}
};

struct Vector3 {
    float x;
    float y;
    float z;
};

// Network types
struct NetworkAddress {
    std::string ip;
    uint16_t port;
    
    bool operator==(const NetworkAddress& other) const {
        return ip == other.ip && port == other.port;
    }
};

// Entity information
struct PlayerInfo {
    player_id_t id;
    std::string name;
    map_id_t current_map;
    Position position;
    NetworkAddress connection;
    uint32_t status;
    timestamp_t last_update;
};

struct MobInfo {
    uint32_t id;
    uint32_t mob_id;
    Position position;
    uint32_t hp;
    uint32_t status;
};

struct ItemInfo {
    uint32_t id;
    uint32_t item_id;
    Position position;
    uint32_t amount;
    timestamp_t despawn_time;
};

// Performance types
struct SystemMetrics {
    float cpu_usage;         // Percentage (0-100)
    float memory_usage;      // Percentage (0-100)
    uint64_t free_memory;    // Bytes
    float network_latency;   // Milliseconds
    uint32_t bandwidth;      // Bytes per second
};

// Map types
struct MapInfo {
    map_id_t id;
    std::string name;
    bool is_critical;
    uint32_t max_players;
    uint32_t current_players;
    uint32_t width;
    uint32_t height;
    std::vector<uint8_t> collision_data;
};

// Host types
struct HostRequirements {
    uint32_t min_cpu_cores;
    uint64_t min_memory;
    uint32_t min_bandwidth;
    float max_latency;
    uint32_t min_uptime;
};

// Status enums
enum class MapStatus : uint8_t {
    OFFLINE = 0,
    STARTING = 1,
    ONLINE = 2,
    TRANSFERRING = 3,
    CLOSING = 4,
    ERROR = 5
};

enum class PlayerStatus : uint8_t {
    OFFLINE = 0,
    CONNECTING = 1,
    ACTIVE = 2,
    DEAD = 3,
    TELEPORTING = 4,
    DISCONNECTING = 5
};

enum class HostStatus : uint8_t {
    OFFLINE = 0,
    STARTING = 1,
    ONLINE = 2,
    DEGRADED = 3,
    FAILING = 4,
    MAINTENANCE = 5
};

// Error types
enum class ErrorCode : uint32_t {
    SUCCESS = 0,
    INVALID_HOST = 1,
    INVALID_MAP = 2,
    HOST_OVERLOADED = 3,
    CONNECTION_FAILED = 4,
    AUTHENTICATION_FAILED = 5,
    SYNC_ERROR = 6,
    TIMEOUT = 7,
    INTERNAL_ERROR = 8
};

// Network packets
struct PacketHeader {
    uint16_t length;
    uint16_t type;
    uint32_t sequence;
    uint32_t timestamp;
    uint8_t flags;
};

// Configuration types
struct SecurityConfig {
    bool enable_encryption;
    bool verify_packets;
    uint32_t key_rotation_interval;
    uint32_t token_validity_period;
};

struct PerformanceConfig {
    uint32_t update_interval;
    uint32_t sync_batch_size;
    uint32_t max_concurrent_transfers;
    bool enable_compression;
};

// Time helper functions
inline timestamp_t current_time() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
}

inline bool is_expired(timestamp_t timestamp, uint32_t timeout_ms) {
    return (current_time() - timestamp) > timeout_ms;
}

} // namespace p2p
} // namespace rathena

#endif // P2P_COMMON_TYPES_HPP