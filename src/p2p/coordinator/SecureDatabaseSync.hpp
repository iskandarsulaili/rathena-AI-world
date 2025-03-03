#ifndef SECURE_DATABASE_SYNC_HPP
#define SECURE_DATABASE_SYNC_HPP

#include <memory>
#include <queue>
#include <mutex>
#include <thread>
#include <atomic>
#include <future>
#include <unordered_map>
#include "../common/types.hpp"

namespace rathena {
namespace p2p {

class SecureDatabaseSync {
public:
    struct Config {
        uint32_t sync_batch_size;
        uint32_t max_queue_size;
        uint32_t poll_interval_ms;
        bool enable_compression;
        std::string encryption_key_file;
    };

    explicit SecureDatabaseSync(const Config& config);
    ~SecureDatabaseSync();

    // Initialize sync system
    bool initialize();
    void start();
    void stop();

    // Query handling
    struct QueryRequest {
        uint32_t request_id;
        uint32_t host_id;
        std::string query_hash;     // Hash of the actual query
        std::vector<uint8_t> data;  // Encrypted parameters
        timestamp_t timestamp;
    };

    struct QueryResult {
        uint32_t request_id;
        bool success;
        std::vector<uint8_t> data;  // Encrypted result
        std::string error_hash;     // Hash of error message if any
    };

    // Host interface
    uint32_t queue_query(const QueryRequest& request);
    QueryResult get_result(uint32_t request_id);
    void acknowledge_result(uint32_t request_id);

private:
    // Sync queue management
    struct QueueEntry {
        QueryRequest request;
        std::shared_ptr<std::promise<QueryResult>> promise;
        std::chrono::system_clock::time_point expiry;
    };

    std::queue<QueueEntry> sync_queue_;
    std::mutex queue_mutex_;
    std::atomic<bool> running_;
    std::thread sync_thread_;
    Config config_;

    // Encryption context
    struct EncryptionContext {
        std::vector<uint8_t> key;
        std::vector<uint8_t> iv;
        uint32_t rotation_counter;
    };
    EncryptionContext encryption_;

    // Query templates
    struct QueryTemplate {
        std::string hash;
        std::string pattern;
        std::vector<std::string> parameters;
    };
    std::unordered_map<std::string, QueryTemplate> query_templates_;

    // Processing methods
    void process_queue();
    void rotate_encryption_keys();
    bool validate_query_template(const std::string& query_hash);
    std::vector<uint8_t> encrypt_parameters(const std::vector<std::string>& params);
    std::vector<std::string> decrypt_parameters(const std::vector<uint8_t>& data);
    std::vector<uint8_t> encrypt_result(const std::string& result);
    std::string decrypt_result(const std::vector<uint8_t>& data);

    // Template management
    void load_query_templates();
    std::string hash_query(const std::string& query);
    bool register_query_template(const std::string& query);

    // Security
    bool verify_host_permissions(host_id_t host_id, const std::string& query_hash);
    bool rate_limit_check(host_id_t host_id);
    void log_security_event(const std::string& event, host_id_t host_id);
};

// Implementation of template-based secure queries
class SecureQueryTemplate {
public:
    static SecureQueryTemplate create(const std::string& pattern);
    
    std::string get_hash() const { return hash_; }
    bool validate_parameters(const std::vector<std::string>& params) const;
    std::string build_query(const std::vector<std::string>& params) const;

private:
    std::string pattern_;
    std::string hash_;
    std::vector<std::string> parameter_types_;
};

// Real-time sync handler
class RealTimeSync {
public:
    struct Config {
        uint32_t batch_interval_ms;
        uint32_t max_batch_size;
        bool compress_batches;
    };

    struct BatchMessage {
        uint32_t batch_size;
        uint32_t total_requests;
        uint64_t timestamp;
        bool compressed;
        std::vector<uint8_t> data;
    };

    explicit RealTimeSync(const Config& config);
    
    // Batch management
    void add_to_batch(const SecureDatabaseSync::QueryRequest& request);
    void flush_batch();
    
    // Status
    size_t get_pending_count() const { return current_batch_.size(); }
    uint32_t get_average_latency() const { return avg_latency_; }

private:
    Config config_;
    std::vector<SecureDatabaseSync::QueryRequest> current_batch_;
    std::mutex batch_mutex_;
    std::atomic<uint32_t> avg_latency_;
    
    // Batch processing
    void process_batch(const std::vector<SecureDatabaseSync::QueryRequest>& batch);
    void handle_batch_result(const std::vector<SecureDatabaseSync::QueryResult>& results);
    
    // Helper methods
    std::vector<SecureDatabaseSync::QueryResult> send_to_coordinator(const BatchMessage& message);
    void handle_request_failure(const SecureDatabaseSync::QueryRequest& request);
    void handle_result_failure(const SecureDatabaseSync::QueryResult& result);
    void notify_request_complete(const SecureDatabaseSync::QueryResult& result);
};

} // namespace p2p
} // namespace rathena

#endif // SECURE_DATABASE_SYNC_HPP