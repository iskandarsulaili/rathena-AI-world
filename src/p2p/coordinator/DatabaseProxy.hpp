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
    
    // Cache management
    struct CacheEntry {
        std::vector<std::vector<std::string>> data;
        std::chrono::system_clock::time_point timestamp;
        uint32_t access_count;
    };
    
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