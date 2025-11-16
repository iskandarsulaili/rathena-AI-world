// security_manager.cpp
// Implementation of SecurityManager for P2P Coordinator

#include "security_manager.hpp"
#include <nlohmann/json.hpp>
#include <iostream>
#include <cmath>
#include "metrics_exporter.hpp"

extern std::shared_ptr<MetricsExporter> g_metrics_exporter;

SecurityManager::SecurityManager() {}

SecurityManager::~SecurityManager() {}

bool SecurityManager::validate_signature(const std::string& peer_id, const std::vector<uint8_t>& message, const std::vector<uint8_t>& signature) {
    // ED25519 signature validation using OpenSSL
    bool valid = false;
    std::string result_msg;
    std::vector<uint8_t> pubkey = get_peer_public_key(peer_id);

    if (pubkey.size() != 32 || signature.size() != 64) {
        result_msg = "invalid_key_or_signature_size";
    } else {
        EVP_PKEY* pkey = EVP_PKEY_new_raw_public_key(EVP_PKEY_ED25519, nullptr, pubkey.data(), pubkey.size());
        EVP_MD_CTX* md_ctx = EVP_MD_CTX_new();
        if (pkey && md_ctx) {
            if (EVP_DigestVerifyInit(md_ctx, nullptr, nullptr, nullptr, pkey) == 1) {
                if (EVP_DigestVerify(md_ctx, signature.data(), signature.size(), message.data(), message.size()) == 1) {
                    valid = true;
                    result_msg = "valid";
                } else {
                    result_msg = "invalid_signature";
                }
            } else {
                result_msg = "verify_init_failed";
            }
        } else {
            result_msg = "openssl_alloc_failed";
        }
        if (md_ctx) EVP_MD_CTX_free(md_ctx);
        if (pkey) EVP_PKEY_free(pkey);
    }

    nlohmann::json log = {
        {"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())},
        {"level", valid ? "INFO" : "ERROR"},
        {"event", "signature_validation"},
        {"peer_id", peer_id},
        {"result", result_msg}
    };
    std::cout << log.dump() << std::endl;

    if (!valid) {
        blacklist_peer(peer_id);
    }
    return valid;
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
        if (g_metrics_exporter) {
            MetricEvent ev;
            ev.name = "speed_violation";
            ev.type = MetricType::COUNTER;
            ev.labels = {peer_id};
            ev.value = 1;
            ev.attributes = log;
            g_metrics_exporter->export_metric(ev);
        }
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
        if (g_metrics_exporter) {
            MetricEvent ev;
            ev.name = "action_spam";
            ev.type = MetricType::COUNTER;
            ev.labels = {peer_id};
            ev.value = 1;
            ev.attributes = log;
            g_metrics_exporter->export_metric(ev);
        }
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
        if (g_metrics_exporter) {
            MetricEvent ev;
            ev.name = "resource_anomaly";
            ev.type = MetricType::COUNTER;
            ev.labels = {peer_id};
            ev.value = 1;
            ev.attributes = log;
            g_metrics_exporter->export_metric(ev);
        }
    }
}

void SecurityManager::adjust_reputation(const std::string& peer_id, int delta) {
    std::lock_guard<std::mutex> lock(sec_mutex_);
    int old_score = reputation_[peer_id];
    reputation_[peer_id] += delta;
    nlohmann::json log = {
        {"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())},
        {"level", "INFO"},
        {"event", "reputation_adjusted"},
        {"peer_id", peer_id},
        {"delta", delta},
        {"old_score", old_score},
        {"new_score", reputation_[peer_id]}
    };
    std::cout << log.dump() << std::endl;
    // Structured event for Prometheus/Loki
    if (g_metrics_exporter) {
        MetricEvent ev;
        ev.name = "peer_reputation";
        ev.type = MetricType::GAUGE;
        ev.labels = {peer_id};
        ev.value = reputation_[peer_id];
        ev.attributes = log;
        g_metrics_exporter->export_metric(ev);
    }
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
    // Structured event for Prometheus/Loki
    if (g_metrics_exporter) {
        MetricEvent ev;
        ev.name = "peer_blacklisted";
        ev.type = MetricType::COUNTER;
        ev.labels = {peer_id};
        ev.value = 1;
        ev.attributes = log;
        g_metrics_exporter->export_metric(ev);
    }
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
    // TODO: Implement DragonflyDB integration for cheat flags, reputation, load metrics, and public keys
    // Example: Load peer public keys from DragonflyDB
    load_peer_public_keys_from_dragonfly();
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