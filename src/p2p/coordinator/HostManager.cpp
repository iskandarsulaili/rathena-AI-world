#include "HostManager.hpp"
#include <algorithm>
#include <sstream>
#include <iostream>

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

uint32_t HostManager::register_host(const std::string& address, uint16_t port, bool is_vps) {
    std::lock_guard<std::mutex> lock(state_mutex_);
    
    // Create new host info
    HostInfo host;
    host.id = next_host_id_++;
    host.address = address;
    host.port = port;
    host.is_vps = is_vps;
    host.performance_score = 0;
    host.registration_time = std::chrono::system_clock::now();
    
    // Generate authentication token
    host.auth_token = TokenGenerator::generate_auth_token();
    
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
              << (is_vps ? " (VPS)" : " (P2P)") << std::endl;
              
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
    uint32_t total_players = 0;
    uint32_t active_count = 0;
    
    for (const auto& [id, host] : hosts_) {
        if (!health_states_[id].is_degraded) {
            total_cpu += host.metrics.cpu_usage;
            total_memory += host.metrics.memory_usage;
            total_latency += host.metrics.network_latency;
            total_players += host.metrics.player_count;
            active_count++;
        }
    }
    
    if (active_count > 0) {
        performance_metrics_.average_cpu = total_cpu / active_count;
        performance_metrics_.average_memory = total_memory / active_count;
        performance_metrics_.average_latency = total_latency / active_count;
        performance_metrics_.total_players = total_players;
        performance_metrics_.active_hosts = active_count;
    }
}

void HostManager::log_performance_metrics() {
    std::ostringstream oss;
    oss << "Host Performance Metrics:\n"
        << "Active Hosts: " << performance_metrics_.active_hosts << "\n"
        << "Average CPU: " << performance_metrics_.average_cpu << "%\n"
        << "Average Memory: " << performance_metrics_.average_memory << "%\n"
        << "Average Latency: " << performance_metrics_.average_latency << "ms\n"
        << "Total Players: " << performance_metrics_.total_players;
    
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
    
    return host_it->second.metrics.player_count < config_.max_players_per_host;
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

} // namespace p2p
} // namespace rathena