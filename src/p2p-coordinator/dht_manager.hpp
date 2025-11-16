// dht_manager.hpp
// Kademlia DHT peer discovery and mesh management for P2P Coordinator

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>

struct DHTPeerInfo {
    std::string peer_id;
    std::string endpoint;
    std::vector<std::string> capabilities;
    int reputation;
};

class DHTManager {
public:
    DHTManager();
    ~DHTManager();

    // Register or update a peer in the DHT
    void register_peer(const DHTPeerInfo& peer);

    // Remove a peer from the DHT
    void remove_peer(const std::string& peer_id);

    // Find peers by capability or interest
    std::vector<DHTPeerInfo> find_peers(const std::string& capability, size_t max_results = 20);

    // Kademlia-style lookup
    std::vector<DHTPeerInfo> kademlia_lookup(const std::string& target_id, size_t alpha = 3);

    // Periodic maintenance (refresh buckets, prune stale)
    void tick();

    // Thread-safe access to all peers
    std::vector<DHTPeerInfo> get_all_peers() const;

private:
    mutable std::mutex dht_mutex_;
    std::unordered_map<std::string, DHTPeerInfo> peers_;
    // Kademlia buckets, etc. (stub for now)
};