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
    std::chrono::system_clock::time_point last_update;
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
};

class HostManager {
public:
    struct Config {
        uint32_t min_score_threshold;
        uint32_t check_interval;
        uint32_t grace_period;
        uint32_t max_players_per_host;
        bool enable_auto_failover;
    };

    explicit HostManager(const Config& config);
    ~HostManager();

    // Host registration and management
    uint32_t register_host(const std::string& address, uint16_t port, bool is_vps);
    bool unregister_host(uint32_t host_id);
    bool update_host_metrics(uint32_t host_id, const HostMetrics& metrics);
    
    // Host selection and querying
    std::vector<uint32_t> get_eligible_hosts(uint32_t min_score = 0);
    HostInfo* get_host_info(uint32_t host_id);
    bool is_host_healthy(uint32_t host_id);
    uint32_t get_best_host_for_map(uint32_t map_id);
    
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
    
private:
    // Internal state
    std::mutex state_mutex_;
    std::unordered_map<uint32_t, HostInfo> hosts_;
    uint32_t next_host_id_;
    
    // Host validation
    bool validate_host_requirements(const HostMetrics& metrics);
    bool validate_host_capacity(uint32_t host_id);
    
    // Authentication
    std::string generate_auth_token();
    bool verify_auth_token(uint32_t host_id, const std::string& token);
    
    // Health monitoring
    void mark_host_unhealthy(uint32_t host_id);
    bool try_host_recovery(uint32_t host_id);
    
    // Load balancing
    void rebalance_hosts();
    float calculate_host_load(uint32_t host_id);
    
    // Configuration and state
    Config config_;
    struct HealthCheckState {
        uint32_t consecutive_failures;
        std::chrono::system_clock::time_point last_check;
        bool is_degraded;
    };
    std::unordered_map<uint32_t, HealthCheckState> health_states_;
    
    // Metrics tracking
    struct PerformanceMetrics {
        float average_cpu;
        float average_memory;
        float average_latency;
        uint32_t total_players;
        uint32_t active_hosts;
    } performance_metrics_;
    
    void update_performance_metrics();
    void log_performance_metrics();
};

} // namespace p2p
} // namespace rathena

#endif // HOST_MANAGER_HPP