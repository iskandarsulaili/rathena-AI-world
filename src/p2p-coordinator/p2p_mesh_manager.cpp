// p2p_mesh_manager.cpp
// Implementation of P2PMeshManager for peer discovery, mesh topology, and AOI management

#include "p2p_mesh_manager.hpp"
#include <cmath>
#include <algorithm>
#include <nlohmann/json.hpp>
#include <iostream>

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
        {"endpoint", peer.quic_endpoint}
    };
    std::cout << log.dump() << std::endl;
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
            {"peer_id", peer.peer_id}
        };
        std::cout << log.dump() << std::endl;
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
    for (const auto& kv : peers_) {
        if (kv.second.reputation < 100) { // Example threshold
            to_remove.push_back(kv.first);
        }
    }
    for (const auto& peer_id : to_remove) {
        peers_.erase(peer_id);
        nlohmann::json log = {
            {"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())},
            {"level", "WARN"},
            {"event", "peer_pruned"},
            {"peer_id", peer_id}
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
}

void P2PMeshManager::sync_with_dragonfly() {
    // TODO: Implement DragonflyDB integration for peer metadata
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