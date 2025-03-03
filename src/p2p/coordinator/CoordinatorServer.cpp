#include "CoordinatorServer.hpp"
#include <chrono>
#include <thread>
#include <sstream>
#include <iostream>

namespace rathena {
namespace p2p {

CoordinatorServer::CoordinatorServer(const Config& config)
    : config_(config)
    , running_(false) {
    
    // Initialize components with default configurations
    HostManager::Config host_config{
        .min_score_threshold = 75,
        .check_interval = 5000,
        .grace_period = 30000,
        .max_players_per_host = 100,
        .enable_auto_failover = true
    };
    host_manager_ = std::make_unique<HostManager>(host_config);

    MapDistributor::Config map_config{
        .max_maps_per_host = 5,
        .rebalance_interval = 300,
        .load_threshold = 0.8f,
        .enable_dynamic_assignment = true
    };
    map_distributor_ = std::make_unique<MapDistributor>(map_config, host_manager_.get());

    DatabaseProxy::Config db_config{
        .port = config.db_proxy_port,
        .max_connections = 100,
        .connection_timeout = 30000,
        .retry_interval = 5000,
        .batch_size = 1000,
        .enable_caching = true
    };
    db_proxy_ = std::make_unique<DatabaseProxy>(db_config);
}

CoordinatorServer::~CoordinatorServer() {
    if (running_) {
        stop();
    }
}

bool CoordinatorServer::initialize() {
    try {
        // Initialize network manager
        if (!network_manager_.start_server(config_.port)) {
            std::cerr << "Failed to start network server on port " << config_.port << std::endl;
            return false;
        }

        // Register packet handlers
        network_manager_.register_handler(
            PacketType::HOST_REGISTER,
            std::bind(&CoordinatorServer::handle_host_registration, this, std::placeholders::_1)
        );
        
        network_manager_.register_handler(
            PacketType::HOST_METRICS,
            std::bind(&CoordinatorServer::handle_host_metrics, this, std::placeholders::_1)
        );
        
        network_manager_.register_handler(
            PacketType::MAP_REQUEST,
            std::bind(&CoordinatorServer::handle_map_request, this, std::placeholders::_1)
        );
        
        network_manager_.register_handler(
            PacketType::DB_SYNC,
            std::bind(&CoordinatorServer::handle_db_sync, this, std::placeholders::_1)
        );

        // Initialize database proxy
        if (!db_proxy_->initialize()) {
            std::cerr << "Failed to initialize database proxy" << std::endl;
            return false;
        }

        return true;
    } catch (const std::exception& e) {
        std::cerr << "Initialization error: " << e.what() << std::endl;
        return false;
    }
}

void CoordinatorServer::start() {
    if (running_) {
        return;
    }

    running_ = true;
    
    // Start components
    db_proxy_->start();
    
    // Start health check thread
    health_check_thread_ = std::thread([this]() {
        while (running_) {
            check_host_health();
            std::this_thread::sleep_for(std::chrono::milliseconds(config_.check_interval));
        }
    });
    
    // Start metrics update thread
    metrics_thread_ = std::thread([this]() {
        while (running_) {
            update_metrics();
            log_metrics();
            std::this_thread::sleep_for(std::chrono::seconds(60));
        }
    });
}

void CoordinatorServer::stop() {
    if (!running_) {
        return;
    }

    running_ = false;
    
    // Stop health check thread
    if (health_check_thread_.joinable()) {
        health_check_thread_.join();
    }
    
    // Stop metrics thread
    if (metrics_thread_.joinable()) {
        metrics_thread_.join();
    }
    
    // Stop components
    db_proxy_->stop();
}

void CoordinatorServer::handle_host_registration(const NetworkPacket& packet) {
    try {
        std::string address = packet.read_string();
        uint16_t port = packet.read_uint16();
        bool is_vps = packet.read_uint8() != 0;
        
        uint32_t host_id = host_manager_->register_host(address, port, is_vps);
        if (host_id == 0) {
            send_error_response(packet, "Host registration failed");
            return;
        }
        
        // Generate and send authentication token
        std::string token = generate_host_token();
        NetworkPacket response(PacketType::HOST_REGISTER);
        response.write_uint32(host_id);
        response.write_string(token);
        network_manager_.send_packet(packet.get_session(), response);
        
        std::cout << "Registered new host: " << address << ":" << port 
                  << " (ID: " << host_id << ")" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Host registration error: " << e.what() << std::endl;
        send_error_response(packet, e.what());
    }
}

void CoordinatorServer::handle_host_metrics(const NetworkPacket& packet) {
    try {
        uint32_t host_id = packet.read_uint32();
        
        HostMetrics metrics;
        metrics.cpu_usage = packet.read_float();
        metrics.memory_usage = packet.read_float();
        metrics.network_latency = packet.read_float();
        metrics.player_count = packet.read_uint32();
        metrics.error_count = packet.read_uint32();
        
        if (!host_manager_->update_host_metrics(host_id, metrics)) {
            send_error_response(packet, "Failed to update host metrics");
            return;
        }
        
        // Check if rebalancing is needed
        if (metrics.cpu_usage > 80.0f || metrics.memory_usage > 85.0f) {
            map_distributor_->rebalance_maps();
        }
    } catch (const std::exception& e) {
        std::cerr << "Host metrics error: " << e.what() << std::endl;
        send_error_response(packet, e.what());
    }
}

void CoordinatorServer::handle_map_request(const NetworkPacket& packet) {
    try {
        uint32_t map_id = packet.read_uint32();
        uint32_t requesting_host = packet.read_uint32();
        
        // Find best host for the map
        uint32_t selected_host = map_distributor_->get_map_host(map_id);
        if (selected_host == 0) {
            selected_host = map_distributor_->find_best_host(map_id);
            if (selected_host == 0) {
                send_error_response(packet, "No suitable host found for map");
                return;
            }
        }
        
        // Send response with host information
        NetworkPacket response(PacketType::MAP_ASSIGN);
        response.write_uint32(map_id);
        response.write_uint32(selected_host);
        
        HostInfo* host_info = host_manager_->get_host_info(selected_host);
        if (host_info) {
            response.write_string(host_info->address);
            response.write_uint16(host_info->port);
        }
        
        network_manager_.send_packet(packet.get_session(), response);
    } catch (const std::exception& e) {
        std::cerr << "Map request error: " << e.what() << std::endl;
        send_error_response(packet, e.what());
    }
}

void CoordinatorServer::handle_db_sync(const NetworkPacket& packet) {
    try {
        uint32_t host_id = packet.read_uint32();
        if (!verify_host_token(packet.read_string())) {
            send_error_response(packet, "Invalid host token");
            return;
        }
        
        DatabaseProxy::QueryRequest request;
        request.host_id = host_id;
        request.type = static_cast<DatabaseProxy::QueryType>(packet.read_uint8());
        request.query = packet.read_string();
        
        uint32_t param_count = packet.read_uint16();
        for (uint32_t i = 0; i < param_count; ++i) {
            request.parameters.push_back(packet.read_string());
        }
        
        uint32_t request_id = db_proxy_->submit_query(request);
        
        NetworkPacket response(PacketType::DB_RESPONSE);
        response.write_uint32(request_id);
        network_manager_.send_packet(packet.get_session(), response);
    } catch (const std::exception& e) {
        std::cerr << "Database sync error: " << e.what() << std::endl;
        send_error_response(packet, e.what());
    }
}

void CoordinatorServer::check_host_health() {
    std::lock_guard<std::mutex> lock(state_mutex_);
    
    for (auto& [host_id, host] : hosts_) {
        auto now = std::chrono::system_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - host.last_check).count();
            
        if (elapsed > config_.grace_period) {
            handle_host_failure(host_id);
        }
    }
}

void CoordinatorServer::handle_host_failure(uint32_t host_id) {
    std::cout << "Handling failure of host " << host_id << std::endl;
    
    // Get maps that need to be reassigned
    auto hosted_maps = map_distributor_->get_host_maps(host_id);
    
    // Unregister the failed host
    host_manager_->unregister_host(host_id);
    
    // Reassign each map
    for (auto map_id : hosted_maps) {
        if (!map_distributor_->reassign_map(map_id)) {
            std::cerr << "Failed to reassign map " << map_id << std::endl;
        }
    }
    
    metrics_.failover_count++;
}

void CoordinatorServer::update_metrics() {
    std::lock_guard<std::mutex> lock(state_mutex_);
    
    metrics_.active_hosts = 0;
    metrics_.total_players = 0;
    
    for (const auto& [host_id, host] : hosts_) {
        if (host_manager_->is_host_healthy(host_id)) {
            metrics_.active_hosts++;
            metrics_.total_players += host.current_players;
        }
    }
}

void CoordinatorServer::log_metrics() {
    std::ostringstream oss;
    oss << "P2P Network Metrics:\n"
        << "Active Hosts: " << metrics_.active_hosts << "\n"
        << "Total Players: " << metrics_.total_players << "\n"
        << "Failover Count: " << metrics_.failover_count << "\n"
        << "Sync Errors: " << metrics_.sync_errors;
    
    std::cout << oss.str() << std::endl;
}

bool CoordinatorServer::verify_host_token(const std::string& token) {
    // TODO: Implement proper token verification
    return true;
}

std::string CoordinatorServer::generate_host_token() {
    // TODO: Implement secure token generation
    return "temporary_token";
}

void CoordinatorServer::send_error_response(const NetworkPacket& original_packet, 
                                          const std::string& error_message) {
    NetworkPacket response(PacketType::ERROR);
    response.write_uint32(original_packet.get_sequence());
    response.write_string(error_message);
    network_manager_.send_packet(original_packet.get_session(), response);
}

} // namespace p2p
} // namespace rathena