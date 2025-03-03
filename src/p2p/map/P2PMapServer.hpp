#ifndef P2P_MAP_SERVER_HPP
#define P2P_MAP_SERVER_HPP

#include <memory>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <chrono>
#include "../common/types.hpp"
#include "../common/network.hpp"
#include "MapInstance.hpp"

namespace rathena {
namespace p2p {

class P2PMapServer {
public:
    struct Config {
        uint16_t port;
        uint32_t update_interval;        // ms
        uint32_t sync_interval;          // ms
        uint32_t max_players;
        bool enable_mob_ai;
        bool enable_pet_ai;
        std::string map_cache_path;
    };

    explicit P2PMapServer(const Config& config);
    ~P2PMapServer();

    // Server management
    bool initialize();
    void start();
    void stop();
    bool is_running() const { return running_; }

    // Map management
    bool load_map(map_id_t map_id);
    bool unload_map(map_id_t map_id);
    MapInstance* get_map_instance(map_id_t map_id);
    
    // Player management
    bool add_player(player_id_t player_id, const PlayerInfo& info);
    bool remove_player(player_id_t player_id);
    void update_player_position(player_id_t player_id, float x, float y);
    void handle_player_action(player_id_t player_id, const NetworkPacket& packet);

    // State synchronization
    void sync_with_coordinator();
    bool save_state();
    bool load_state(const std::vector<uint8_t>& state_data);

private:
    // Map state management
    struct MapState {
        map_id_t id;
        std::unique_ptr<MapInstance> instance;
        uint32_t player_count;
        bool is_active;
        std::chrono::system_clock::time_point last_update;
    };

    // Player tracking
    struct PlayerState {
        player_id_t id;
        PlayerInfo info;
        map_id_t current_map;
        float x;
        float y;
        std::chrono::system_clock::time_point last_update;
    };

    // Game loop and updates
    void update_loop();
    void update_maps();
    void update_players();
    void process_ai();
    void handle_collisions();

    // Networking
    void handle_network_packet(const NetworkPacket& packet);
    void broadcast_state_update();
    void send_map_update(map_id_t map_id);

    // State validation
    bool validate_player_action(player_id_t player_id, const NetworkPacket& packet);
    bool validate_player_position(player_id_t player_id, float x, float y);
    bool check_collision(float x1, float y1, float x2, float y2);

    // Anti-cheat
    void detect_speed_hacks();
    void validate_damage_values();
    void check_teleport_hacks();

    // Performance monitoring
    struct Metrics {
        float cpu_usage;
        float memory_usage;
        uint32_t network_latency;
        uint32_t total_players;
        uint32_t packets_per_second;
        uint32_t updates_per_second;
    } metrics_;

    void update_metrics();
    void log_metrics();

    // Thread safety
    std::mutex state_mutex_;
    std::mutex player_mutex_;
    std::mutex network_mutex_;

    // State storage
    std::unordered_map<map_id_t, MapState> maps_;
    std::unordered_map<player_id_t, PlayerState> players_;
    
    // Network management
    std::unique_ptr<NetworkManager> network_manager_;
    
    // Update thread
    std::thread update_thread_;
    std::atomic<bool> running_;
    
    // Configuration
    Config config_;

    // Timers and counters
    std::chrono::system_clock::time_point last_sync_;
    std::chrono::system_clock::time_point last_metrics_update_;
    uint32_t update_count_;
    uint32_t packet_count_;
};

} // namespace p2p
} // namespace rathena

#endif // P2P_MAP_SERVER_HPP