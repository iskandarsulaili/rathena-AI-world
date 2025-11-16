// security_manager.hpp
// Security, anti-cheat, and resource management subsystem for P2P Coordinator

#pragma once

#include <string>
#include <unordered_map>
#include <mutex>
#include <vector>
#include <chrono>

struct CheatFlags {
    int speed_violations = 0;
    int teleport_count = 0;
    int action_spam = 0;
    int resource_anomaly = 0;
    double risk_score = 0.0;
    std::chrono::system_clock::time_point last_flag;
};

class SecurityManager {
public:
    SecurityManager();
    ~SecurityManager();

    // Signature validation (ED25519)
    bool validate_signature(const std::string& peer_id, const std::vector<uint8_t>& message, const std::vector<uint8_t>& signature);

    // Anomaly detection
    void record_movement(const std::string& peer_id, float speed, float max_allowed);
    void record_teleport(const std::string& peer_id);
    void record_action(const std::string& peer_id, int actions_per_sec, int max_allowed);
    void record_resource(const std::string& peer_id, float value, float min_allowed);

    // Reputation management
    void adjust_reputation(const std::string& peer_id, int delta);
    int get_reputation(const std::string& peer_id) const;
    void blacklist_peer(const std::string& peer_id);

    // Load/resource metrics
    void update_worker_load(const std::string& worker_id, double cpu_usage, size_t entity_count);

    // DragonflyDB integration (stub for now)
    void sync_with_dragonfly();

    // Logging and diagnostics
    CheatFlags get_cheat_flags(const std::string& peer_id) const;
    bool is_blacklisted(const std::string& peer_id) const;

private:
    mutable std::mutex sec_mutex_;
    std::unordered_map<std::string, CheatFlags> cheat_flags_;
    std::unordered_map<std::string, int> reputation_;
    std::unordered_map<std::string, bool> blacklist_;
    std::unordered_map<std::string, double> worker_cpu_;
    std::unordered_map<std::string, size_t> worker_entities_;
};