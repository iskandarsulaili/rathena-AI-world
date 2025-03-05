#ifndef DATABASE_PROXY_HPP
#define DATABASE_PROXY_HPP

#include <memory>
#include <string>
#include <vector>
#include <queue>
#include <mutex>
#include <thread>
#include <functional>
#include <chrono>
#include "../common/types.hpp"
#include "../common/network.hpp"

namespace rathena {
namespace p2p {

class DatabaseProxy {
public:
    struct Config {
        uint16_t port;
        uint32_t max_connections;
        uint32_t connection_timeout;
        uint32_t retry_interval;
        uint32_t batch_size;
        bool enable_caching;
    };

    enum class QueryType {
        SELECT,
        INSERT,
        UPDATE,
        DELETE,
        PROCEDURE
    };

    struct QueryRequest {
        host_id_t host_id;
        QueryType type;
        std::string query;
        std::vector<std::string> parameters;
        uint32_t request_id;
        std::chrono::system_clock::time_point timestamp;
    };

    struct QueryResult {
        uint32_t request_id;
        bool success;
        std::string error_message;
        std::vector<std::vector<std::string>> rows;
    };

    // Geographic-aware host operations
    struct GeoPoint {
        double latitude;
        double longitude;
    };

    struct HostLocation {
        host_id_t host_id;
        GeoPoint location;
        std::string region;
        float performance_score;
    };

    struct SessionData {
        uint32_t session_id;
        uint32_t char_id;
        uint32_t account_id;
        host_id_t host_id;
        uint32_t map_id;
        std::chrono::system_clock::time_point started_at;
        std::chrono::system_clock::time_point last_ping;
        std::string connection_data;
    };

    struct MapSyncState {
        uint32_t sync_id;
        uint32_t map_id;
        host_id_t host_id;
        std::chrono::system_clock::time_point last_full_sync;
        std::chrono::system_clock::time_point last_delta_sync;
        std::string state_hash;
        enum class Status {
            Synced,
            Syncing,
            OutOfSync,
            Error
        } status;
        std::string sync_details;
    };

    // New high-level operations
    bool register_host_location(host_id_t host_id, const GeoPoint& location, const std::string& region);
    std::vector<HostLocation> find_nearest_hosts(const GeoPoint& location, uint32_t limit = 5);
    
    // Session management
    uint32_t create_session(const SessionData& session);
    bool update_session_ping(uint32_t session_id);
    bool end_session(uint32_t session_id);
    std::vector<SessionData> get_host_sessions(host_id_t host_id);
    
    // State synchronization
    bool update_map_sync_state(const MapSyncState& state);
    MapSyncState get_map_sync_state(uint32_t map_id, host_id_t host_id);
    std::vector<MapSyncState> get_out_of_sync_maps(host_id_t host_id);
    
    // Migration tracking
    struct MigrationEvent {
        uint32_t char_id;
        host_id_t source_host;
        host_id_t target_host;
        std::string reason;
    };
    bool record_migration(const MigrationEvent& migration);

    explicit DatabaseProxy(const Config& config);
    ~DatabaseProxy();

    // Connection management
    bool initialize();
    void start();
    void stop();
    bool is_running() const { return running_; }

    // Query handling
    uint32_t submit_query(const QueryRequest& request);
    void register_result_handler(std::function<void(const QueryResult&)> handler);
    bool cancel_query(uint32_t request_id);

    // Batch operations
    void queue_batch_operation(const QueryRequest& request);
    void flush_batch_queue();
    void set_batch_handler(std::function<void(const std::vector<QueryResult>&)> handler) {
        batch_handler_ = std::move(handler);
    }

private:
    // Query processing
    void process_query_queue();
    void update_metrics(const QueryRequest& request, const QueryResult& result);
    
    // Security
    bool authenticate_host(host_id_t host_id);
    std::string encrypt_sensitive_data(const std::string& data);
    std::string decrypt_sensitive_data(const std::string& data);
    bool check_rate_limit(host_id_t host_id);
    
    // Geographic utilities
    double calculate_distance(const GeoPoint& p1, const GeoPoint& p2);
    std::string serialize_point(const GeoPoint& point);
    GeoPoint deserialize_point(const std::string& data);

    // Cache management
    struct CacheEntry {
        std::vector<std::vector<std::string>> data;
        std::chrono::system_clock::time_point timestamp;
        uint32_t access_count;
    };
    
    // State tracking for rate limiting
    bool check_rate_limit(host_id_t host_id);
    void update_rate_limit(host_id_t host_id, bool success);
    
    // Rate limiting
    struct HostState {
        uint32_t query_count;
        uint32_t error_count;
        std::chrono::system_clock::time_point last_query;
        bool in_transaction;
        uint32_t failed_attempts;
        std::chrono::system_clock::time_point lockout_until;
    };
    
    // Thread safety
    std::mutex state_mutex_;
    std::mutex cache_mutex_;
    std::mutex queue_mutex_;
    
    // Processing threads
    std::thread query_thread_;
    std::thread batch_thread_;
    
    // State tracking
    std::unordered_map<host_id_t, HostState> host_states_;
    std::unordered_map<std::string, CacheEntry> query_cache_;
    std::queue<QueryRequest> query_queue_;
    std::queue<QueryRequest> batch_queue_;
    
    // Callbacks
    std::function<void(const QueryResult&)> result_handler_;
    std::function<void(const std::vector<QueryResult>&)> batch_handler_;
    
    // Configuration
    Config config_;
    bool running_;
    
    // Metrics
    struct Metrics {
        uint32_t total_queries;
        uint32_t cached_hits;
        uint32_t errors;
        uint32_t retries;
        float average_response_time;
    } metrics_;
    
    void log_metrics();
    
    // Implementation details
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace p2p
} // namespace rathena

#endif // DATABASE_PROXY_HPP