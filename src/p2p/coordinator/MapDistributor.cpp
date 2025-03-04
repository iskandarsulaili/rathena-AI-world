#include "MapDistributor.hpp"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <queue>
#include <cmath>

namespace rathena {
namespace p2p {

MapDistributor::MapDistributor(const Config& config, std::shared_ptr<HostManager> host_manager)
    : config_(config)
    , host_manager_(std::move(host_manager)) {
    
    metrics_ = {
        .total_maps = 0,
        .p2p_hosted_maps = 0,
        .vps_hosted_maps = 0,
        .main_server_fallbacks = 0,
        .pending_transfers = 0,
        .successful_p2p_migrations = 0,
        .failed_p2p_attempts = 0,
        .failed_transfers = 0,
        .average_load = 0.0f
    };
}

MapDistributor::~MapDistributor() = default;

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
    
    // Network quality score for P2P hosts (20%)
    if (!host->is_vps) {
        float network_factor = 1.0f;
        
        // Apply latency penalty
        if (host->metrics.network_latency > 0) {
            network_factor *= std::max(0.0f, 1.0f - (host->metrics.network_latency / 100.0f));
        }
        
        // Apply uptime bonus (max bonus at 24 hours)
        auto uptime = std::chrono::duration_cast<std::chrono::hours>(
            std::chrono::system_clock::now() - host->registration_time).count();
            
        float uptime_hours = static_cast<float>(uptime);
        network_factor *= (1.0f + std::min(uptime_hours, 24.0f) / 24.0f);
        
        score *= (network_factor * 0.2f);
    }
    
    // Map count penalty (20%)
    auto hosted_maps = get_host_maps(host_id);
    if (hosted_maps.size() >= config_.max_maps_per_host) {
        score *= 0.8f;
    }
    
    return score;
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

bool MapDistributor::validate_p2p_eligibility(host_id_t host_id, map_id_t map_id) {
    auto host = host_manager_->get_host_info(host_id);
    if (!host) {
        return false;
    }
    
    // Check if host meets P2P requirements
    if (host->is_vps || host->performance_score < (config_.min_p2p_score / 100.0f)) {
        return false;
    }
    
    return true;
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

} // namespace p2p
} // namespace rathena