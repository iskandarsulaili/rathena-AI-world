// protocol_handler.cpp
// Implementation of ProtocolHandler for QUIC, gRPC, and NATS/JetStream traffic

#include "protocol_handler.hpp"
#include <nlohmann/json.hpp>
#include <iostream>
#include <cstring>

ProtocolHandler::ProtocolHandler(std::shared_ptr<P2PMeshManager> mesh_manager)
    : mesh_manager_(mesh_manager) {}

ProtocolHandler::~ProtocolHandler() {}

void ProtocolHandler::handle_quic_datagram(const MovementDelta& delta) {
    // Example: log and route movement delta to mesh peers
    log_protocol_event(ProtocolType::QUIC_DATAGRAM, "movement_delta_received", delta.entity_id);
    // TODO: Validate, update entity state, forward to AOI peers
}

void ProtocolHandler::handle_quic_stream(const CombatAction& action) {
    // Example: log and process combat action
    log_protocol_event(ProtocolType::QUIC_STREAM, "combat_action_received", action.player_id + "->" + action.target_id);
    // TODO: Validate, forward to authoritative worker, reconcile
}

void ProtocolHandler::handle_grpc_message(const std::string& message) {
    log_protocol_event(ProtocolType::GRPC, "grpc_message_received", message);
    // TODO: Parse and process authoritative state/cross-region sync
}

void ProtocolHandler::handle_nats_event(const std::string& event) {
    log_protocol_event(ProtocolType::NATS, "nats_event_received", event);
    // TODO: Parse and propagate event to mesh/worker pool
}

MovementDelta ProtocolHandler::parse_movement_datagram(const std::vector<uint8_t>& data) {
    // Example: very basic parsing (should match protobuf spec in production)
    MovementDelta delta;
    if (data.size() < 32) return delta;
    // TODO: Proper deserialization (protobuf, etc)
    std::memcpy(&delta.delta_x, data.data(), sizeof(int32_t));
    std::memcpy(&delta.delta_y, data.data() + 4, sizeof(int32_t));
    std::memcpy(&delta.delta_z, data.data() + 8, sizeof(int32_t));
    // ... fill other fields
    return delta;
}

std::vector<uint8_t> ProtocolHandler::serialize_movement_datagram(const MovementDelta& delta) {
    // Example: very basic serialization (should match protobuf spec in production)
    std::vector<uint8_t> data(12);
    std::memcpy(data.data(), &delta.delta_x, sizeof(int32_t));
    std::memcpy(data.data() + 4, &delta.delta_y, sizeof(int32_t));
    std::memcpy(data.data() + 8, &delta.delta_z, sizeof(int32_t));
    // ... fill other fields
    return data;
}

void ProtocolHandler::log_protocol_event(ProtocolType type, const std::string& event, const std::string& details) {
    nlohmann::json log = {
        {"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())},
        {"level", "INFO"},
        {"protocol", static_cast<int>(type)},
        {"event", event},
        {"details", details}
    };
    std::cout << log.dump() << std::endl;
}