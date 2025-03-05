#include "HostManager.hpp"
#include <algorithm>
#include <sstream>
#include <iostream>
#include <cmath>
#include "../p2p/common/security.hpp"

namespace rathena {
namespace p2p {

HostManager::HostManager(const Config& config)
    : config_(config)
    , next_host_id_(1) {
    
    performance_metrics_ = {
        .average_cpu = 0.0f,
        .average_memory = 0.0f,
        .average_latency = 0.0f,
        .total_players = 0,
        .active_hosts = 0
    };
}

HostManager::~HostManager() = default;

uint32_t HostManager::register_host(const std::string& address, uint16_t port, 
                                  bool is_vps, const GeoLocation& location) {
    std::lock_guard<std::mutex> lock(state_mutex_);
    
    // Create new host info
    HostInfo host;
    host.id = next_host_id_++;
    host.address = address;
    host.port = port;
    host.is_vps = is_vps;
    host.performance_score = 0;
    host.registration_time = std::chrono::system_clock::now();
    host.location = location;
    host.last_sync = std::chrono::system_clock::now();
    
    // Generate authentication token
    host.auth_token = Security::generate_auth_token();
    
    // Initialize health check state
    health_states_[host.id] = {
        .consecutive_failures = 0,
        .last_check = std::chrono::system_clock::now(),
        .is_degraded = false
    };
    
    // Store host info
    hosts_[host.id] = std::move(host);
    
    std::cout << "New host registered - ID: " << host.id 
              << ", Address: " << address << ":" << port 
              << (is_vps ? " (VPS)" : " (P2P)")
              << ", Region: " << location.region << std::endl;
              
    return host.id;
}

bool HostManager::unregister_host(uint32_t host_id) {
    std::lock_guard<std::mutex> lock(state_mutex_);
    
    auto it = hosts_.find(host_id);
    if (it == hosts_.end()) {
        return false;
    }
    
    // Remove host from all tracking structures
    hosts_.erase(it);
    health_states_.erase(host_id);
    
    std::cout << "Host unregistered - ID: " << host_id << std::endl;
    return true;
}

bool HostManager::update_host_metrics(uint32_t host_id, const HostMetrics& metrics) {
    std::lock_guard<std::mutex> lock(state_mutex_);
    
    auto host_it = hosts_.find(host_id);
    if (host_it == hosts_.end()) {
        return false;
    }
    
    // Update metrics
    host_it->second.metrics = metrics;
    host_it->second.metrics.last_update = std::chrono::system_clock::now();
    
    // Update health state
    auto& health = health_states_[host_id];
    if (validate_host_requirements(metrics)) {
        health.consecutive_failures = 0;
        health.is_degraded = false;
    } else {
        health.consecutive_failures++;
        if (health.consecutive_failures >= 3) {
            health.is_degraded = true;
        }
    }
    
    // Update performance score
    host_it->second.performance_score = calculate_host_score(metrics);
    
    return true;
}

std::vector<uint32_t> HostManager::get_eligible_hosts(uint32_t min_score) {
    std::lock_guard<std::mutex> lock(state_mutex_);
    std::vector<uint32_t> eligible_hosts;
    
    for (const auto& [id, host] : hosts_) {
        if (host.performance_score >= min_score && 
            !health_states_[id].is_degraded &&
            validate_host_capacity(id)) {
            eligible_hosts.push_back(id);
        }
    }
    
    // Sort by performance score
    std::sort(eligible_hosts.begin(), eligible_hosts.end(),
        [this](uint32_t a, uint32_t b) {
            return hosts_[a].performance_score > hosts_[b].performance_score;
        });
    
    return eligible_hosts;
}

std::vector<uint32_t> HostManager::get_nearby_hosts(const GeoLocation& location, double max_distance) {
    std::lock_guard<std::mutex> lock(state_mutex_);
    std::vector<uint32_t> nearby_hosts;
    
    for (const auto& [id, host] : hosts_) {
        if (!health_states_[id].is_degraded && 
            is_within_range(location, host.location, max_distance)) {
            nearby_hosts.push_back(id);
        }
    }
    
    return sort_hosts_by_distance(location, nearby_hosts);
}

bool HostManager::start_player_session(uint32_t host_id, uint32_t char_id, uint32_t map_id) {
    std::lock_guard<std::mutex> lock(state_mutex_);
    
    auto host_it = hosts_.find(host_id);
    if (host_it == hosts_.end()) {
        return false;
    }
    
    // Create new session
    SessionInfo session{
        .char_id = char_id,
        .map_id = map_id,
        .start_time = std::chrono::system_clock::now()
    };
    
    host_it->second.active_sessions.push_back(session);
    return true;
}

bool HostManager::end_player_session(uint32_t host_id, uint32_t char_id) {
    std::lock_guard<std::mutex> lock(state_mutex_);
    
    auto host_it = hosts_.find(host_id);
    if (host_it == hosts_.end()) {
        return false;
    }
    
    auto& sessions = host_it->second.active_sessions;
    auto it = std::find_if(sessions.begin(), sessions.end(),
                          [char_id](const SessionInfo& s) {
                              return s.char_id == char_id;
                          });
    
    if (it != sessions.end()) {
        sessions.erase(it);
        return true;
    }
    
    return false;
}

std::vector<SessionInfo> HostManager::get_active_sessions(uint32_t host_id) {
    std::lock_guard<std::mutex> lock(state_mutex_);
    
    auto host_it = hosts_.find(host_id);
    if (host_it == hosts_.end()) {
        return {};
    }
    
    return host_it->second.active_sessions;
}

uint32_t HostManager::get_session_count(uint32_t host_id) {
    std::lock_guard<std::mutex> lock(state_mutex_);
    
    auto host_it = hosts_.find(host_id);
    if (host_it == hosts_.end()) {
        return 0;
    }
    
    return static_cast<uint32_t>(host_it->second.active_sessions.size());
}

HostInfo* HostManager::get_host_info(uint32_t host_id) {
    std::lock_guard<std::mutex> lock(state_mutex_);
    auto it = hosts_.find(host_id);
    return it != hosts_.end() ? &it->second : nullptr;
}

bool HostManager::is_host_healthy(uint32_t host_id) {
    std::lock_guard<std::mutex> lock(state_mutex_);
    
    auto health_it = health_states_.find(host_id);
    if (health_it == health_states_.end()) {
        return false;
    }
    
    return !health_it->second.is_degraded && 
           health_it->second.consecutive_failures == 0;
}

uint32_t HostManager::get_best_host_for_map(uint32_t map_id) {
    std::vector<uint32_t> eligible = get_eligible_hosts(config_.min_score_threshold);
    if (eligible.empty()) {
        return 0;
    }

    // Get VPS hosts first for critical maps
    bool is_critical = false; // TODO: Implement map criticality check
    if (is_critical) {
        auto vps_it = std::find_if(eligible.begin(), eligible.end(),
            [this](uint32_t id) { return hosts_[id].is_vps; });
        if (vps_it != eligible.end()) {
            return *vps_it;
        }
    }

    // Sort by performance and load
    std::sort(eligible.begin(), eligible.end(),
        [this](uint32_t a, uint32_t b) {
            const auto& host_a = hosts_[a];
            const auto& host_b = hosts_[b];
            float score_a = host_a.performance_score * (1.0f - calculate_host_load(a));
            float score_b = host_b.performance_score * (1.0f - calculate_host_load(b));
            return score_a > score_b;
        });
    
    return eligible[0];
}

double HostManager::calculate_distance(const GeoLocation& loc1, const GeoLocation& loc2) {
    // Haversine formula for calculating distance between two points on a sphere
    const double R = 6371.0; // Earth's radius in kilometers
    const double dLat = (loc2.latitude - loc1.latitude) * M_PI / 180.0;
    const double dLon = (loc2.longitude - loc1.longitude) * M_PI / 180.0;
    
    double a = std::sin(dLat/2) * std::sin(dLat/2) +
               std::cos(loc1.latitude * M_PI/180.0) * std::cos(loc2.latitude * M_PI/180.0) *
               std::sin(dLon/2) * std::sin(dLon/2);
               
    double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1-a));
    return R * c;
}

