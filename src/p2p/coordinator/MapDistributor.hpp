#ifndef MAP_DISTRIBUTOR_HPP
#define MAP_DISTRIBUTOR_HPP

#include <unordered_map>
#include <vector>
#include <memory>
#include <mutex>
#include <chrono>
#include "../common/types.hpp"
#include "HostManager.hpp"

namespace rathena {
namespace p2p {

class MapDistributor {
public:
    struct Config {
        uint32_t max_maps_per_host;
        uint32_t rebalance_interval;
        float load_threshold;
        bool enable_dynamic_assignment;
        bool prefer_p2p_hosting;        // Prioritize P2P hosting for eligible maps
        uint32_t min_p2p_score;         // Minimum score required for P2P hosting
        uint32_t p2p_host_grace_period; // Time to wait for P2P host before fallback
    };

    explicit MapDistributor(const Config& config, std::shared_ptr<HostManager> host_manager);
    ~MapDistributor();

    // Map assignment
    bool assign_map(map_id_t map_id, host_id_t preferred_host = 0);
    bool reassign_map(map_id_t map_id);
    bool unassign_map(map_id_t map_id);
    
    // Host operations
    std::vector<map_id_t> get_host_maps(host_id_t host_id);
    bool can_host_map(host_id_t host_id, map_id_t map_id);
    float get_host_load(host_id_t host_id);
    
    // Map queries
    host_id_t get_map_host(map_id_t map_id);
    std::vector<map_id_t> get_critical_maps();
    std::vector<map_id_t> get_p2p_eligible_maps();
    bool is_map_p2p_eligible(map_id_t map_id);
    
    // Distribution management
    void rebalance_maps();
    void handle_host_failure(host_id_t host_id);
    void update_map_status(map_id_t map_id, MapStatus status);
    void complete_map_transfer(map_id_t map_id);

private:
    // Map state tracking
    struct MapState {
        host_id_t current_host;
        uint32_t player_count;
        MapStatus status;
        bool is_critical;              // Critical maps can't be P2P hosted
        bool prefer_p2p;              // Prefer P2P hosting for this map
        uint32_t p2p_attempts;        // Number of P2P hosting attempts
        bool allow_main_fallback;     // Allow fallback to main server
        std::chrono::system_clock::time_point last_transfer;
    };
    
    // Distribution algorithms
    bool assign_map_to_host(map_id_t map_id, host_id_t host_id);
    host_id_t find_best_p2p_host(map_id_t map_id);
    bool try_p2p_first_assignment(map_id_t map_id);
    host_id_t find_best_host(map_id_t map_id, const std::vector<host_id_t>& excluded_hosts = {});
    std::vector<std::pair<map_id_t, host_id_t>> calculate_optimal_distribution();
    bool should_rebalance_map(map_id_t map_id);
    
    // Load balancing
    float calculate_host_score(host_id_t host_id);
    bool is_host_overloaded(host_id_t host_id);
    void distribute_load(host_id_t overloaded_host);
    
    // Transfer management
    bool initiate_map_transfer(map_id_t map_id, host_id_t new_host);
    void rollback_map_transfer(map_id_t map_id);
    
    // Validation
    bool validate_map_assignment(map_id_t map_id, host_id_t host_id);
    bool validate_host_capacity(host_id_t host_id);
    bool validate_p2p_eligibility(host_id_t host_id, map_id_t map_id);
    
    // State management
    std::mutex state_mutex_;
    std::unordered_map<map_id_t, MapState> map_states_;
    std::unordered_map<host_id_t, std::vector<map_id_t>> host_maps_;
    
    // Dependencies
    std::shared_ptr<HostManager> host_manager_;
    Config config_;
    
    // Metrics
    struct DistributionMetrics {
        uint32_t total_maps;
        uint32_t p2p_hosted_maps;
        uint32_t vps_hosted_maps;
        uint32_t main_server_fallbacks;
        uint32_t pending_transfers;
        uint32_t successful_p2p_migrations;
        uint32_t failed_p2p_attempts;
        uint32_t failed_transfers;
        float average_load;
    } metrics_;
    
    void update_metrics();
    void log_distribution_metrics();
};

} // namespace p2p
} // namespace rathena

#endif // MAP_DISTRIBUTOR_HPP