// dragonfly_client.cpp
// Implementation of DragonflyClient for redis-plus-plus integration

#include "dragonfly_client.hpp"
#include <iostream>
#include <sstream>
#include <chrono>

DragonflyClient::DragonflyClient(const std::string& host, int port)
    : geo_key_("peers")
{
    std::ostringstream url;
    url << "tcp://" << host << ":" << port;
    redis_ = std::make_shared<sw::redis::Redis>(url.str());
}

DragonflyClient::~DragonflyClient() {}

void DragonflyClient::register_peer(const PeerGeoInfo& info) {
    std::lock_guard<std::mutex> lock(client_mutex_);
    // GEOADD for geospatial index
    redis_->geoadd(geo_key_, { {info.x, info.y, info.peer_id} });
    // HSET for peer metadata
    redis_->hset("peer:" + info.peer_id, "endpoint", info.endpoint);
    redis_->hset("peer:" + info.peer_id, "reputation", std::to_string(info.reputation));
    redis_->hset("peer:" + info.peer_id, "risk_score", std::to_string(info.risk_score));
    redis_->hset("peer:" + info.peer_id, "capabilities", nlohmann::json(info.capabilities).dump());
    log_operation("register_peer", {{"peer_id", info.peer_id}});
}

void DragonflyClient::update_peer(const PeerGeoInfo& info) {
    register_peer(info); // Same as register for upsert
    log_operation("update_peer", {{"peer_id", info.peer_id}});
}

void DragonflyClient::remove_peer(const std::string& peer_id) {
    std::lock_guard<std::mutex> lock(client_mutex_);
    redis_->zrem(geo_key_, peer_id);
    redis_->del("peer:" + peer_id);
    log_operation("remove_peer", {{"peer_id", peer_id}});
}

void DragonflyClient::set_reputation(const std::string& peer_id, int reputation) {
    std::lock_guard<std::mutex> lock(client_mutex_);
    redis_->hset("peer:" + peer_id, "reputation", std::to_string(reputation));
    log_operation("set_reputation", {{"peer_id", peer_id}, {"reputation", reputation}});
}

int DragonflyClient::get_reputation(const std::string& peer_id) {
    std::lock_guard<std::mutex> lock(client_mutex_);
    auto val = redis_->hget("peer:" + peer_id, "reputation");
    int rep = 0;
    if (val) {
        try { rep = std::stoi(*val); } catch (...) {}
    }
    log_operation("get_reputation", {{"peer_id", peer_id}, {"reputation", rep}});
    return rep;
}

void DragonflyClient::set_cheat_flags(const std::string& peer_id, const nlohmann::json& flags) {
    std::lock_guard<std::mutex> lock(client_mutex_);
    redis_->hset("peer:" + peer_id, "cheat_flags", flags.dump());
    log_operation("set_cheat_flags", {{"peer_id", peer_id}, {"flags", flags}});
}

nlohmann::json DragonflyClient::get_cheat_flags(const std::string& peer_id) {
    std::lock_guard<std::mutex> lock(client_mutex_);
    auto val = redis_->hget("peer:" + peer_id, "cheat_flags");
    nlohmann::json flags;
    if (val) {
        try { flags = nlohmann::json::parse(*val); } catch (...) {}
    }
    log_operation("get_cheat_flags", {{"peer_id", peer_id}, {"flags", flags}});
    return flags;
}

std::vector<PeerGeoInfo> DragonflyClient::geosearch(double x, double y, double radius) {
    std::lock_guard<std::mutex> lock(client_mutex_);
    std::vector<PeerGeoInfo> result;
    auto peers = redis_->georadius(geo_key_, x, y, radius, sw::redis::GeoUnit::M);
    for (const auto& peer_id : peers) {
        PeerGeoInfo info;
        info.peer_id = peer_id;
        // Load metadata
        auto vals = redis_->hgetall("peer:" + peer_id);
        for (const auto& kv : vals) {
            if (kv.first == "endpoint") info.endpoint = kv.second;
            else if (kv.first == "reputation") info.reputation = std::stoi(kv.second);
            else if (kv.first == "risk_score") info.risk_score = std::stod(kv.second);
            else if (kv.first == "capabilities") {
                try { info.capabilities = nlohmann::json::parse(kv.second).get<std::vector<std::string>>(); } catch (...) {}
            }
        }
        info.x = x; info.y = y; // Optionally, fetch real coordinates
        result.push_back(info);
    }
    log_operation("geosearch", {{"x", x}, {"y", y}, {"radius", radius}, {"result_count", result.size()}});
    return result;
}

void DragonflyClient::log_operation(const std::string& op, const nlohmann::json& details) {
    nlohmann::json log = {
        {"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())},
        {"level", "INFO"},
        {"event", op},
        {"details", details}
    };
    std::cout << log.dump() << std::endl;
}