bool HostManager::is_within_range(const GeoLocation& loc1, const GeoLocation& loc2, 
                                double max_distance) {
    return calculate_distance(loc1, loc2) <= max_distance;
}

std::vector<uint32_t> HostManager::sort_hosts_by_distance(const GeoLocation& location,
                                                       const std::vector<uint32_t>& host_ids) {
    std::vector<uint32_t> sorted = host_ids;
    std::sort(sorted.begin(), sorted.end(),
              [this, &location](uint32_t a, uint32_t b) {
                  const auto& host_a = hosts_[a];
                  const auto& host_b = hosts_[b];
                  double dist_a = calculate_distance(location, host_a.location);
                  double dist_b = calculate_distance(location, host_b.location);
                  return dist_a < dist_b;
              });
    return sorted;
}

void HostManager::check_hosts_health() {
    std::lock_guard<std::mutex> lock(state_mutex_);
    auto now = std::chrono::system_clock::now();
    
    for (auto& [id, health] : health_states_) {
        auto host_it = hosts_.find(id);
        if (host_it == hosts_.end()) continue;
        
        auto& host = host_it->second;
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - host.metrics.last_update).count();
            
        if (elapsed > config_.grace_period) {
            health.consecutive_failures++;
            if (health.consecutive_failures >= 3) {
                mark_host_unhealthy(id);
            }
        }
    }
}

