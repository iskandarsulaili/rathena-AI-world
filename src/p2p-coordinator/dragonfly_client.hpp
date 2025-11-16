// dragonfly_client.hpp
// Thread-safe DragonflyDB (redis-plus-plus) client for peer metadata, reputation, cheat flags, AOI geospatial queries

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <sw/redis++/redis++.h>
#include <nlohmann/json.hpp>

struct PeerGeoInfo {
    std::string peer_id;
    double x, y, z;
    std::string endpoint;
    int reputation;
    double risk_score;
    std::vector<std::string> capabilities;
};

class DragonflyClient {
public:
    DragonflyClient(const std::string& host, int port);
    ~DragonflyClient();

    // Peer metadata
    void register_peer(const PeerGeoInfo& info);
    void update_peer(const PeerGeoInfo& info);
    void remove_peer(const std::string& peer_id);

    // Reputation and cheat flags
    void set_reputation(const std::string& peer_id, int reputation);
    int get_reputation(const std::string& peer_id);
    void set_cheat_flags(const std::string& peer_id, const nlohmann::json& flags);
    nlohmann::json get_cheat_flags(const std::string& peer_id);

    // AOI geospatial queries
    std::vector<PeerGeoInfo> geosearch(double x, double y, double radius);

    // Structured logging for all operations
    void log_operation(const std::string& op, const nlohmann::json& details);

private:
    std::shared_ptr<sw::redis::Redis> redis_;
    std::mutex client_mutex_;
    std::string geo_key_;
};