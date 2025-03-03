#include "P2PMapServer.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <chrono>

namespace rathena {
namespace p2p {

P2PMapServer::P2PMapServer(const Config& config)
    : config_(config)
    , running_(false)
    , update_count_(0)
    , packet_count_(0)
    , network_manager_(std::make_unique<NetworkManager>()) {
    
    metrics_ = {
        .cpu_usage = 0.0f,
        .memory_usage = 0.0f,
        .network_latency = 0,
        .total_players = 0,
        .packets_per_second = 0,
        .updates_per_second = 0
    };
}

P2PMapServer::~P2PMapServer() {
    if (running_) {
        stop();
    }
}

bool P2PMapServer::initialize() {
    try {
        // Initialize network manager
        if (!network_manager_->start_server(config_.port)) {
            std::cerr << "Failed to start network server on port " << config_.port << std::endl;
            return false;
        }

        // Register packet handlers
        network_manager_->register_handler(
            PacketType::PLAYER_CONNECT,
            [this](const NetworkPacket& packet) { handle_network_packet(packet); }
        );

        network_manager_->register_handler(
            PacketType::PLAYER_MOVE,
            [this](const NetworkPacket& packet) { handle_network_packet(packet); }
        );

        network_manager_->register_handler(
            PacketType::PLAYER_ACTION,
            [this](const NetworkPacket& packet) { handle_network_packet(packet); }
        );

        last_sync_ = std::chrono::system_clock::now();
        last_metrics_update_ = last_sync_;

        return true;
    } catch (const std::exception& e) {
        std::cerr << "Initialization error: " << e.what() << std::endl;
        return false;
    }
}

void P2PMapServer::start() {
    if (running_) {
        return;
    }

    running_ = true;

    // Start update thread
    update_thread_ = std::thread([this]() {
        while (running_) {
            update_loop();
            std::this_thread::sleep_for(std::chrono::milliseconds(config_.update_interval));
        }
    });
}

void P2PMapServer::stop() {
    if (!running_) {
        return;
    }

    running_ = false;

    if (update_thread_.joinable()) {
        update_thread_.join();
    }
}

bool P2PMapServer::load_map(map_id_t map_id) {
    std::lock_guard<std::mutex> lock(state_mutex_);
    
    try {
        MapInstance::Config map_config{
            .width = 0,              // Will be set from map data
            .height = 0,             // Will be set from map data
            .pvp_enabled = false,    // Default value
            .mob_spawns_enabled = config_.enable_mob_ai,
            .max_mobs = 100,         // Default value
            .max_items = 1000,       // Default value
            .mob_db_path = config_.map_cache_path + "/mob_db.txt",
            .item_db_path = config_.map_cache_path + "/item_db.txt"
        };

        auto& map_state = maps_[map_id];
        map_state.id = map_id;
        map_state.instance = std::make_unique<MapInstance>(map_config);
        map_state.player_count = 0;
        map_state.is_active = true;
        map_state.last_update = std::chrono::system_clock::now();

        if (!map_state.instance->initialize(map_id)) {
            maps_.erase(map_id);
            return false;
        }

        std::cout << "Loaded map " << map_id << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading map " << map_id << ": " << e.what() << std::endl;
        return false;
    }
}

bool P2PMapServer::unload_map(map_id_t map_id) {
    std::lock_guard<std::mutex> lock(state_mutex_);
    
    auto it = maps_.find(map_id);
    if (it == maps_.end()) {
        return false;
    }

    // Remove all players from the map
    std::vector<player_id_t> players_to_remove;
    for (const auto& [player_id, state] : players_) {
        if (state.current_map == map_id) {
            players_to_remove.push_back(player_id);
        }
    }

    for (auto player_id : players_to_remove) {
        remove_player(player_id);
    }

    maps_.erase(it);
    std::cout << "Unloaded map " << map_id << std::endl;
    return true;
}

bool P2PMapServer::add_player(player_id_t player_id, const PlayerInfo& info) {
    std::lock_guard<std::mutex> lock(player_mutex_);
    
    // Check if map exists and is active
    auto map_it = maps_.find(info.current_map);
    if (map_it == maps_.end() || !map_it->second.is_active) {
        return false;
    }

    // Add player to map instance
    MapInstance::Position pos{info.position.x, info.position.y, 0};
    if (!map_it->second.instance->add_player(player_id, pos)) {
        return false;
    }

    // Update player state
    PlayerState& state = players_[player_id];
    state.id = player_id;
    state.info = info;
    state.current_map = info.current_map;
    state.x = info.position.x;
    state.y = info.position.y;
    state.last_update = std::chrono::system_clock::now();

    // Update map state
    map_it->second.player_count++;
    metrics_.total_players++;

    std::cout << "Added player " << player_id << " to map " << info.current_map << std::endl;
    return true;
}

void P2PMapServer::update_loop() {
    auto now = std::chrono::system_clock::now();
    
    // Update game state
    update_maps();
    update_players();
    process_ai();
    handle_collisions();

    // Sync with coordinator if needed
    auto since_last_sync = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - last_sync_).count();
    if (since_last_sync >= config_.sync_interval) {
        sync_with_coordinator();
        last_sync_ = now;
    }

