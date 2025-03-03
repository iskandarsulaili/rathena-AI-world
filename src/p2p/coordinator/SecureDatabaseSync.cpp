#include "SecureDatabaseSync.hpp"
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/sha.h>
#include <zlib.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <future>
#include <chrono>

namespace rathena {
namespace p2p {

// Query template storage
std::unordered_map<std::string, SecureQueryTemplate> query_templates_;

SecureDatabaseSync::SecureDatabaseSync(const Config& config)
    : config_(config)
    , running_(false)
    , sync_queue_()
    , queue_mutex_() {
}

SecureDatabaseSync::~SecureDatabaseSync() {
    if (running_) {
        stop();
    }
}

bool SecureDatabaseSync::initialize() {
    try {
        // Load encryption keys
        std::ifstream key_file(config_.encryption_key_file, std::ios::binary);
        if (!key_file) {
            std::cerr << "Failed to load encryption key file" << std::endl;
            return false;
        }
        
        std::vector<uint8_t> key_data((std::istreambuf_iterator<char>(key_file)),
                                      std::istreambuf_iterator<char>());
        
        // Initialize encryption context
        encryption_.key.assign(key_data.begin(), key_data.begin() + 32);
        encryption_.iv.assign(key_data.begin() + 32, key_data.begin() + 48);
        encryption_.rotation_counter = 0;
        
        // Load query templates
        load_query_templates();
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Initialization error: " << e.what() << std::endl;
        return false;
    }
}

void SecureDatabaseSync::start() {
    if (running_) {
        return;
    }
    
    running_ = true;
    sync_thread_ = std::thread(&SecureDatabaseSync::process_queue, this);
}

void SecureDatabaseSync::stop() {
    if (!running_) {
        return;
    }
    
    running_ = false;
    if (sync_thread_.joinable()) {
        sync_thread_.join();
    }
}

uint32_t SecureDatabaseSync::queue_query(const QueryRequest& request) {
    if (!validate_query_template(request.query_hash)) {
        throw std::runtime_error("Invalid query template");
    }
    
    if (!verify_host_permissions(request.host_id, request.query_hash)) {
        throw std::runtime_error("Host not authorized for this query");
    }
    
    if (!rate_limit_check(request.host_id)) {
        throw std::runtime_error("Rate limit exceeded");
    }
    
    auto promise_ptr = std::make_shared<std::promise<QueryResult>>();
    auto future = promise_ptr->get_future();
    
    {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        
        if (sync_queue_.size() >= config_.max_queue_size) {
            throw std::runtime_error("Queue capacity exceeded");
        }
        
        QueueEntry entry{
            request,
            promise_ptr,
            std::chrono::system_clock::now() + std::chrono::milliseconds(30000)  // 30 second timeout
        };
        
        sync_queue_.push(std::move(entry));
    }
    
    try {
        auto result = future.get();
        return result.request_id;
    } catch (const std::exception& e) {
        throw std::runtime_error("Query processing failed: " + std::string(e.what()));
    }
}

SecureDatabaseSync::QueryResult SecureDatabaseSync::get_result(uint32_t request_id) {
    // Implementation for retrieving results
    // This would typically check a result cache or wait for completion
    return QueryResult{request_id, false, std::vector<uint8_t>(), ""};
}

void SecureDatabaseSync::process_queue() {
    RealTimeSync::Config rt_config{
        100,     // batch_interval_ms
        config_.sync_batch_size,
        config_.enable_compression
    };
    
    RealTimeSync real_time_sync(rt_config);
    
    while (running_) {
        std::vector<QueryRequest> batch;
        
        {
            std::lock_guard<std::mutex> lock(queue_mutex_);
            
            while (!sync_queue_.empty() && 
                   batch.size() < config_.sync_batch_size) {
                auto& entry = sync_queue_.front();
                
                if (std::chrono::system_clock::now() > entry.expiry) {
                    // Handle expired request
                    QueryResult timeout_result{
                        entry.request.request_id,
                        false,
                        std::vector<uint8_t>(),
                        hash_query("Query timeout")
                    };
                    entry.promise->set_value(timeout_result);
                    sync_queue_.pop();
                    continue;
                }
                
                batch.push_back(entry.request);
                sync_queue_.pop();
            }
        }
        
        if (!batch.empty()) {
            for (const auto& request : batch) {
                real_time_sync.add_to_batch(request);
            }
            
            if (batch.size() >= config_.sync_batch_size || 
                real_time_sync.get_pending_count() >= config_.sync_batch_size) {
                real_time_sync.flush_batch();
            }
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}

bool SecureDatabaseSync::validate_query_template(const std::string& query_hash) {
    return query_templates_.find(query_hash) != query_templates_.end();
}

void SecureDatabaseSync::load_query_templates() {
    // Load predefined query templates
    std::vector<std::string> templates = {
        "SELECT * FROM characters WHERE char_id = ?",
        "UPDATE inventory SET amount = ? WHERE char_id = ? AND nameid = ?",
        "INSERT INTO mail (sender_id, receiver_id, title, message) VALUES (?, ?, ?, ?)",
        // Add more templates as needed
    };
    
    for (const auto& query : templates) {
        register_query_template(query);
    }
}

std::string SecureDatabaseSync::hash_query(const std::string& query) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, query.c_str(), query.length());
    SHA256_Final(hash, &sha256);
    
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }
    
