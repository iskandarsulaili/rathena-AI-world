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
    // Structured log for movement delta received
    log_protocol_event(ProtocolType::QUIC_DATAGRAM, "movement_delta_received", delta.entity_id);

    // Validate movement delta (signature, sequence, bounds)
    bool valid = true;
    std::string validation_error;
    if (delta.signature.empty()) {
        valid = false;
        validation_error = "Missing signature";
    }
    // Example: Add more validation (ED25519, sequence, bounds, etc.)
    extern SecurityManager g_security_manager; // Global instance, or inject as needed
    if (!g_security_manager.validate_signature(delta.entity_id, /*serialize delta for signature*/ std::vector<uint8_t>(reinterpret_cast<const uint8_t*>(&delta), reinterpret_cast<const uint8_t*>(&delta) + sizeof(MovementDelta)), delta.signature)) {
        valid = false;
        validation_error = "Invalid signature";
    }

    if (!valid) {
        log_protocol_event(ProtocolType::QUIC_DATAGRAM, "movement_delta_validation_failed", validation_error);
        return;
    }

    // Authoritative state update and speculative prediction/reconciliation
    bool reconciled = false;
    std::string reconcile_reason;
    if (mesh_manager_) {
        auto worker = mesh_manager_->get_simulation_worker_for_entity(delta.entity_id);
        if (worker) {
            auto entity = worker->get_entity(delta.entity_id);
            if (entity) {
                // Speculative prediction: compare predicted vs. authoritative
                int32_t predicted_x = entity->x + delta.delta_x;
                int32_t predicted_y = entity->y + delta.delta_y;
                int32_t predicted_z = entity->z + delta.delta_z;
                // Example: If position delta is too large, trigger reconciliation
                int32_t dx = std::abs(predicted_x - entity->x);
                int32_t dy = std::abs(predicted_y - entity->y);
                int32_t dz = std::abs(predicted_z - entity->z);
                if (dx > 100 || dy > 100 || dz > 100) {
                    // Position mismatch, trigger correction/rollback
                    reconciled = true;
                    reconcile_reason = "Position delta too large, correction applied";
                    // Rollback to authoritative state (could be more sophisticated)
                    predicted_x = entity->x;
                    predicted_y = entity->y;
                    predicted_z = entity->z;
                    // Propagate correction to client/peers
                    nlohmann::json correction_log = {
                        {"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())},
                        {"level", "WARN"},
                        {"protocol", static_cast<int>(ProtocolType::QUIC_DATAGRAM)},
                        {"event", "correction_propagated"},
                        {"entity_id", delta.entity_id},
                        {"authoritative_x", entity->x},
                        {"authoritative_y", entity->y},
                        {"authoritative_z", entity->z},
                        {"client_predicted_x", predicted_x},
                        {"client_predicted_y", predicted_y},
                        {"client_predicted_z", predicted_z},
                        {"reason", reconcile_reason}
                    };
                    std::cout << correction_log.dump() << std::endl;
                } else {
                    // Accept movement
                    entity->x = predicted_x;
                    entity->y = predicted_y;
                    entity->z = predicted_z;
                }
                // TODO: update velocity/orientation
                log_protocol_event(ProtocolType::QUIC_DATAGRAM, "entity_state_updated", delta.entity_id);
                if (reconciled) {
                    log_protocol_event(ProtocolType::QUIC_DATAGRAM, "reconciliation", reconcile_reason);
                }
                // AOI propagation: forward to nearby peers
                auto peers = mesh_manager_->get_peers_in_zone(entity->x, entity->y, entity->z, AOIZone::CRITICAL);
                for (const auto& peer : peers) {
                    if (peer.peer_id != delta.entity_id) {
                        // TODO: Send movement delta or correction to peer.quic_endpoint (QUIC datagram send)
                        log_protocol_event(ProtocolType::QUIC_DATAGRAM, reconciled ? "correction_forwarded" : "movement_delta_forwarded", peer.peer_id);
                    }
                }
            } else {
                log_protocol_event(ProtocolType::QUIC_DATAGRAM, "entity_not_found", delta.entity_id);
            }
        } else {
            log_protocol_event(ProtocolType::QUIC_DATAGRAM, "simulation_worker_not_found", delta.entity_id);
        }
    }
}

