// protocol_server.cpp
// Implementation of ProtocolServer for unified protocol event handling

#include "protocol_server.hpp"
#include <iostream>
#include <chrono>

ProtocolServer::ProtocolServer(std::shared_ptr<ProtocolHandler> handler, std::shared_ptr<MetricsExporter> metrics)
    : handler_(handler), metrics_(metrics), running_(false) {}

ProtocolServer::~ProtocolServer() {
    stop();
}

void ProtocolServer::start() {
    running_ = true;
    threads_.emplace_back(&ProtocolServer::quic_receive_loop, this);
    threads_.emplace_back(&ProtocolServer::grpc_receive_loop, this);
    threads_.emplace_back(&ProtocolServer::nats_receive_loop, this);
    nlohmann::json log = {
        {"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())},
        {"level", "INFO"},
        {"event", "protocol_server_started"}
    };
    std::cout << log.dump() << std::endl;
}

void ProtocolServer::stop() {
    running_ = false;
    for (auto& t : threads_) {
        if (t.joinable()) t.join();
    }
    threads_.clear();
    nlohmann::json log = {
        {"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())},
        {"level", "INFO"},
        {"event", "protocol_server_stopped"}
    };
    std::cout << log.dump() << std::endl;
}

void ProtocolServer::dispatch_event(ProtocolType type, const std::string& payload) {
    // Thread-safe dispatch to protocol handler
    switch (type) {
        case ProtocolType::QUIC_DATAGRAM:
            handler_->handle_quic_datagram(ProtocolHandler::parse_movement_datagram(std::vector<uint8_t>(payload.begin(), payload.end())));
            break;
        case ProtocolType::QUIC_STREAM:
            // Assume payload is a serialized CombatAction (stub)
            // handler_->handle_quic_stream(...);
            break;
        case ProtocolType::GRPC:
            handler_->handle_grpc_message(payload);
            break;
        case ProtocolType::NATS:
            handler_->handle_nats_event(payload);
            break;
    }
}

void ProtocolServer::quic_receive_loop() {
    // Stub: Replace with msquic event loop
    while (running_) {
        // Simulate receiving a QUIC datagram (movement delta)
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        // Example: dispatch_event(ProtocolType::QUIC_DATAGRAM, ...);
    }
}

void ProtocolServer::grpc_receive_loop() {
    // Stub: Replace with grpc-cpp event loop
    while (running_) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        // Example: dispatch_event(ProtocolType::GRPC, ...);
    }
}

void ProtocolServer::nats_receive_loop() {
    // Stub: Replace with nats-cpp event loop
    while (running_) {
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        // Example: dispatch_event(ProtocolType::NATS, ...);
    }
}