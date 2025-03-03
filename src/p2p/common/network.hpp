#ifndef P2P_COMMON_NETWORK_HPP
#define P2P_COMMON_NETWORK_HPP

#include <vector>
#include <memory>
#include <string>
#include <functional>
#include <thread>
#include <atomic>
#include <queue>
#include <mutex>
#include "types.hpp"

namespace rathena {
namespace p2p {

// Forward declarations
class NetworkSession;
class NetworkManager;

enum class PacketType : uint16_t {
    // Host management
    HOST_REGISTER = 0x1000,
    HOST_UNREGISTER = 0x1001,
    HOST_METRICS = 0x1002,
    HOST_STATUS = 0x1003,
    
    // Map management
    MAP_REQUEST = 0x2000,
    MAP_ASSIGN = 0x2001,
    MAP_TRANSFER = 0x2002,
    MAP_STATUS = 0x2003,
    
    // Player management
    PLAYER_CONNECT = 0x3000,
    PLAYER_DISCONNECT = 0x3001,
    PLAYER_MOVE = 0x3002,
    PLAYER_ACTION = 0x3003,
    
    // Database operations
    DB_SYNC = 0x4000,
    DB_QUERY = 0x4001,
    DB_UPDATE = 0x4002,
    DB_RESPONSE = 0x4003,
    
    // Security
    AUTH_REQUEST = 0x5000,
    AUTH_RESPONSE = 0x5001,
    KEY_EXCHANGE = 0x5002,
    
    // System
    PING = 0x6000,
    PONG = 0x6001,
    ERROR = 0x6002
};

class NetworkPacket {
public:
    NetworkPacket(PacketType type);
    NetworkPacket(const uint8_t* data, size_t length);
    
    // Write methods
    void write_uint8(uint8_t value);
    void write_uint16(uint16_t value);
    void write_uint32(uint32_t value);
    void write_uint64(uint64_t value);
    void write_int8(int8_t value);
    void write_int16(int16_t value);
    void write_int32(int32_t value);
    void write_int64(int64_t value);
    void write_float(float value);
    void write_double(double value);
    void write_string(const std::string& value);
    void write_buffer(const std::vector<uint8_t>& buffer);
    
    // Read methods
    uint8_t read_uint8() const;
    uint16_t read_uint16() const;
    uint32_t read_uint32() const;
    uint64_t read_uint64() const;
    int8_t read_int8() const;
    int16_t read_int16() const;
    int32_t read_int32() const;
    int64_t read_int64() const;
    float read_float() const;
    double read_double() const;
    std::string read_string() const;
    std::vector<uint8_t> read_buffer(size_t length) const;
    
    // Packet properties
    PacketType get_type() const { return type_; }
    size_t get_length() const { return data_.size(); }
    const uint8_t* get_data() const { return data_.data(); }
    uint32_t get_sequence() const { return sequence_; }
    std::shared_ptr<NetworkSession> get_session() const { return session_.lock(); }
    
    // Packet flags
    bool is_encrypted() const { return flags_ & FLAG_ENCRYPTED; }
    bool is_compressed() const { return flags_ & FLAG_COMPRESSED; }
    void set_encrypted(bool value);
    void set_compressed(bool value);
    
    static const uint8_t FLAG_ENCRYPTED = 0x01;
    static const uint8_t FLAG_COMPRESSED = 0x02;

private:
    PacketType type_;
    std::vector<uint8_t> data_;
    mutable size_t read_pos_;
    uint32_t sequence_;
    uint8_t flags_;
    std::weak_ptr<NetworkSession> session_;
    
    friend class NetworkManager;
    void set_session(std::shared_ptr<NetworkSession> session) {
        session_ = session;
    }
};

class NetworkSession : public std::enable_shared_from_this<NetworkSession> {
public:
    NetworkSession(const NetworkAddress& remote_address);
    ~NetworkSession();
    
    // Session state
    bool is_connected() const { return connected_; }
    bool is_encrypted() const { return encrypted_; }
    const NetworkAddress& get_remote_address() const { return remote_address_; }
    uint32_t get_latency() const { return latency_; }
    
    // Packet operations
    void queue_packet(const NetworkPacket& packet);
    void process_incoming(const uint8_t* data, size_t length);
    
    // Metrics
    void update_latency(uint32_t new_latency) { latency_ = new_latency; }
    timestamp_t get_last_receive() const { return last_receive_; }

private:
    NetworkAddress remote_address_;
    std::atomic<bool> connected_;
    std::atomic<bool> encrypted_;
    std::atomic<uint32_t> latency_;
    timestamp_t last_receive_;
    std::queue<NetworkPacket> outgoing_queue_;
    std::mutex queue_mutex_;
    std::weak_ptr<NetworkManager> manager_;
    
    friend class NetworkManager;
    void set_manager(std::shared_ptr<NetworkManager> manager) {
        manager_ = manager;
    }
};

class NetworkManager : public std::enable_shared_from_this<NetworkManager> {
public:
    using PacketHandler = std::function<void(const NetworkPacket&)>;
    
    NetworkManager();
    ~NetworkManager();
    
    // Server operations
    bool start_server(uint16_t port);
    void stop_server();
    bool is_running() const { return running_; }
    
    // Connection management
    std::shared_ptr<NetworkSession> connect(const std::string& address, uint16_t port);
    void disconnect(const std::shared_ptr<NetworkSession>& session);
    
    // Packet handling
    void register_handler(PacketType type, PacketHandler handler);
    void send_packet(const std::shared_ptr<NetworkSession>& session, const NetworkPacket& packet);
    void handle_packet(const NetworkPacket& packet);
    
    // Security
    void enable_encryption(const std::shared_ptr<NetworkSession>& session);
    void rotate_keys(const std::shared_ptr<NetworkSession>& session);
    
    // Configuration
    void set_compression_threshold(size_t bytes) { compression_threshold_ = bytes; }
    void set_keep_alive_interval(uint32_t ms) { keep_alive_interval_ = ms; }
    void set_timeout(uint32_t ms) { timeout_ = ms; }
    
    // Metrics
    uint32_t get_average_latency() const;
    uint32_t get_connected_sessions() const { return active_sessions_; }
    uint32_t get_packets_per_second() const { return packets_per_second_; }

private:
    // Network thread
    void network_loop();
    void process_incoming();
    void process_outgoing();
    void check_timeouts();
    void send_keep_alive();
    
    // State
    std::atomic<bool> running_;
    std::thread network_thread_;
    std::unordered_map<PacketType, PacketHandler> handlers_;
    std::vector<std::shared_ptr<NetworkSession>> sessions_;
    std::mutex sessions_mutex_;
    
    // Configuration
    size_t compression_threshold_;
    uint32_t keep_alive_interval_;
    uint32_t timeout_;
    
    // Metrics
    std::atomic<uint32_t> active_sessions_;
    std::atomic<uint32_t> packets_per_second_;
    timestamp_t last_metrics_update_;
};

} // namespace p2p
} // namespace rathena

#endif // P2P_COMMON_NETWORK_HPP