void HostManager::handle_host_failure(uint32_t host_id) {
    std::lock_guard<std::mutex> lock(state_mutex_);
    
    auto host_it = hosts_.find(host_id);
    if (host_it == hosts_.end()) {
        return;
    }
    
    // Mark host as unhealthy
    mark_host_unhealthy(host_id);
    
    // Try to recover if it's a VPS host
    if (host_it->second.is_vps && try_host_recovery(host_id)) {
        std::cout << "VPS host recovered - ID: " << host_id << std::endl;
        return;
    }
    
    // Unregister the host if recovery fails or it's a P2P host
    std::cout << "Host failure handled - ID: " << host_id << std::endl;
    unregister_host(host_id);
}

uint32_t HostManager::calculate_host_score(const HostMetrics& metrics) {
    // Base score calculation
    float score = 100.0f;
    
    // CPU penalty
    if (metrics.cpu_usage > 80.0f) {
        score -= (metrics.cpu_usage - 80.0f) * 2;
    }
    
    // Memory penalty
    if (metrics.memory_usage > 85.0f) {
        score -= (metrics.memory_usage - 85.0f) * 2;
    }
    
    // Latency penalty (more than 100ms starts to reduce score)
    if (metrics.network_latency > 100.0f) {
        score -= (metrics.network_latency - 100.0f) / 10;
    }
    
    // Error penalty
    score -= metrics.error_count * 5;
    
    return static_cast<uint32_t>(std::max(0.0f, std::min(100.0f, score)));
}

void HostManager::update_performance_metrics() {
    std::lock_guard<std::mutex> lock(state_mutex_);
    
    if (hosts_.empty()) {
        performance_metrics_ = {};
        return;
    }
    
    float total_cpu = 0.0f;
    float total_memory = 0.0f;
    float total_latency = 0.0f;
    uint32_t total_sessions = 0;
    uint32_t active_count = 0;

    // Clear previous region metrics
    performance_metrics_.region_metrics.clear();
    
    for (const auto& [id, host] : hosts_) {
        if (!health_states_[id].is_degraded) {
            total_cpu += host.metrics.cpu_usage;
            total_memory += host.metrics.memory_usage;
            total_latency += host.metrics.network_latency;
            total_sessions += host.active_sessions.size();
            active_count++;

            // Update region metrics
            auto& region = performance_metrics_.region_metrics[host.location.region];
            region.host_count++;
            region.player_count += host.active_sessions.size();
            region.average_latency += host.metrics.network_latency;
        }
    }
    
    if (active_count > 0) {
        performance_metrics_.average_cpu = total_cpu / active_count;
        performance_metrics_.average_memory = total_memory / active_count;
        performance_metrics_.average_latency = total_latency / active_count;
        performance_metrics_.total_players = total_sessions;
        performance_metrics_.active_hosts = active_count;

        // Normalize region latency averages
        for (auto& [region, metrics] : performance_metrics_.region_metrics) {
            if (metrics.host_count > 0) {
                metrics.average_latency /= metrics.host_count;
            }
        }
    }
}

