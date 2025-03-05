#ifndef HOST_MANAGER_HPP
#define HOST_MANAGER_HPP

#include <memory>
#include <unordered_map>
#include <vector>
#include <mutex>
#include <chrono>
#include "../common/types.hpp"
#include "../common/network.hpp"

namespace rathena {
namespace p2p {

struct HostMetrics {
    float cpu_usage;
    float memory_usage;
    float network_latency;
    uint32_t player_count;
    uint32_t error_count;
    float bandwidth_in;      // Incoming bandwidth in Mbps
    float bandwidth_out;     // Outgoing bandwidth in Mbps
    uint32_t connection_rate;   // New connections per second
    uint32_t packet_rate;      // Packets per second
    float packet_loss;         // Packet loss percentage
    float jitter;             // Network jitter in ms
    std::vector<float> latency_history; // Recent latency measurements
    bool potential_ddos;      // DDoS detection flag
    std::chrono::system_clock::time_point last_update;
};

struct GeoLocation {
    double latitude;
    double longitude;
    std::string region;
};

struct SessionInfo {
    uint32_t char_id;
    uint32_t map_id;
    std::chrono::system_clock::time_point start_time;
};

struct HostInfo {
    uint32_t id;
    std::string address;
    uint16_t port;
    uint32_t performance_score;
    bool is_vps;
    HostMetrics metrics;
    std::vector<uint32_t> hosted_maps;
    std::string auth_token;
    std::chrono::system_clock::time_point registration_time;
    GeoLocation location;
    std::vector<SessionInfo> active_sessions;
    std::chrono::system_clock::time_point last_sync;
};

class HostManager {
public:
    struct Config {
        uint32_t min_score_threshold;
        uint32_t check_interval;
        uint32_t grace_period;
        uint32_t max_players_per_host;
        bool enable_auto_failover;
        double max_session_distance;  // Maximum distance (km) for session assignments
        uint32_t sync_interval;       // Map state sync interval in seconds
        float base_player_limit;     // Base number of players per host
        float max_player_multiplier; // Maximum player limit multiplier
        uint32_t ddos_threshold;    // Connections per second threshold for DDoS
        float bandwidth_requirement; // Minimum bandwidth required per player
        float max_latency;         // Maximum acceptable latency
        float max_jitter;         // Maximum acceptable jitter
        bool enable_ddos_protection; // Enable DDoS protection
    };

    explicit HostManager(const Config& config);
    ~HostManager();

    // Host registration and management
    uint32_t register_host(const std::string& address, uint16_t port, bool is_vps,
                          const GeoLocation& location);
    bool unregister_host(uint32_t host_id);
    bool update_host_metrics(uint32_t host_id, const HostMetrics& metrics);
    
    // Host selection and querying
    std::vector<uint32_t> get_eligible_hosts(uint32_t min_score = 0);
    HostInfo* get_host_info(uint32_t host_id);
    bool is_host_healthy(uint32_t host_id);
    uint32_t get_best_host_for_map(uint32_t map_id);
    std::vector<uint32_t> get_nearby_hosts(const GeoLocation& location, 
                                          double max_distance);
    
    // Map assignment
    bool assign_map_to_host(uint32_t map_id, uint32_t host_id);
    bool remove_map_from_host(uint32_t map_id, uint32_t host_id);
    std::vector<uint32_t> get_hosted_maps(uint32_t host_id);
    
    // Health checking
    void check_hosts_health();
    void handle_host_failure(uint32_t host_id);
    
    // Performance scoring
    uint32_t calculate_host_score(const HostMetrics& metrics);
    void update_performance_scores();
    
    // Session management
    bool start_player_session(uint32_t host_id, uint32_t char_id, uint32_t map_id);
    bool end_player_session(uint32_t host_id, uint32_t char_id);
    std::vector<SessionInfo> get_active_sessions(uint32_t host_id);
    uint32_t get_session_count(uint32_t host_id);
    
    // State synchronization
    bool mark_map_synced(uint32_t host_id, uint32_t map_id);
    bool needs_sync(uint32_t host_id, uint32_t map_id);
    std::vector<uint32_t> get_out_of_sync_maps(uint32_t host_id);
    
private:
    // Internal state
    std::mutex state_mutex_;
    std::unordered_map<uint32_t, HostInfo> hosts_;
    uint32_t next_host_id_;
    
    // Geographic utilities
    double calculate_distance(const GeoLocation& loc1, const GeoLocation& loc2);
    bool is_within_range(const GeoLocation& loc1, const GeoLocation& loc2, 
                        double max_distance);
    std::vector<uint32_t> sort_hosts_by_distance(const GeoLocation& location,
                                                const std::vector<uint32_t>& hosts);
    
    // Host validation
    bool validate_host_requirements(const HostMetrics& metrics);
    bool validate_host_capacity(uint32_t host_id);
    float calculate_player_limit_multiplier(const HostMetrics& metrics);
    bool detect_ddos_attack(const HostMetrics& metrics);
    
    // Authentication
    std::string generate_auth_token();
    bool verify_auth_token(uint32_t host_id, const std::string& token);
    
    // Health monitoring
    void mark_host_unhealthy(uint32_t host_id);
    bool try_host_recovery(uint32_t host_id);
    void restore_round_robin_state(uint32_t failed_host_id);
    
    // Load balancing
    void rebalance_hosts();
    float calculate_host_load(uint32_t host_id);
    void distribute_sessions_geographically();
    
    // Configuration and state
    Config config_;
    struct HealthCheckState {
        uint32_t consecutive_failures;
        std::chrono::system_clock::time_point last_check;
        bool is_degraded;
    };
    std::vector<uint32_t> round_robin_order_; // Track round-robin host order
    uint32_t current_round_robin_index_;
    std::unordered_map<uint32_t, HealthCheckState> health_states_;
    
    // Metrics tracking
    struct PerformanceMetrics {
        float average_cpu;
        float average_memory;
        float average_latency;
        uint32_t total_players;
        uint32_t active_hosts;
        struct RegionMetrics {
            uint32_t host_count;
            uint32_t player_count;
            float average_latency;
        };
        std::unordered_map<std::string, RegionMetrics> region_metrics;
        void update_region_metrics(const std::string& region, const HostMetrics& metrics);
    } performance_metrics_;
    
    void update_performance_metrics();
    void log_performance_metrics();
};

} // namespace p2p
} // namespace rathena

#endif // HOST_MANAGER_HPP