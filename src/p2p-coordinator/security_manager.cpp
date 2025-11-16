// security_manager.cpp
// Implementation of SecurityManager for P2P Coordinator

#include "security_manager.hpp"
#include <nlohmann/json.hpp>
#include <iostream>
#include <cmath>

SecurityManager::SecurityManager() {}

SecurityManager::~SecurityManager() {}

bool SecurityManager::validate_signature(const std::string& peer_id, const std::vector<uint8_t>& message, const std::vector<uint8_t>& signature) {
    // TODO: Implement ED25519 signature validation
    // For now, always return true (stub)
    nlohmann::json log = {
        {"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())},
        {"level", "DEBUG"},
        {"event", "signature_validation"},
        {"peer_id", peer_id},
        {"result", "stub_true"}
    };
    std::cout << log.dump() << std::endl;
    return true;
}

void SecurityManager::record_movement(const std::string& peer_id, float speed, float max_allowed) {
    std::lock_guard<std::mutex> lock(sec_mutex_);
    if (speed > max_allowed) {
        cheat_flags_[peer_id].speed_violations++;
        cheat_flags_[peer_id].last_flag = std::chrono::system_clock::now();
        cheat_flags_[peer_id].risk_score += 0.2;
        nlohmann::json log = {
            {"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())},
            {"level", "WARN"},
            {"event", "speed_violation"},
            {"peer_id", peer_id},
            {"speed", speed},
            {"max_allowed", max_allowed}
        };
        std::cout << log.dump() << std::endl;
    }
}

void SecurityManager::record_teleport(const std::string& peer_id) {
    std::lock_guard<std::mutex> lock(sec_mutex_);
    cheat_flags_[peer_id].teleport_count++;
    cheat_flags_[peer_id].last_flag = std::chrono::system_clock::now();
    cheat_flags_[peer_id].risk_score += 0.3;
    nlohmann::json log = {
        {"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())},
        {"level", "WARN"},
        {"event", "teleport_detected"},
        {"peer_id", peer_id}
    };
    std::cout << log.dump() << std::endl;
}

void SecurityManager::record_action(const std::string& peer_id, int actions_per_sec, int max_allowed) {
    std::lock_guard<std::mutex> lock(sec_mutex_);
    if (actions_per_sec > max_allowed) {
        cheat_flags_[peer_id].action_spam++;
        cheat_flags_[peer_id].last_flag = std::chrono::system_clock::now();
        cheat_flags_[peer_id].risk_score += 0.15;
        nlohmann::json log = {
            {"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())},
            {"level", "WARN"},
            {"event", "action_spam"},
            {"peer_id", peer_id},
            {"actions_per_sec", actions_per_sec},
            {"max_allowed", max_allowed}
        };
        std::cout << log.dump() << std::endl;
    }
}

void SecurityManager::record_resource(const std::string& peer_id, float value, float min_allowed) {
    std::lock_guard<std::mutex> lock(sec_mutex_);
    if (value < min_allowed) {
        cheat_flags_[peer_id].resource_anomaly++;
        cheat_flags_[peer_id].last_flag = std::chrono::system_clock::now();
        cheat_flags_[peer_id].risk_score += 0.25;
        nlohmann::json log = {
            {"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())},
            {"level", "WARN"},
            {"event", "resource_anomaly"},
            {"peer_id", peer_id},
            {"value", value},
            {"min_allowed", min_allowed}
        };
        std::cout << log.dump() << std::endl;
    }
}

void SecurityManager::adjust_reputation(const std::string& peer_id, int delta) {
    std::lock_guard<std::mutex> lock(sec_mutex_);
    reputation_[peer_id] += delta;
    nlohmann::json log = {
        {"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())},
        {"level", "INFO"},
        {"event", "reputation_adjusted"},
        {"peer_id", peer_id},
        {"delta", delta},
        {"new_score", reputation_[peer_id]}
    };
    std::cout << log.dump() << std::endl;
}

int SecurityManager::get_reputation(const std::string& peer_id) const {
    std::lock_guard<std::mutex> lock(sec_mutex_);
    auto it = reputation_.find(peer_id);
    return (it != reputation_.end()) ? it->second : 0;
}

void SecurityManager::blacklist_peer(const std::string& peer_id) {
    std::lock_guard<std::mutex> lock(sec_mutex_);
    blacklist_[peer_id] = true;
    nlohmann::json log = {
        {"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())},
        {"level", "ERROR"},
        {"event", "peer_blacklisted"},
        {"peer_id", peer_id}
    };
    std::cout << log.dump() << std::endl;
}

void SecurityManager::update_worker_load(const std::string& worker_id, double cpu_usage, size_t entity_count) {
    std::lock_guard<std::mutex> lock(sec_mutex_);
    worker_cpu_[worker_id] = cpu_usage;
    worker_entities_[worker_id] = entity_count;
    nlohmann::json log = {
        {"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())},
        {"level", "DEBUG"},
        {"event", "worker_load_update"},
        {"worker_id", worker_id},
        {"cpu_usage", cpu_usage},
        {"entity_count", entity_count}
    };
    std::cout << log.dump() << std::endl;
}

void SecurityManager::sync_with_dragonfly() {
    // TODO: Implement DragonflyDB integration for cheat flags, reputation, and load metrics
}

CheatFlags SecurityManager::get_cheat_flags(const std::string& peer_id) const {
    std::lock_guard<std::mutex> lock(sec_mutex_);
    auto it = cheat_flags_.find(peer_id);
    return (it != cheat_flags_.end()) ? it->second : CheatFlags{};
}

bool SecurityManager::is_blacklisted(const std::string& peer_id) const {
    std::lock_guard<std::mutex> lock(sec_mutex_);
    auto it = blacklist_.find(peer_id);
    return (it != blacklist_.end()) ? it->second : false;
}