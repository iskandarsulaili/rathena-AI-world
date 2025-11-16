// p2p_mesh_manager.cpp
// Implementation of P2PMeshManager for peer discovery, mesh topology, and AOI management

#include "p2p_mesh_manager.hpp"
#include <cmath>
#include <algorithm>
#include <nlohmann/json.hpp>
#include <iostream>
#include "dht_manager.hpp"
#include "metrics_exporter.hpp"

extern std::shared_ptr<DHTManager> g_dht_manager;
extern std::shared_ptr<MetricsExporter> g_metrics_exporter;

P2PMeshManager::P2PMeshManager() {}

P2PMeshManager::~P2PMeshManager() {}

void P2PMeshManager::register_peer(const PeerInfo& peer) {
    std::lock_guard<std::mutex> lock(mesh_mutex_);
    peers_[peer.peer_id] = peer;
    nlohmann::json log = {
        {"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())},
        {"level", "INFO"},
        {"event", "peer_registered"},
        {"peer_id", peer.peer_id},
        {"endpoint", peer.quic_endpoint},
        {"reputation", peer.reputation}
    };
    std::cout << log.dump() << std::endl;
    // Register peer in DHT
    if (g_dht_manager) {
        DHTPeerInfo dht_peer;
        dht_peer.peer_id = peer.peer_id;
        dht_peer.endpoint = peer.quic_endpoint;
        dht_peer.capabilities = peer.capabilities;
        dht_peer.reputation = peer.reputation;
        g_dht_manager->register_peer(dht_peer);
    }
    // Export metric
    if (g_metrics_exporter) {
        MetricEvent ev;
        ev.name = "peer_registered";
        ev.type = MetricType::COUNTER;
        ev.labels = {peer.peer_id};
        ev.value = 1;
        ev.attributes = log;
        g_metrics_exporter->export_metric(ev);
    }
    // Anomaly detection: flag if reputation is low or peer is blacklisted
    extern SecurityManager g_security_manager;
    if (g_security_manager.is_blacklisted(peer.peer_id) || peer.reputation < 50) {
        nlohmann::json anomaly_log = {
            {"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())},
            {"level", "WARN"},
            {"event", "peer_anomaly_flagged"},
            {"peer_id", peer.peer_id},
            {"reputation", peer.reputation},
            {"blacklisted", g_security_manager.is_blacklisted(peer.peer_id)}
        };
        std::cout << anomaly_log.dump() << std::endl;
        if (g_metrics_exporter) {
            MetricEvent ev;
            ev.name = "peer_anomaly_flagged";
            ev.type = MetricType::COUNTER;
            ev.labels = {peer.peer_id};
            ev.value = 1;
            ev.attributes = anomaly_log;
            g_metrics_exporter->export_metric(ev);
        }
        // Optionally, deprioritize or disconnect
    }
}

void P2PMeshManager::update_peer(const PeerInfo& peer) {
    std::lock_guard<std::mutex> lock(mesh_mutex_);
    auto it = peers_.find(peer.peer_id);
    if (it != peers_.end()) {
        it->second = peer;
        nlohmann::json log = {
            {"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())},
            {"level", "DEBUG"},
            {"event", "peer_updated"},
            {"peer_id", peer.peer_id},
            {"reputation", peer.reputation}
        };
        std::cout << log.dump() << std::endl;
        // Update peer in DHT
        if (g_dht_manager) {
            DHTPeerInfo dht_peer;
            dht_peer.peer_id = peer.peer_id;
            dht_peer.endpoint = peer.quic_endpoint;
            dht_peer.capabilities = peer.capabilities;
            dht_peer.reputation = peer.reputation;
            g_dht_manager->register_peer(dht_peer);
        }
        // Export metric
        if (g_metrics_exporter) {
            MetricEvent ev;
            ev.name = "peer_updated";
            ev.type = MetricType::COUNTER;
            ev.labels = {peer.peer_id};
            ev.value = 1;
            ev.attributes = log;
            g_metrics_exporter->export_metric(ev);
        }
        // Peer reputation system: decay, correction frequency, mesh prioritization
        extern SecurityManager g_security_manager;
        int rep = g_security_manager.get_reputation(peer.peer_id);
        if (rep != peer.reputation) {
            nlohmann::json rep_log = {
                {"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())},
                {"level", "INFO"},
                {"event", "reputation_sync"},
                {"peer_id", peer.peer_id},
                {"old_score", peer.reputation},
                {"new_score", rep}
            };
            std::cout << rep_log.dump() << std::endl;
            it->second.reputation = rep;
        }
        // Decay reputation if peer is inactive or flagged
        CheatFlags flags = g_security_manager.get_cheat_flags(peer.peer_id);
        if (flags.risk_score > 1.0) {
            g_security_manager.adjust_reputation(peer.peer_id, -5);
            nlohmann::json decay_log = {
                {"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())},
                {"level", "WARN"},
                {"event", "reputation_decay"},
                {"peer_id", peer.peer_id},
                {"risk_score", flags.risk_score}
            };
            std::cout << decay_log.dump() << std::endl;
            if (g_metrics_exporter) {
                MetricEvent ev;
                ev.name = "reputation_decay";
                ev.type = MetricType::COUNTER;
                ev.labels = {peer.peer_id};
                ev.value = 1;
                ev.attributes = decay_log;
                g_metrics_exporter->export_metric(ev);
            }
        }
    }
}

void P2PMeshManager::remove_peer(const std::string& peer_id) {
    std::lock_guard<std::mutex> lock(mesh_mutex_);
    peers_.erase(peer_id);
    nlohmann::json log = {
        {"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())},
        {"level", "INFO"},
        {"event", "peer_removed"},
        {"peer_id", peer_id}
    };
    std::cout << log.dump() << std::endl;
}

std::vector<PeerInfo> P2PMeshManager::get_peers_in_zone(float x, float y, float z, AOIZone zone) const {
    std::lock_guard<std::mutex> lock(mesh_mutex_);
    std::vector<PeerInfo> result;
    float radius = 0.0f;
    switch (zone) {
        case AOIZone::CRITICAL: radius = 100.0f; break;
        case AOIZone::EXTENDED: radius = 500.0f; break;
        case AOIZone::PERIPHERAL: radius = 1000.0f; break;
    }
    for (const auto& kv : peers_) {
        const PeerInfo& peer = kv.second;
        float dx = peer.x - x, dy = peer.y - y, dz = peer.z - z;
        float dist = std::sqrt(dx*dx + dy*dy + dz*dz);
        if (dist <= radius) {
            result.push_back(peer);
        }
    }
    return result;
}

void P2PMeshManager::prune_low_score_peers() {
    std::lock_guard<std::mutex> lock(mesh_mutex_);
    std::vector<std::string> to_remove;
    extern SecurityManager g_security_manager;
    for (const auto& kv : peers_) {
        bool is_blacklisted = g_security_manager.is_blacklisted(kv.first);
        if (is_blacklisted || kv.second.reputation < 100) { // Blacklist or low reputation
            to_remove.push_back(kv.first);
        }
    }
    for (const auto& peer_id : to_remove) {
        peers_.erase(peer_id);
        nlohmann::json log = {
            {"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())},
            {"level", "WARN"},
            {"event", "peer_pruned"},
            {"peer_id", peer_id},
            {"reason", g_security_manager.is_blacklisted(peer_id) ? "blacklisted" : "low_reputation"}
        };
        std::cout << log.dump() << std::endl;
    }
}

void P2PMeshManager::refresh_mesh() {
    // Example: remove stale peers (not updated in last 30s)
    std::lock_guard<std::mutex> lock(mesh_mutex_);
    auto now = std::chrono::steady_clock::now();
    std::vector<std::string> to_remove;
    for (const auto& kv : peers_) {
        auto age = std::chrono::duration_cast<std::chrono::seconds>(now - kv.second.last_update).count();
        if (age > 30) {
            to_remove.push_back(kv.first);
        }
    }
    for (const auto& peer_id : to_remove) {
        peers_.erase(peer_id);
        nlohmann::json log = {
            {"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())},
            {"level", "INFO"},
            {"event", "peer_expired"},
            {"peer_id", peer_id}
        };
        std::cout << log.dump() << std::endl;
    }
    
    void P2PMeshManager::discover_peers(const std::string& target_id) {
        std::lock_guard<std::mutex> lock(mesh_mutex_);
        // DHT/Kademlia peer discovery logic
        nlohmann::json log = {
            {"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())},
            {"level", "INFO"},
            {"event", "peer_discovery"},
            {"target_id", target_id}
        };
        std::cout << log.dump() << std::endl;
        // Example: lookup in dht_table_
        for (const auto& kv : dht_table_) {
            for (const auto& pid : kv.second) {
                if (pid == target_id) {
                    log["event"] = "peer_found";
                    log["interest"] = kv.first;
                    std::cout << log.dump() << std::endl;
                    return;
                }
            }
        }
        log["event"] = "peer_not_found";
        std::cout << log.dump() << std::endl;
    }
    
    void P2PMeshManager::update_dht(const PeerInfo& peer) {
        std::lock_guard<std::mutex> lock(mesh_mutex_);
        for (const auto& cap : peer.capabilities) {
            dht_table_[cap].push_back(peer.peer_id);
        }
        nlohmann::json log = {
            {"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())},
            {"level", "DEBUG"},
            {"event", "dht_updated"},
            {"peer_id", peer.peer_id},
            {"capabilities", peer.capabilities}
        };
        std::cout << log.dump() << std::endl;
    }
    
    std::vector<PeerInfo> P2PMeshManager::query_dht(const std::string& interest) {
        std::lock_guard<std::mutex> lock(mesh_mutex_);
        std::vector<PeerInfo> result;
        auto it = dht_table_.find(interest);
        if (it != dht_table_.end()) {
            for (const auto& pid : it->second) {
                auto peer_it = peers_.find(pid);
                if (peer_it != peers_.end()) {
                    result.push_back(peer_it->second);
                }
            }
        }
        nlohmann::json log = {
            {"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())},
            {"level", "INFO"},
            {"event", "dht_query"},
            {"interest", interest},
            {"result_count", result.size()}
        };
        std::cout << log.dump() << std::endl;
        return result;
    }
    
    float P2PMeshManager::score_peer(const PeerInfo& peer, float x, float y, float z) const {
        // AOI scoring: proximity, reputation, bandwidth
        float dx = peer.x - x, dy = peer.y - y, dz = peer.z - z;
        float dist = std::sqrt(dx*dx + dy*dy + dz*dz);
        float score = 0.0f;
        if (dist < 100.0f) score += 1.0f;
        else if (dist < 500.0f) score += 0.5f;
        else score += 0.1f;
        score += peer.reputation / 1000.0f;
        score += peer.bandwidth / 100000.0f;
        return score;
    }
    
    void P2PMeshManager::prune_by_score(float min_score, float x, float y, float z) {
        std::lock_guard<std::mutex> lock(mesh_mutex_);
        std::vector<std::string> to_remove;
        for (const auto& kv : peers_) {
            float s = score_peer(kv.second, x, y, z);
            if (s < min_score) {
                to_remove.push_back(kv.first);
            }
        }
        for (const auto& peer_id : to_remove) {
            peers_.erase(peer_id);
            nlohmann::json log = {
                {"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())},
                {"level", "WARN"},
                {"event", "peer_pruned_by_score"},
                {"peer_id", peer_id}
            };
            std::cout << log.dump() << std::endl;
        }
    }
    
    std::string P2PMeshManager::geohash(float x, float y) const {
        // Simple geohash stub (replace with real geohash algorithm)
        int ix = static_cast<int>(x);
        int iy = static_cast<int>(y);
        return std::to_string(ix) + "_" + std::to_string(iy);
    }
    
    std::vector<PeerInfo> P2PMeshManager::geohash_query(float x, float y, float z, float radius) {
        std::lock_guard<std::mutex> lock(mesh_mutex_);
        std::vector<PeerInfo> result;
        std::string target_hash = geohash(x, y);
        for (const auto& kv : peers_) {
            std::string peer_hash = geohash(kv.second.x, kv.second.y);
            if (peer_hash == target_hash) {
                float dx = kv.second.x - x, dy = kv.second.y - y, dz = kv.second.z - z;
                float dist = std::sqrt(dx*dx + dy*dy + dz*dz);
                if (dist <= radius) {
                    result.push_back(kv.second);
                }
            }
        }
        nlohmann::json log = {
            {"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())},
            {"level", "INFO"},
            {"event", "geohash_query"},
            {"target_hash", target_hash},
            {"radius", radius},
            {"result_count", result.size()}
        };
        std::cout << log.dump() << std::endl;
        return result;
    }
}

void P2PMeshManager::sync_with_dragonfly() {
    // DragonflyDB integration for peer metadata, geohash, mesh state, and anomaly/risk scoring
    nlohmann::json log = {
        {"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())},
        {"level", "INFO"},
        {"event", "dragonfly_sync_start"},
        {"peer_count", peers_.size()}
    };
    std::cout << log.dump() << std::endl;
    // Pseudocode: connect to DragonflyDB and sync
    // Actual implementation should use a C++ Redis client (e.g., redis-plus-plus)
    // for (const auto& kv : peers_) {
    //     dragonfly_client.geoadd("peers", kv.second.x, kv.second.y, kv.second.peer_id);
    //     dragonfly_client.hset("peer:" + kv.second.peer_id, ...);
    //     // Also sync risk_score, anomaly flags, etc.
    // }
    // Example: log anomaly/risk metrics for Prometheus/Loki
    for (const auto& kv : peers_) {
        extern SecurityManager g_security_manager;
        CheatFlags flags = g_security_manager.get_cheat_flags(kv.first);
        nlohmann::json metrics_log = {
            {"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())},
            {"level", "INFO"},
            {"event", "peer_metrics"},
            {"peer_id", kv.first},
            {"risk_score", flags.risk_score},
            {"speed_violations", flags.speed_violations},
            {"teleport_count", flags.teleport_count},
            {"action_spam", flags.action_spam},
            {"resource_anomaly", flags.resource_anomaly}
        };
        std::cout << metrics_log.dump() << std::endl;
        // Prometheus/Loki metrics
        nlohmann::json prom_metrics = {
            {"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())},
            {"metric", "cheat_flags"},
            {"peer_id", kv.first},
            {"risk_score", flags.risk_score},
            {"speed_violations", flags.speed_violations},
            {"teleport_count", flags.teleport_count},
            {"action_spam", flags.action_spam},
            {"resource_anomaly", flags.resource_anomaly}
        };
        std::cout << prom_metrics.dump() << std::endl;
        nlohmann::json rep_metric = {
            {"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())},
            {"metric", "peer_reputation"},
            {"peer_id", kv.first},
            {"value", kv.second.reputation}
        };
        std::cout << rep_metric.dump() << std::endl;
    }
    log["event"] = "dragonfly_sync_complete";
    std::cout << log.dump() << std::endl;
}

size_t P2PMeshManager::peer_count() const {
    std::lock_guard<std::mutex> lock(mesh_mutex_);
    return peers_.size();
}

std::vector<PeerInfo> P2PMeshManager::get_all_peers() const {
    std::lock_guard<std::mutex> lock(mesh_mutex_);
    std::vector<PeerInfo> result;
    for (const auto& kv : peers_) {
        result.push_back(kv.second);
    }
    return result;
}