void ProtocolHandler::handle_quic_stream(const CombatAction& action) {
    // Structured log for combat action received
    log_protocol_event(ProtocolType::QUIC_STREAM, "combat_action_received", action.player_id + "->" + action.target_id);

    // Validate combat action (timestamp, skill, etc.)
    bool valid = true;
    std::string validation_error;
    // Authority validation: signature, sequence, etc.
    extern SecurityManager g_security_manager;
    // For demonstration, assume CombatAction has a signature field (add if missing)
    // if (!g_security_manager.validate_signature(action.player_id, /*serialize action for signature*/ std::vector<uint8_t>(reinterpret_cast<const uint8_t*>(&action), reinterpret_cast<const uint8_t*>(&action) + sizeof(CombatAction)), action.signature)) {
    //     valid = false;
    //     validation_error = "Invalid signature";
    // }
    if (action.skill_id.empty()) {
        valid = false;
        validation_error = "Missing skill_id";
    }
    // Example: Add more validation (timestamp, skill existence, etc.)

    if (!valid) {
        log_protocol_event(ProtocolType::QUIC_STREAM, "combat_action_validation_failed", validation_error);
        return;
    }

    // Authoritative validation and reconciliation
    bool impossible_action = false;
    std::string impossible_reason;
    if (mesh_manager_) {
        auto worker = mesh_manager_->get_simulation_worker_for_entity(action.player_id);
        if (worker) {
            auto entity = worker->get_entity(action.player_id);
            if (entity) {
                // Example: Check if action is possible (e.g., skill cooldown, range)
                // (Production: integrate with combat system)
                if (false) { // Replace with real checks
                    impossible_action = true;
                    impossible_reason = "Impossible action: skill on cooldown or out of range";
                    // Propagate rollback/correction to client/peers
                    log_protocol_event(ProtocolType::QUIC_STREAM, "rollback_propagated", action.player_id);
                } else {
                    // Apply combat action to entity state
                    // TODO: Implement actual combat logic
                    log_protocol_event(ProtocolType::QUIC_STREAM, "combat_action_applied", action.player_id);
                }
                if (impossible_action) {
                    nlohmann::json rollback_log = {
                        {"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())},
                        {"level", "WARN"},
                        {"protocol", static_cast<int>(ProtocolType::QUIC_STREAM)},
                        {"event", "rollback_propagated"},
                        {"player_id", action.player_id},
                        {"target_id", action.target_id},
                        {"skill_id", action.skill_id},
                        {"reason", impossible_reason}
                    };
                    std::cout << rollback_log.dump() << std::endl;
                    log_protocol_event(ProtocolType::QUIC_STREAM, "combat_action_impossible", impossible_reason);
                    // Rollback or reject action
                }
                // AOI propagation: forward to nearby peers
                auto peers = mesh_manager_->get_peers_in_zone(entity->x, entity->y, entity->z, AOIZone::CRITICAL);
                for (const auto& peer : peers) {
                    if (peer.peer_id != action.player_id) {
                        // TODO: Send combat action or rollback to peer.quic_endpoint (QUIC stream send)
                        log_protocol_event(ProtocolType::QUIC_STREAM, impossible_action ? "rollback_forwarded" : "combat_action_forwarded", peer.peer_id);
                    }
                }
            } else {
                log_protocol_event(ProtocolType::QUIC_STREAM, "entity_not_found", action.player_id);
            }
        } else {
            log_protocol_event(ProtocolType::QUIC_STREAM, "simulation_worker_not_found", action.player_id);
        }
    }
}

void ProtocolHandler::handle_grpc_message(const std::string& message) {
    log_protocol_event(ProtocolType::GRPC, "grpc_message_received", message);

    // Example: Parse gRPC message (assume JSON for demonstration)
    nlohmann::json grpc_json;
    try {
        grpc_json = nlohmann::json::parse(message);
    } catch (const std::exception& ex) {
        log_protocol_event(ProtocolType::GRPC, "grpc_message_parse_error", ex.what());
        return;
    }

    // Route by message type
    std::string type = grpc_json.value("type", "");
    if (type == "state_sync") {
        // Authoritative state sync: update entity state
        std::string entity_id = grpc_json.value("entity_id", "");
        int32_t x = grpc_json.value("x", 0);
        int32_t y = grpc_json.value("y", 0);
        int32_t z = grpc_json.value("z", 0);
        if (mesh_manager_) {
            auto worker = mesh_manager_->get_simulation_worker_for_entity(entity_id);
            if (worker) {
                auto entity = worker->get_entity(entity_id);
                if (entity) {
                    entity->x = x;
                    entity->y = y;
                    entity->z = z;
                    log_protocol_event(ProtocolType::GRPC, "state_sync_applied", entity_id);
                } else {
                    log_protocol_event(ProtocolType::GRPC, "entity_not_found", entity_id);
                }
            } else {
                log_protocol_event(ProtocolType::GRPC, "simulation_worker_not_found", entity_id);
            }
        }
    } else if (type == "region_handoff") {
        // Cross-region handoff: migrate entity to new region/worker
        std::string entity_id = grpc_json.value("entity_id", "");
        std::string target_region = grpc_json.value("target_region", "");
        if (mesh_manager_) {
            // Example: Remove from current worker, add to target region (stub)
            log_protocol_event(ProtocolType::GRPC, "region_handoff_initiated", entity_id + "->" + target_region);
            // TODO: Implement actual migration logic
        }
    } else if (type == "db_transaction") {
        // Database transaction: log and acknowledge
        std::string txn_id = grpc_json.value("txn_id", "");
        std::string op = grpc_json.value("operation", "");
        log_protocol_event(ProtocolType::GRPC, "db_transaction", txn_id + ":" + op);
        // TODO: Integrate with DB layer
    } else {
        log_protocol_event(ProtocolType::GRPC, "grpc_message_unknown_type", type);
    }
}

void ProtocolHandler::handle_nats_event(const std::string& event) {
    log_protocol_event(ProtocolType::NATS, "nats_event_received", event);

    // Example: Parse NATS/JetStream event (assume JSON for demonstration)
    nlohmann::json nats_json;
    try {
        nats_json = nlohmann::json::parse(event);
    } catch (const std::exception& ex) {
        log_protocol_event(ProtocolType::NATS, "nats_event_parse_error", ex.what());
        return;
    }

    std::string type = nats_json.value("type", "");
    if (type == "cross_region_state") {
        // Cross-region state propagation
        std::string entity_id = nats_json.value("entity_id", "");
        int32_t x = nats_json.value("x", 0);
        int32_t y = nats_json.value("y", 0);
        int32_t z = nats_json.value("z", 0);
        if (mesh_manager_) {
            auto worker = mesh_manager_->get_simulation_worker_for_entity(entity_id);
            if (worker) {
                auto entity = worker->get_entity(entity_id);
                if (entity) {
                    entity->x = x;
                    entity->y = y;
                    entity->z = z;
                    log_protocol_event(ProtocolType::NATS, "cross_region_state_applied", entity_id);
                } else {
                    log_protocol_event(ProtocolType::NATS, "entity_not_found", entity_id);
                }
            } else {
                log_protocol_event(ProtocolType::NATS, "simulation_worker_not_found", entity_id);
            }
        }
    } else if (type == "boss_event" || type == "npc_event") {
        // Boss/NPC event propagation
        std::string event_id = nats_json.value("event_id", "");
        std::string description = nats_json.value("description", "");
        log_protocol_event(ProtocolType::NATS, "boss_npc_event", event_id + ":" + description);
        // TODO: Propagate to relevant peers/regions
        if (mesh_manager_) {
            // Example: Forward to all peers in EXTENDED AOI
            auto peers = mesh_manager_->get_peers_in_zone(0, 0, 0, AOIZone::EXTENDED); // Replace with actual event location
            for (const auto& peer : peers) {
                log_protocol_event(ProtocolType::NATS, "boss_npc_event_forwarded", peer.peer_id);
                // TODO: Send event to peer.quic_endpoint (NATS publish)
            }
        }
    } else if (type == "ownership_migration") {
        // Ownership migration event
        std::string entity_id = nats_json.value("entity_id", "");
        std::string new_owner = nats_json.value("new_owner", "");
        log_protocol_event(ProtocolType::NATS, "ownership_migration", entity_id + "->" + new_owner);
        // TODO: Update ownership in mesh manager
        if (mesh_manager_) {
            // Example: Update peer info (stub)
            // mesh_manager_->update_peer_owner(entity_id, new_owner);
        }
    } else {
        log_protocol_event(ProtocolType::NATS, "nats_event_unknown_type", type);
    }
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