    return ss.str();
}

bool SecureDatabaseSync::register_query_template(const std::string& query) {
    std::string hash = hash_query(query);
    query_templates_[hash] = SecureQueryTemplate::create(query);
    return true;
}

std::vector<uint8_t> SecureDatabaseSync::encrypt_parameters(
    const std::vector<std::string>& params) {
    
    // Serialize parameters
    std::stringstream ss;
    for (const auto& param : params) {
        ss << param.length() << "|" << param;
    }
    std::string serialized = ss.str();
    
    // Compress if enabled
    std::vector<uint8_t> input_data;
    if (config_.enable_compression) {
        std::vector<uint8_t> compressed(serialized.length() * 2);  // Conservative estimate
        uLong compressed_size = compressed.size();
        
        if (compress2(compressed.data(), &compressed_size,
                     reinterpret_cast<const Bytef*>(serialized.data()),
                     serialized.length(), Z_BEST_SPEED) != Z_OK) {
            throw std::runtime_error("Compression failed");
        }
        
        compressed.resize(compressed_size);
        input_data = std::move(compressed);
    } else {
        input_data.assign(serialized.begin(), serialized.end());
    }
    
    // Encrypt
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        throw std::runtime_error("Failed to create encryption context");
    }
    
    if (!EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr,
                           encryption_.key.data(), encryption_.iv.data())) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to initialize encryption");
    }
    
    std::vector<uint8_t> encrypted(input_data.size() + EVP_MAX_BLOCK_LENGTH);
    int encrypted_length = 0;
    
    if (!EVP_EncryptUpdate(ctx, encrypted.data(), &encrypted_length,
                          input_data.data(), input_data.size())) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Encryption failed");
    }
    
    int final_length = 0;
    if (!EVP_EncryptFinal_ex(ctx, encrypted.data() + encrypted_length, &final_length)) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Encryption finalization failed");
    }
    
    encrypted.resize(encrypted_length + final_length);
    
    // Get tag
    unsigned char tag[16];
    if (!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, 16, tag)) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to get encryption tag");
    }
    
    // Append tag to encrypted data
    encrypted.insert(encrypted.end(), tag, tag + 16);
    
    EVP_CIPHER_CTX_free(ctx);
    return encrypted;
}

bool SecureDatabaseSync::verify_host_permissions(host_id_t host_id,
                                               const std::string& query_hash) {
    // Implementation would check host permissions against configured ACLs
    return true;  // Placeholder
}

bool SecureDatabaseSync::rate_limit_check(host_id_t host_id) {
    // Implementation would check rate limits for the host
    return true;  // Placeholder
}

void SecureDatabaseSync::log_security_event(const std::string& event,
                                          host_id_t host_id) {
    // Implementation would log security events for monitoring
}

} // namespace p2p
} // namespace rathena