void HostManager::log_performance_metrics() {
    std::ostringstream oss;
    oss << "Host Performance Metrics:\n"
        << "Active Hosts: " << performance_metrics_.active_hosts << "\n"
        << "Average CPU: " << performance_metrics_.average_cpu << "%\n"
        << "Average Memory: " << performance_metrics_.average_memory << "%\n"
        << "Average Latency: " << performance_metrics_.average_latency << "ms\n"
        << "Total Players: " << performance_metrics_.total_players << "\n"
        << "\nRegion Metrics:";
    
    for (const auto& [region, metrics] : performance_metrics_.region_metrics) {
        oss << "\n" << region << ":\n"
            << "  Hosts: " << metrics.host_count << "\n"
            << "  Players: " << metrics.player_count << "\n"
            << "  Avg Latency: " << metrics.average_latency << "ms";
    }
    
    std::cout << oss.str() << std::endl;
}

bool HostManager::validate_host_requirements(const HostMetrics& metrics) {
    return metrics.cpu_usage < 90.0f &&
           metrics.memory_usage < 95.0f &&
           metrics.network_latency < 200.0f &&
           metrics.error_count < 10;
}

bool HostManager::validate_host_capacity(uint32_t host_id) {
    auto host_it = hosts_.find(host_id);
    if (host_it == hosts_.end()) {
        return false;
    }
    
    return get_session_count(host_id) < config_.max_players_per_host;
}

void HostManager::mark_host_unhealthy(uint32_t host_id) {
    auto& health = health_states_[host_id];
    health.is_degraded = true;
    health.last_check = std::chrono::system_clock::now();
    
    std::cout << "Host marked unhealthy - ID: " << host_id 
              << ", Failures: " << health.consecutive_failures << std::endl;
}

bool HostManager::try_host_recovery(uint32_t host_id) {
    auto host_it = hosts_.find(host_id);
    if (host_it == hosts_.end() || !host_it->second.is_vps) {
        return false;
    }
    
    // TODO: Implement VPS recovery mechanism
    // This would typically involve:
    // 1. Attempting to restart the host process
    // 2. Verifying connectivity
    // 3. Resyncing state
    
    return false;
}

float HostManager::calculate_host_load(uint32_t host_id) {
    auto host_it = hosts_.find(host_id);
    if (host_it == hosts_.end()) {
        return 1.0f;  // Max load for unknown hosts
    }

    uint32_t session_count = get_session_count(host_id);
    return static_cast<float>(session_count) / config_.max_players_per_host;
}

bool HostManager::mark_map_synced(uint32_t host_id, uint32_t map_id) {
    std::lock_guard<std::mutex> lock(state_mutex_);
    
    auto host_it = hosts_.find(host_id);
    if (host_it == hosts_.end()) {
        return false;
    }
    
    host_it->second.last_sync = std::chrono::system_clock::now();
    return true;
}

bool HostManager::needs_sync(uint32_t host_id, uint32_t map_id) {
    std::lock_guard<std::mutex> lock(state_mutex_);
    
    auto host_it = hosts_.find(host_id);
    if (host_it == hosts_.end()) {
        return true;
    }
    
    auto now = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
        now - host_it->second.last_sync).count();
        
    return elapsed >= config_.sync_interval;
}

void HostManager::distribute_sessions_geographically() {
    std::lock_guard<std::mutex> lock(state_mutex_);
    
    // TODO: Implement geographic-aware session redistribution
    // This would balance sessions across regions while minimizing latency
}

} // namespace p2p
} // namespace rathena