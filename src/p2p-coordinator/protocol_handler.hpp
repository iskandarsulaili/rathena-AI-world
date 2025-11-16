// protocol_handler.hpp
// Protocol handler for QUIC, gRPC, and NATS/JetStream traffic

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <cstdint>
#include "p2p_mesh_manager.hpp"

enum class ProtocolType {
    QUIC_DATAGRAM,
    QUIC_STREAM,
    GRPC,
    NATS
};

struct MovementDelta {
    std::string entity_id;
    uint64_t timestamp_ms;
    uint32_t sequence_num;
    int32_t delta_x, delta_y, delta_z;
    int16_t velocity_x, velocity_y, velocity_z;
    std::vector<uint8_t> orientation; // 8 bytes compressed
    std::vector<uint8_t> signature;   // 32 bytes ED25519
};

struct CombatAction {
    std::string player_id;
    std::string target_id;
    std::string skill_id;
    uint64_t timestamp_ms;
    uint32_t sequence_num;
    // ... other fields as needed
};

class ProtocolHandler {
public:
    ProtocolHandler(std::shared_ptr<P2PMeshManager> mesh_manager);
    ~ProtocolHandler();

    // QUIC datagram: movement deltas
    void handle_quic_datagram(const MovementDelta& delta);

    // QUIC stream: combat actions, state sync
    void handle_quic_stream(const CombatAction& action);

    // gRPC: authoritative state, cross-region
    void handle_grpc_message(const std::string& message);

    // NATS/JetStream: event propagation
    void handle_nats_event(const std::string& event);

    // Serialization/deserialization helpers
    static MovementDelta parse_movement_datagram(const std::vector<uint8_t>& data);
    static std::vector<uint8_t> serialize_movement_datagram(const MovementDelta& delta);

    // Logging and diagnostics
    void log_protocol_event(ProtocolType type, const std::string& event, const std::string& details);

private:
    std::shared_ptr<P2PMeshManager> mesh_manager_;
};