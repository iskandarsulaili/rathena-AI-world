#ifndef COORDINATOR_SERVER_HPP
#define COORDINATOR_SERVER_HPP

#include <memory>
#include <unordered_map>
#include <vector>
#include <queue>
#include <mutex>
#include <chrono>
#include "../common/crypt.hpp"
#include "../common/network.hpp"
#include "../common/database.hpp"
#include "HostManager.hpp"
#include "MapDistributor.hpp"
#include "DatabaseProxy.hpp"

namespace rathena {
namespace p2p {

class CoordinatorServer {
public:
    struct Config {
        std::string ip;
        uint16_t port;
        uint16_t db_proxy_port;
        uint32_t check_interval;
        uint32_t grace_period;
        uint32_t max_players_per_host;
        bool enable_auto_failover;
    };

    explicit CoordinatorServer(const Config& config);
    ~CoordinatorServer();

    bool initialize();
    void start();
    void stop();

private:
    // Core components
    std::unique_ptr<HostManager> host_manager_;
    std::unique_ptr<MapDistributor> map_distributor_;
    std::unique_ptr<DatabaseProxy> db_proxy_;
    
    // Network handlers
    void handle_host_registration(const NetworkPacket& packet);
    void handle_host_metrics(const NetworkPacket& packet);
    void handle_map_request(const NetworkPacket& packet);
    void handle_failover_request(const NetworkPacket& packet);
    void handle_db_sync(const NetworkPacket& packet);

    // Host management
    void check_host_health();
    void handle_host_failure(uint32_t host_id);
    void redistribute_maps();
    
    // Map management
    bool assign_map_to_host(uint32_t map_id, uint32_t host_id);
    void handle_map_failover(uint32_t map_id);
    
    // Security
    bool verify_host_token(const std::string& token);
    std::string generate_host_token();
    void rotate_encryption_keys();
    
    // State management
    struct HostState {
        uint32_t id;
        std::string address;
        uint32_t performance_score;
        std::chrono::system_clock::time_point last_check;
        std::vector<uint32_t> hosted_maps;
        bool is_vps;
    };
    
    struct MapState {
        uint32_t id;
        uint32_t host_id;
        uint32_t player_count;
        bool is_critical;
    };
    
    // Internal state
    std::mutex state_mutex_;
    std::unordered_map<uint32_t, HostState> hosts_;
    std::unordered_map<uint32_t, MapState> maps_;
    std::queue<uint32_t> failover_queue_;
    
    // Configuration
    Config config_;
    bool running_;
    
    // Metrics and monitoring
    struct Metrics {
        uint32_t active_hosts;
        uint32_t total_players;
        uint32_t failover_count;
        uint32_t sync_errors;
    } metrics_;
    
    void update_metrics();
    void log_metrics();
};

} // namespace p2p
} // namespace rathena

#endif // COORDINATOR_SERVER_HPP