    // Update metrics
    update_count_++;
    auto since_last_metrics = std::chrono::duration_cast<std::chrono::seconds>(
        now - last_metrics_update_).count();
    if (since_last_metrics >= 1) {
        update_metrics();
        log_metrics();
        last_metrics_update_ = now;
        update_count_ = 0;
        packet_count_ = 0;
    }
}

void P2PMapServer::update_maps() {
    std::lock_guard<std::mutex> lock(state_mutex_);
    
    for (auto& [map_id, state] : maps_) {
        if (!state.is_active) continue;

        try {
            state.instance->update(config_.update_interval);
            state.last_update = std::chrono::system_clock::now();
        } catch (const std::exception& e) {
            std::cerr << "Error updating map " << map_id << ": " << e.what() << std::endl;
            state.is_active = false;
        }
    }
}

void P2PMapServer::update_players() {
    std::lock_guard<std::mutex> lock(player_mutex_);
    
    auto now = std::chrono::system_clock::now();
    std::vector<player_id_t> inactive_players;

    for (auto& [player_id, state] : players_) {
        auto idle_time = std::chrono::duration_cast<std::chrono::seconds>(
            now - state.last_update).count();
            
        if (idle_time > 300) {  // 5 minutes timeout
            inactive_players.push_back(player_id);
            continue;
        }

        auto map_it = maps_.find(state.current_map);
        if (map_it != maps_.end() && map_it->second.is_active) {
            map_it->second.instance->update_player(player_id);
        }
    }

    // Remove inactive players
    for (auto player_id : inactive_players) {
        remove_player(player_id);
    }
}

void P2PMapServer::handle_network_packet(const NetworkPacket& packet) {
    packet_count_++;

    try {
        switch (packet.get_type()) {
            case PacketType::PLAYER_CONNECT: {
                player_id_t player_id = packet.read_uint32();
                PlayerInfo info;
                info.name = packet.read_string();
                info.current_map = packet.read_uint32();
                info.position.x = packet.read_float();
                info.position.y = packet.read_float();
                
                add_player(player_id, info);
                break;
            }
            case PacketType::PLAYER_MOVE: {
                player_id_t player_id = packet.read_uint32();
                float x = packet.read_float();
                float y = packet.read_float();
                
                if (validate_player_position(player_id, x, y)) {
                    update_player_position(player_id, x, y);
                }
                break;
            }
            case PacketType::PLAYER_ACTION: {
                player_id_t player_id = packet.read_uint32();
                
                if (validate_player_action(player_id, packet)) {
                    handle_player_action(player_id, packet);
                }
                break;
            }
            default:
                std::cerr << "Unknown packet type: " << static_cast<int>(packet.get_type()) << std::endl;
                break;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error handling packet: " << e.what() << std::endl;
    }
}

void P2PMapServer::update_metrics() {
    metrics_.updates_per_second = update_count_;
    metrics_.packets_per_second = packet_count_;
    
    // TODO: Implement CPU and memory usage monitoring
    metrics_.cpu_usage = 0.0f;
    metrics_.memory_usage = 0.0f;
    
    // Calculate average network latency
    metrics_.network_latency = network_manager_->get_average_latency();
}

void P2PMapServer::log_metrics() {
    std::ostringstream oss;
    oss << "P2P Map Server Metrics:\n"
        << "Total Players: " << metrics_.total_players << "\n"
        << "Updates/sec: " << metrics_.updates_per_second << "\n"
        << "Packets/sec: " << metrics_.packets_per_second << "\n"
        << "Network Latency: " << metrics_.network_latency << "ms\n"
        << "CPU Usage: " << metrics_.cpu_usage << "%\n"
        << "Memory Usage: " << metrics_.memory_usage << "MB";
    
    std::cout << oss.str() << std::endl;
}

} // namespace p2p
} // namespace rathena