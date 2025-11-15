/**
 * aiworld_plugin.cpp
 * rAthena AIWorld Plugin/Module (ZeroMQ IPC)
 * Implements plugin lifecycle, ZeroMQ IPC client, and integration with rAthena event loop and script engine.
 */

#include "aiworld_plugin.hpp"
#include "aiworld_utils.hpp"
#include <nlohmann/json.hpp>
#include <iostream>

// --- Security, Anti-Cheat, and Authority Enforcement Implementation (Scaffold) ---
SecurityManager::SecurityManager(const SecurityConfig& config)
    : config_(config) {}

SecurityManager::~SecurityManager() {}

bool SecurityManager::validate_signature(const std::string& peer_id, const std::string& payload, const std::string& signature) {
    if (!config_.enable_signature_validation) return true;
    // TODO: Implement real signature validation (ED25519, etc.)
    // For now, always return true (stub)
    return true;
}

void SecurityManager::blacklist_peer(const std::string& peer_id) {
    if (!config_.enable_peer_blacklisting) return;
    peer_states_[peer_id].blacklisted = true;
    log_warn("Peer blacklisted: " + peer_id);
}

bool SecurityManager::is_peer_blacklisted(const std::string& peer_id) const {
    auto it = peer_states_.find(peer_id);
    return it != peer_states_.end() && it->second.blacklisted;
}

void SecurityManager::record_anomaly(const std::string& peer_id, const std::string& type) {
    if (!config_.enable_anomaly_detection) return;
    peer_states_[peer_id].cheat_flags += 1;
    log_warn("Anomaly detected for peer " + peer_id + ": " + type);
    if (peer_states_[peer_id].cheat_flags > 3 && config_.enable_peer_blacklisting) {
        blacklist_peer(peer_id);
    }
}

void SecurityManager::update_reputation(const std::string& peer_id, int delta) {
    if (!config_.enable_reputation_system) return;
    peer_states_[peer_id].reputation += delta;
    log_info("Peer " + peer_id + " reputation updated to " + std::to_string(peer_states_[peer_id].reputation));
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
    // TODO: Query peer registry (e.g., via ZeroMQ or shared state) for all peers in AOI
    // For now, return empty (stub)
    return {};
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
    // TODO: Periodically refresh mesh topology (e.g., every 10s)
    // For now, stub
}

std::vector<PeerMeshInfo> AOIMeshManager::get_current_mesh() const {
    return current_mesh_;
}
// --- Protocol Selection and Traffic Flow Implementation (Scaffold) ---
AIWorldPlugin::ProtocolType current_protocol = AIWorldPlugin::ProtocolType::ZEROMQ;

void AIWorldPlugin::set_protocol(ProtocolType proto) {
    current_protocol = proto;
    log_info("Protocol set to: " + std::to_string(static_cast<int>(proto)));
}

AIWorldPlugin::ProtocolType AIWorldPlugin::get_protocol() {
    return current_protocol;
}

AIWorldPlugin::ProtocolType AIWorldPlugin::select_protocol_for_message(int message_type) {
    // For now, always use ZEROMQ for compatibility.
    // In future: switch based on message_type (e.g., QUIC for movement, gRPC for DB, etc.)
    return current_protocol;
}

void AIWorldPlugin::force_server_protocol() {
    set_protocol(ProtocolType::ZEROMQ);
    log_warn("Forcing all traffic to SERVER/ZeroMQ protocol (fallback)");
}
// --- P2P Enable/Disable and Fallback Logic Implementation ---
bool AIWorldPlugin::p2p_enabled = false;

void AIWorldPlugin::set_p2p_enabled(bool enabled) {
    // Thread-safe setter for P2P enable/disable
    p2p_enabled = enabled;
    log_info(std::string("P2P ") + (enabled ? "ENABLED" : "DISABLED") + " at runtime");
}

bool AIWorldPlugin::is_p2p_enabled() {
    return p2p_enabled;
}

void AIWorldPlugin::auto_configure_p2p() {
    // TODO: Load from config file or environment variable
    // For now, default to false for maximum compatibility
    set_p2p_enabled(false);
    log_info("Auto-configured P2P: DISABLED (default)");
}

void AIWorldPlugin::fallback_to_server_only_mode() {
    set_p2p_enabled(false);
    log_warn("Falling back to SERVER-ONLY mode (P2P forcibly disabled)");
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
    // Example: handle aiworld_assign_mission, aiworld_query_state, etc.
    nlohmann::json payload;
    payload["command"] = command;
    payload["args"] = args;
    // AIWorldMessage now expects IPCMessageType (enum class) as first argument
    // No cast needed, constructor signature matches
    if (!ipc_client->send_message(msg)) {
        log_error("Failed to send script command to AIWorld server.");
        return "{\"error\": \"IPC send failed\"}";
    }

    // For synchronous commands, block and wait for response (not implemented here)
    // For async, return immediately
    return "{\"status\": \"command sent\"}";
}

} // namespace aiworld