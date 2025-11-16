// p2p_mesh_manager.hpp
// P2P Mesh Manager for peer discovery, mesh topology, and AOI management

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <chrono>

struct PeerInfo {
    std::string peer_id;
    std::string quic_endpoint;
    float x, y, z;
    std::chrono::steady_clock::time_point last_update;
    std::vector<std::string> capabilities;
    int bandwidth; // bytes/sec
    int reputation;
};

enum class AOIZone {
    CRITICAL,
    EXTENDED,
    PERIPHERAL
};

class P2PMeshManager {
public:
    P2PMeshManager();
    ~P2PMeshManager();

    // Peer discovery and registration
    void register_peer(const PeerInfo& peer);
    void update_peer(const PeerInfo& peer);
    void remove_peer(const std::string& peer_id);

    // Query peers by AOI/interest zone
    std::vector<PeerInfo> get_peers_in_zone(float x, float y, float z, AOIZone zone) const;

    // Mesh maintenance (pruning, scoring, refresh)
    void prune_low_score_peers();
    void refresh_mesh();

    // DragonflyDB integration (stub for now)
    void sync_with_dragonfly();

    // Logging and diagnostics
    size_t peer_count() const;
    std::vector<PeerInfo> get_all_peers() const;

private:
    mutable std::mutex mesh_mutex_;
    std::unordered_map<std::string, PeerInfo> peers_;
};