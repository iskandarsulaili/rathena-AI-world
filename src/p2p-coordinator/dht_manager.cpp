// dht_manager.cpp
// Implementation of DHTManager for Kademlia DHT peer discovery and mesh management

#include "dht_manager.hpp"
#include <nlohmann/json.hpp>
#include <iostream>
#include <algorithm>

DHTManager::DHTManager() {}

DHTManager::~DHTManager() {}

void DHTManager::register_peer(const DHTPeerInfo& peer) {
    std::lock_guard<std::mutex> lock(dht_mutex_);
    peers_[peer.peer_id] = peer;
    nlohmann::json log = {
        {"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())},
        {"level", "INFO"},
        {"event", "dht_peer_registered"},
        {"peer_id", peer.peer_id},
        {"endpoint", peer.endpoint},
        {"reputation", peer.reputation}
    };
    std::cout << log.dump() << std::endl;
}

void DHTManager::remove_peer(const std::string& peer_id) {
    std::lock_guard<std::mutex> lock(dht_mutex_);
    peers_.erase(peer_id);
    nlohmann::json log = {
        {"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())},
        {"level", "INFO"},
        {"event", "dht_peer_removed"},
        {"peer_id", peer_id}
    };
    std::cout << log.dump() << std::endl;
}

std::vector<DHTPeerInfo> DHTManager::find_peers(const std::string& capability, size_t max_results) {
    std::lock_guard<std::mutex> lock(dht_mutex_);
    std::vector<DHTPeerInfo> result;
    for (const auto& kv : peers_) {
        if (std::find(kv.second.capabilities.begin(), kv.second.capabilities.end(), capability) != kv.second.capabilities.end()) {
            result.push_back(kv.second);
            if (result.size() >= max_results) break;
        }
    }
    nlohmann::json log = {
        {"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())},
        {"level", "DEBUG"},
        {"event", "dht_find_peers"},
        {"capability", capability},
        {"result_count", result.size()}
    };
    std::cout << log.dump() << std::endl;
    return result;
}

std::vector<DHTPeerInfo> DHTManager::kademlia_lookup(const std::string& target_id, size_t alpha) {
    std::lock_guard<std::mutex> lock(dht_mutex_);
    // Stub: return up to alpha closest peers (by string distance for now)
    std::vector<DHTPeerInfo> all_peers;
    for (const auto& kv : peers_) {
        all_peers.push_back(kv.second);
    }
    std::sort(all_peers.begin(), all_peers.end(), [&](const DHTPeerInfo& a, const DHTPeerInfo& b) {
        return a.peer_id < b.peer_id; // Replace with XOR distance for real Kademlia
    });
    std::vector<DHTPeerInfo> result;
    for (size_t i = 0; i < std::min(alpha, all_peers.size()); ++i) {
        result.push_back(all_peers[i]);
    }
    nlohmann::json log = {
        {"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())},
        {"level", "DEBUG"},
        {"event", "dht_kademlia_lookup"},
        {"target_id", target_id},
        {"result_count", result.size()}
    };
    std::cout << log.dump() << std::endl;
    return result;
}

void DHTManager::tick() {
    std::lock_guard<std::mutex> lock(dht_mutex_);
    // Example: prune stale peers (stub)
    nlohmann::json log = {
        {"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())},
        {"level", "DEBUG"},
        {"event", "dht_tick"},
        {"peer_count", peers_.size()}
    };
    std::cout << log.dump() << std::endl;
}

std::vector<DHTPeerInfo> DHTManager::get_all_peers() const {
    std::lock_guard<std::mutex> lock(dht_mutex_);
    std::vector<DHTPeerInfo> result;
    for (const auto& kv : peers_) {
        result.push_back(kv.second);
    }
    return result;
}