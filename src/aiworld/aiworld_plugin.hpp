#pragma once
// aiworld_plugin.hpp
// rAthena AIWorld Plugin/Module (ZeroMQ IPC)
// Defines plugin interface and lifecycle for rAthena integration

#include <string>
#include <memory>
#include "aiworld_ipc.hpp"
#include "aiworld_messages.hpp"

// --- P2P Enable/Disable and Fallback Logic ---
// This flag controls whether P2P is enabled for this plugin instance.
// It must be settable at runtime and default to false for maximum compatibility.
static bool p2p_enabled;

// Set P2P enable/disable at runtime (thread-safe)
static void set_p2p_enabled(bool enabled);

// Query P2P enable/disable state
static bool is_p2p_enabled();

// Called by the plugin on startup to auto-detect P2P config (from config file or env)
static void auto_configure_p2p();

// Called by the plugin to handle fallback (e.g., on connection failure or admin override)
static void fallback_to_server_only_mode();
namespace aiworld {

class AIWorldPlugin {
public:
    AIWorldPlugin();
    ~AIWorldPlugin();

    // Initialize plugin (called on map-server/char-server startup)
    bool initialize();


// --- Protocol Selection and Traffic Flow (Scaffold) ---
// Enum for protocol type
enum class ProtocolType {
    NONE,
    ZEROMQ,
    QUIC,
    GRPC,
    NATS
};

// Set protocol at runtime (thread-safe)
static void set_protocol(ProtocolType proto);

// Get current protocol
static ProtocolType get_protocol();

// Traffic flow control: select protocol for a given message type
static ProtocolType select_protocol_for_message(int message_type);

// Fallback: forcibly switch all traffic to ZEROMQ/server mode
static void force_server_protocol();
    // Shutdown plugin (called on server shutdown)
    void shutdown();

    // Main event loop integration (called every tick)
    void on_tick();

    // Handle incoming script command from rAthena script engine
    std::string handle_script_command(const std::string& command, const std::string& args);

    // IPC client for communication with AIWorld server
    std::unique_ptr<AIWorldIPCClient> ipc_client;

    // Plugin state
    bool is_initialized;
};

} // namespace aiworld
// --- Mesh/Peer Discovery/Interest-Based Topology (AOI, Peer Scoring) ---
// AOI (Area of Interest) configuration
struct AOIConfig {
    int critical_radius = 100;   // meters
    int extended_radius = 500;   // meters
    int peripheral_radius = 1000; // meters
    int max_peers = 30;
};

// Peer info for mesh scoring
struct PeerMeshInfo {
    std::string peer_id;
    float distance;
    float activity_weight;
    float bandwidth;
    int reputation;
    bool is_connected;
};

// AOI mesh manager interface
class AOIMeshManager {
public:
    AOIMeshManager(const AOIConfig& config);
    ~AOIMeshManager();

    // Discover peers within AOI
    std::vector<PeerMeshInfo> discover_peers(const std::string& my_id, float x, float y, float z);

    // Score and select peers for mesh
    std::vector<PeerMeshInfo> select_mesh_peers(const std::vector<PeerMeshInfo>& candidates);

    // Prune mesh if over max_peers
    void prune_mesh();

    // Refresh mesh topology (called every N seconds)
    void refresh();

    // Get current mesh
    std::vector<PeerMeshInfo> get_current_mesh() const;

private:
    AOIConfig config_;
    std::vector<PeerMeshInfo> current_mesh_;
};
// --- Security, Anti-Cheat, and Authority Enforcement Hooks (Scaffold) ---
// Security configuration for P2P/mesh
struct SecurityConfig {
    bool enable_signature_validation = true;
    bool enable_peer_blacklisting = true;
    bool enable_anomaly_detection = true;
    bool enable_reputation_system = true;
    int min_reputation = 0;
};

// Peer reputation/cheat state
struct PeerSecurityState {
    std::string peer_id;
    int reputation;
    int cheat_flags;
    bool blacklisted;
};

// Security manager interface
class SecurityManager {
public:
    SecurityManager(const SecurityConfig& config);
    ~SecurityManager();

    // Validate signature for a message/packet
    bool validate_signature(const std::string& peer_id, const std::string& payload, const std::string& signature);

    // Blacklist a peer
    void blacklist_peer(const std::string& peer_id);

    // Check if peer is blacklisted
    bool is_peer_blacklisted(const std::string& peer_id) const;

    // Record anomaly/cheat event
    void record_anomaly(const std::string& peer_id, const std::string& type);

    // Update peer reputation
    void update_reputation(const std::string& peer_id, int delta);

    // Get peer security state
    PeerSecurityState get_peer_state(const std::string& peer_id) const;

private:
    SecurityConfig config_;
    std::unordered_map<std::string, PeerSecurityState> peer_states_;
};