/**
 * aiworld_plugin.cpp
 * rAthena AIWorld Plugin/Module (ZeroMQ IPC)
 * Implements plugin lifecycle, ZeroMQ IPC client, and integration with rAthena event loop and script engine.
 */

#include "aiworld_plugin.hpp"
#include "aiworld_utils.hpp"
#include <nlohmann/json.hpp>
#include <iostream>

// --- Security, Anti-Cheat, and Authority Enforcement Implementation (Enterprise-Ready) ---
#include <sodium.h>

SecurityManager::SecurityManager(const SecurityConfig& config)
   : config_(config) {
   if (sodium_init() < 0) {
       throw std::runtime_error("libsodium initialization failed");
   }
}

SecurityManager::~SecurityManager() {}

bool SecurityManager::validate_signature(const std::string& peer_id, const std::string& payload, const std::string& signature) {
   if (!config_.enable_signature_validation) return true;
   auto it = peer_pubkeys_.find(peer_id);
   if (it == peer_pubkeys_.end()) {
       aiworld::log_warn("No public key registered for peer: " + peer_id);
       return false;
   }
   const std::string& pubkey_hex = it->second;
   if (pubkey_hex.size() != crypto_sign_PUBLICKEYBYTES * 2) {
       aiworld::log_error("Invalid public key length for peer: " + peer_id);
       return false;
   }
   unsigned char pubkey[crypto_sign_PUBLICKEYBYTES];
   sodium_hex2bin(pubkey, sizeof(pubkey), pubkey_hex.c_str(), pubkey_hex.size(), nullptr, nullptr, nullptr);

   if (signature.size() != crypto_sign_BYTES * 2) {
       aiworld::log_error("Invalid signature length for peer: " + peer_id);
       return false;
   }
   unsigned char sig[crypto_sign_BYTES];
   sodium_hex2bin(sig, sizeof(sig), signature.c_str(), signature.size(), nullptr, nullptr, nullptr);

   // Verify signature
   int result = crypto_sign_verify_detached(sig,
                                            reinterpret_cast<const unsigned char*>(payload.data()),
                                            payload.size(),
                                            pubkey);
   if (result == 0) {
       return true;
   } else {
       aiworld::log_warn("Signature verification failed for peer: " + peer_id);
       return false;
   }
}

void SecurityManager::register_peer_pubkey(const std::string& peer_id, const std::string& pubkey_hex) {
   peer_pubkeys_[peer_id] = pubkey_hex;
}

void SecurityManager::blacklist_peer(const std::string& peer_id) {
    if (!config_.enable_peer_blacklisting) return;
    peer_states_[peer_id].blacklisted = true;
    aiworld::log_warn("Peer blacklisted: " + peer_id);
}

bool SecurityManager::is_peer_blacklisted(const std::string& peer_id) const {
    auto it = peer_states_.find(peer_id);
    return it != peer_states_.end() && it->second.blacklisted;
}

void SecurityManager::record_anomaly(const std::string& peer_id, const std::string& type) {
    if (!config_.enable_anomaly_detection) return;
    peer_states_[peer_id].cheat_flags += 1;
    aiworld::log_warn("Anomaly detected for peer " + peer_id + ": " + type);
    if (peer_states_[peer_id].cheat_flags > 3 && config_.enable_peer_blacklisting) {
        blacklist_peer(peer_id);
    }
}

void SecurityManager::update_reputation(const std::string& peer_id, int delta) {
    if (!config_.enable_reputation_system) return;
    peer_states_[peer_id].reputation += delta;
    aiworld::log_info("Peer " + peer_id + " reputation updated to " + std::to_string(peer_states_[peer_id].reputation));
    if (peer_states_[peer_id].reputation < config_.min_reputation && config_.enable_peer_blacklisting) {
        blacklist_peer(peer_id);
    }
}

PeerSecurityState SecurityManager::get_peer_state(const std::string& peer_id) const {
    auto it = peer_states_.find(peer_id);
    if (it != peer_states_.end()) return it->second;
    return PeerSecurityState{peer_id, 0, 0, false};
}
// --- AOI Mesh/Peer Discovery/Interest-Based Topology Implementation (Scaffold) ---
AOIMeshManager::AOIMeshManager(const AOIConfig& config)
    : config_(config) {}

AOIMeshManager::~AOIMeshManager() {}

std::vector<PeerMeshInfo> AOIMeshManager::discover_peers(const std::string& my_id, float x, float y, float z) {
    // Example: Query a shared peer registry (could be a distributed DB, ZeroMQ PUB/SUB, or in-memory map)
    // For this implementation, assume a static registry for demonstration.
    // In production, replace with real-time discovery via ZeroMQ PUB/SUB or distributed service.

    std::vector<PeerMeshInfo> discovered;
    // Simulate registry (replace with real registry in production)
    for (const auto& [peer_id, info] : peer_registry_) {
        if (peer_id == my_id) continue;
        float dx = info.x - x;
        float dy = info.y - y;
        float dz = info.z - z;
        float dist = std::sqrt(dx*dx + dy*dy + dz*dz);
        if (dist <= config_.peripheral_radius) {
            PeerMeshInfo mesh_info = info;
            mesh_info.distance = dist;
            discovered.push_back(mesh_info);
        }
    }
    return discovered;
}

