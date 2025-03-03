#include "MapDistributor.hpp"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <queue>

namespace rathena {
namespace p2p {

MapDistributor::MapDistributor(const Config& config, std::shared_ptr<HostManager> host_manager)
    : config_(config)
    , host_manager_(std::move(host_manager)) {
    
    metrics_ = {
        .total_maps = 0,
        .p2p_hosted_maps = 0,
        .vps_hosted_maps = 0,
        .pending_transfers = 0,
        .failed_transfers = 0,
        .average_load = 0.0f
    };
}

MapDistributor::~MapDistributor() = default;

bool MapDistributor::assign_map_to_host(map_id_t map_id, host_id_t host_id) {
    if (!validate_map_assignment(map_id, host_id)) {
        return false;
    }
    
    // Initialize or update map state
    MapState& state = map_states_[map_id];
    state.current_host = host_id;
    state.player_count = 0;
    state.status = MapStatus::STARTING;
    state.last_transfer = std::chrono::system_clock::now();
    
    // Add to host's map list
    host_maps_[host_id].push_back(map_id);
    
    // Update metrics
    auto host = host_manager_->get_host_info(host_id);
    if (host && host->is_vps) {
        metrics_.vps_hosted_maps++;
    } else {
        metrics_.p2p_hosted_maps++;
    }
    
    std::cout << "Map " << map_id << " assigned to host " << host_id << std::endl;
    return true;
}

bool MapDistributor::assign_map(map_id_t map_id, host_id_t preferred_host) {
    std::lock_guard<std::mutex> lock(state_mutex_);
    
    // Check if map is already assigned
    if (map_states_.count(map_id) > 0) {
        return false;
    }
    
    // Try preferred host first if specified
    if (preferred_host != 0 && can_host_map(preferred_host, map_id)) {
        return assign_map_to_host(map_id, preferred_host);
    }
    
    // Find best available host
    host_id_t selected_host = find_best_host(map_id);
    if (selected_host == 0) {
        std::cerr << "No suitable host found for map " << map_id << std::endl;
        return false;
    }
    
    return assign_map_to_host(map_id, selected_host);
}

bool MapDistributor::reassign_map(map_id_t map_id) {
    std::lock_guard<std::mutex> lock(state_mutex_);
    
    auto map_it = map_states_.find(map_id);
    if (map_it == map_states_.end()) {
        return false;
    }
    
    // Get current host and exclude it from candidates
    host_id_t current_host = map_it->second.current_host;
    std::vector<host_id_t> excluded_hosts = {current_host};
    
    // Find new host
    host_id_t new_host = find_best_host(map_id, excluded_hosts);
    if (new_host == 0) {
        std::cerr << "No alternative host found for map " << map_id << std::endl;
        return false;
    }
    
    // Initiate transfer
    return initiate_map_transfer(map_id, new_host);
}

void MapDistributor::rebalance_maps() {
    std::lock_guard<std::mutex> lock(state_mutex_);
    
    // Calculate optimal distribution
    auto optimal_distribution = calculate_optimal_distribution();
    
    // Apply changes
    for (const auto& [map_id, new_host] : optimal_distribution) {
        auto current_host = get_map_host(map_id);
        if (current_host != new_host) {
            initiate_map_transfer(map_id, new_host);
        }
    }
}

std::vector<std::pair<map_id_t, host_id_t>> MapDistributor::calculate_optimal_distribution() {
    std::vector<std::pair<map_id_t, host_id_t>> distribution;
    
    // Get all maps and available hosts
    std::vector<map_id_t> maps_to_distribute;
    for (const auto& [map_id, state] : map_states_) {
        maps_to_distribute.push_back(map_id);
    }
    
    auto eligible_hosts = host_manager_->get_eligible_hosts();
    if (eligible_hosts.empty()) {
        return distribution;
    }
    
    // Sort maps by priority (player count, is_critical)
    std::sort(maps_to_distribute.begin(), maps_to_distribute.end(),
        [this](map_id_t a, map_id_t b) {
            const auto& state_a = map_states_[a];
            const auto& state_b = map_states_[b];
            if (state_a.is_critical != state_b.is_critical) {
                return state_a.is_critical;
            }
            return state_a.player_count > state_b.player_count;
        });
    
    // Distribute maps
    for (auto map_id : maps_to_distribute) {
        host_id_t best_host = find_best_host(map_id);
        if (best_host != 0) {
            distribution.emplace_back(map_id, best_host);
        }
    }
    
    return distribution;
}

void MapDistributor::handle_host_failure(host_id_t host_id) {
    std::lock_guard<std::mutex> lock(state_mutex_);
    
    // Get all maps hosted by failed host
    auto hosted_maps = host_maps_[host_id];
    
    // Remove failed host from tracking
    host_maps_.erase(host_id);
    
    // Reassign each map
    for (auto map_id : hosted_maps) {
        auto map_it = map_states_.find(map_id);
        if (map_it != map_states_.end()) {
            map_it->second.status = MapStatus::OFFLINE;
            reassign_map(map_id);
        }
    }
    
    update_metrics();
}

host_id_t MapDistributor::find_best_host(map_id_t map_id, 
                                       const std::vector<host_id_t>& excluded_hosts) {
    // Get all eligible hosts
    auto eligible_hosts = host_manager_->get_eligible_hosts();
    
    // Remove excluded hosts
    eligible_hosts.erase(
        std::remove_if(eligible_hosts.begin(), eligible_hosts.end(),
            [&](host_id_t host_id) {
                return std::find(excluded_hosts.begin(), excluded_hosts.end(), 
                               host_id) != excluded_hosts.end();
            }
        ),
        eligible_hosts.end()
    );
    
    if (eligible_hosts.empty()) {
        return 0;
    }
    
    // Calculate scores for each host
    std::vector<std::pair<host_id_t, float>> host_scores;
    for (auto host_id : eligible_hosts) {
        if (!can_host_map(host_id, map_id)) {
            continue;
        }
        
        float score = calculate_host_score(host_id);
        host_scores.emplace_back(host_id, score);
    }
    
    if (host_scores.empty()) {
        return 0;
    }
    
    // Sort by score and return best host
    std::sort(host_scores.begin(), host_scores.end(),
        [](const auto& a, const auto& b) {
            return a.second > b.second;
        }
    );
    
    return host_scores[0].first;
}

bool MapDistributor::validate_map_assignment(map_id_t map_id, host_id_t host_id) {
    // Verify host exists and is healthy
    if (!host_manager_->is_host_healthy(host_id)) {
        return false;
    }
    
    // Check host capacity
    if (!validate_host_capacity(host_id)) {
        return false;
    }
    
    // Check if map is critical and host is suitable
    auto map_it = map_states_.find(map_id);
    if (map_it != map_states_.end() && map_it->second.is_critical) {
        auto host = host_manager_->get_host_info(host_id);
        if (!host || !host->is_vps) {
            return false;
        }
    }
    
    return true;
}

bool MapDistributor::validate_host_capacity(host_id_t host_id) {
    auto maps = get_host_maps(host_id);
    return maps.size() < config_.max_maps_per_host;
}

float MapDistributor::calculate_host_score(host_id_t host_id) {
    auto host = host_manager_->get_host_info(host_id);
    if (!host) {
        return 0.0f;
    }
    
    float score = 100.0f;
    
    // Performance score weight (40%)
    score *= (host->performance_score * 0.4f);
    
    // Current load penalty (30%)
    float load = get_host_load(host_id);
    if (load > config_.load_threshold) {
        score *= (1.0f - ((load - config_.load_threshold) * 0.3f));
    }
    
    // Map count penalty (20%)
    auto hosted_maps = get_host_maps(host_id);
    if (hosted_maps.size() >= config_.max_maps_per_host) {
        score *= 0.8f;
    }
    
    // VPS preference for critical maps (10%)
    if (host->is_vps) {
        score *= 1.1f;
    }
    
    return score;
}

bool MapDistributor::initiate_map_transfer(map_id_t map_id, host_id_t new_host) {
    auto map_it = map_states_.find(map_id);
    if (map_it == map_states_.end()) {
        return false;
    }
    
    // Update map state
    map_it->second.status = MapStatus::TRANSFERRING;
    map_it->second.last_transfer = std::chrono::system_clock::now();
    
    // Update host assignments
    host_id_t old_host = map_it->second.current_host;
    if (old_host != 0) {
        auto& old_host_maps = host_maps_[old_host];
        old_host_maps.erase(
            std::remove(old_host_maps.begin(), old_host_maps.end(), map_id),
            old_host_maps.end()
        );
    }
    
    host_maps_[new_host].push_back(map_id);
    map_it->second.current_host = new_host;
    
    metrics_.pending_transfers++;
    
    std::cout << "Initiating transfer of map " << map_id 
              << " from host " << old_host 
              << " to host " << new_host << std::endl;
              
    return true;
}

void MapDistributor::complete_map_transfer(map_id_t map_id) {
    std::lock_guard<std::mutex> lock(state_mutex_);
    
    auto map_it = map_states_.find(map_id);
    if (map_it != map_states_.end() && 
        map_it->second.status == MapStatus::TRANSFERRING) {
        
        map_it->second.status = MapStatus::ONLINE;
        metrics_.pending_transfers--;
        
        std::cout << "Completed transfer of map " << map_id 
                  << " to host " << map_it->second.current_host << std::endl;
    }
}

void MapDistributor::update_metrics() {
    metrics_.total_maps = map_states_.size();
    metrics_.p2p_hosted_maps = 0;
    metrics_.vps_hosted_maps = 0;
    
    float total_load = 0.0f;
    uint32_t loaded_hosts = 0;
    
    for (const auto& [host_id, maps] : host_maps_) {
        auto host = host_manager_->get_host_info(host_id);
        if (host) {
            if (host->is_vps) {
                metrics_.vps_hosted_maps += maps.size();
            } else {
                metrics_.p2p_hosted_maps += maps.size();
            }
            
            float load = get_host_load(host_id);
            if (load > 0.0f) {
                total_load += load;
                loaded_hosts++;
            }
        }
    }
    
    metrics_.average_load = loaded_hosts > 0 ? total_load / loaded_hosts : 0.0f;
}

void MapDistributor::log_distribution_metrics() {
    std::ostringstream oss;
    oss << "Map Distribution Metrics:\n"
        << "Total Maps: " << metrics_.total_maps << "\n"
        << "P2P Hosted: " << metrics_.p2p_hosted_maps << "\n"
        << "VPS Hosted: " << metrics_.vps_hosted_maps << "\n"
        << "Pending Transfers: " << metrics_.pending_transfers << "\n"
        << "Failed Transfers: " << metrics_.failed_transfers << "\n"
        << "Average Load: " << metrics_.average_load * 100.0f << "%";
    
    std::cout << oss.str() << std::endl;
}

} // namespace p2p
} // namespace rathena