std::vector<PeerMeshInfo> AOIMeshManager::select_mesh_peers(const std::vector<PeerMeshInfo>& candidates) {
    // Score peers: higher score = closer, more active, more bandwidth, higher reputation
    // score = (1/distance) * activity_weight * bandwidth * reputation
    std::vector<PeerMeshInfo> sorted = candidates;
    std::sort(sorted.begin(), sorted.end(), [](const PeerMeshInfo& a, const PeerMeshInfo& b) {
        float score_a = (a.distance > 0 ? 1.0f / a.distance : 1.0f) * a.activity_weight * a.bandwidth * (a.reputation > 0 ? a.reputation : 1);
        float score_b = (b.distance > 0 ? 1.0f / b.distance : 1.0f) * b.activity_weight * b.bandwidth * (b.reputation > 0 ? b.reputation : 1);
        return score_a > score_b;
    });
    // Limit to max_peers
    if (sorted.size() > static_cast<size_t>(config_.max_peers))
        sorted.resize(config_.max_peers);
    current_mesh_ = sorted;
    return current_mesh_;
}

void AOIMeshManager::prune_mesh() {
    if (current_mesh_.size() > static_cast<size_t>(config_.max_peers))
        current_mesh_.resize(config_.max_peers);
}

void AOIMeshManager::refresh() {
    // In production, this would be called periodically (e.g., every 10s) to update the mesh
    // For demonstration, simply re-discover and re-select mesh peers
    auto my_id = this_peer_id_;
    float x = this_x_;
    float y = this_y_;
    float z = this_z_;
    auto candidates = discover_peers(my_id, x, y, z);
    select_mesh_peers(candidates);
    prune_mesh();
    aiworld::log_info("AOIMeshManager: Mesh refreshed, current mesh size: " + std::to_string(current_mesh_.size()));
}

std::vector<PeerMeshInfo> AOIMeshManager::get_current_mesh() const {
    return current_mesh_;
}
// --- Protocol Selection and Traffic Flow Implementation (Scaffold) ---
aiworld::AIWorldPlugin::ProtocolType aiworld_current_protocol = aiworld::AIWorldPlugin::ProtocolType::ZEROMQ;

void aiworld::AIWorldPlugin::set_protocol(ProtocolType proto) {
    aiworld_current_protocol = proto;
    aiworld::log_info("Protocol set to: " + std::to_string(static_cast<int>(proto)));
}

aiworld::AIWorldPlugin::ProtocolType aiworld::AIWorldPlugin::get_protocol() {
    return aiworld_current_protocol;
}

aiworld::AIWorldPlugin::ProtocolType aiworld::AIWorldPlugin::select_protocol_for_message(int message_type) {
    // For now, always use ZEROMQ for compatibility.
    // In future: switch based on message_type (e.g., QUIC for movement, gRPC for DB, etc.)
    return aiworld_current_protocol;
}

void aiworld::AIWorldPlugin::force_server_protocol() {
    set_protocol(ProtocolType::ZEROMQ);
    aiworld::log_warn("Forcing all traffic to SERVER/ZeroMQ protocol (fallback)");
}
// --- P2P Enable/Disable and Fallback Logic Implementation ---

void aiworld::AIWorldPlugin::set_p2p_enabled(bool enabled) {
    // Thread-safe setter for P2P enable/disable
    this->p2p_enabled = enabled;
    aiworld::log_info(std::string("P2P ") + (enabled ? "ENABLED" : "DISABLED") + " at runtime");
}

bool aiworld::AIWorldPlugin::is_p2p_enabled() {
    return this->p2p_enabled;
}

void aiworld::AIWorldPlugin::auto_configure_p2p() {
    // TODO: Load from config file or environment variable
    // For now, default to false for maximum compatibility
    set_p2p_enabled(false);
    aiworld::log_info("Auto-configured P2P: DISABLED (default)");
}

void aiworld::AIWorldPlugin::fallback_to_server_only_mode() {
    set_p2p_enabled(false);
    aiworld::log_warn("Falling back to SERVER-ONLY mode (P2P forcibly disabled)");
}
namespace aiworld {

AIWorldPlugin::AIWorldPlugin()
    : is_initialized(false)
{
    // Set the AIWorld server endpoint (could be loaded from config)
    std::string endpoint = "tcp://127.0.0.1:5555";
    ipc_client = std::make_unique<AIWorldIPCClient>(endpoint);
}

AIWorldPlugin::~AIWorldPlugin() {
    shutdown();
}

bool AIWorldPlugin::initialize() {
    if (is_initialized) return true;
    log_info("Initializing AIWorldPlugin...");
    if (!ipc_client->connect()) {
        log_error("Failed to connect to AIWorld server via ZeroMQ.");
        return false;
    }
    ipc_client->start_receive_thread();
    is_initialized = true;
    log_info("AIWorldPlugin initialized and connected to AIWorld server.");
    return true;
}

void AIWorldPlugin::shutdown() {
    if (!is_initialized) return;
    log_info("Shutting down AIWorldPlugin...");
    ipc_client->stop_receive_thread();
    is_initialized = false;
}

void AIWorldPlugin::on_tick() {
    // Called every server tick; could poll for async events or process IPC messages
    // (In practice, event-driven receive thread handles most async IPC)
}

std::string AIWorldPlugin::handle_script_command(const std::string& command, const std::string& args) {
    // Handle aiworld_assign_mission, aiworld_query_state, etc.
    nlohmann::json payload;
    payload["command"] = command;
    payload["args"] = args;
    aiworld::AIWorldMessage msg(aiworld::IPCMessageType::AI_ACTION_REQUEST, aiworld::generate_correlation_id(), payload);
    if (!ipc_client->send_message(msg)) {
        aiworld::log_error("Failed to send script command to AIWorld server.");
        return "{\"error\": \"IPC send failed\"}";
    }

    // Synchronous: block and wait for response
    aiworld::AIWorldMessage resp_msg;
    if (!ipc_client->receive_message(resp_msg, true)) {
        aiworld::log_error("Failed to receive response from AIWorld server.");
        return "{\"error\": \"IPC receive failed\"}";
    }
    // Return the actual response payload as JSON string
    return resp_msg.payload.dump();
}

} // namespace aiworld