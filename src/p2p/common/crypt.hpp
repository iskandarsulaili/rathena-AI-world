#ifndef P2P_COMMON_CRYPT_HPP
#define P2P_COMMON_CRYPT_HPP

#include <string>
#include <vector>
#include <array>
#include <memory>
#include <cstdint>

namespace rathena {
namespace p2p {

class PacketCrypto {
public:
    static const size_t KEY_SIZE = 32;
    static const size_t IV_SIZE = 16;
    static const size_t HMAC_SIZE = 32;

    PacketCrypto();
    ~PacketCrypto();

    // Key management
    void generate_key_pair();
    bool set_key(const std::vector<uint8_t>& key);
    std::vector<uint8_t> get_public_key() const;
    
    // Encryption/Decryption
    std::vector<uint8_t> encrypt(const std::vector<uint8_t>& data);
    std::vector<uint8_t> decrypt(const std::vector<uint8_t>& data);
    
    // Authentication
    std::vector<uint8_t> sign(const std::vector<uint8_t>& data);
    bool verify(const std::vector<uint8_t>& data, const std::vector<uint8_t>& signature);
    
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

class TokenGenerator {
public:
    static std::string generate_auth_token();
    static std::string generate_session_token();
    static bool verify_token(const std::string& token);
    static uint64_t get_token_expiry(const std::string& token);
private:
    static std::string create_token(const std::string& type, uint64_t expiry);
};

class SecureRandom {
public:
    static std::vector<uint8_t> generate_bytes(size_t length);
    static uint32_t generate_uint32();
    static uint64_t generate_uint64();
    static std::string generate_string(size_t length);
};

class PasswordHasher {
public:
    static std::string hash_password(const std::string& password);
    static bool verify_password(const std::string& password, const std::string& hash);
    
private:
    static const size_t SALT_SIZE = 16;
    static const size_t HASH_SIZE = 32;
    static const uint32_t ITERATIONS = 10000;
};

class DataEncryption {
public:
    // Configuration
    struct Config {
        std::string algorithm = "AES-256-GCM";
        uint32_t key_rotation_interval = 86400;  // 24 hours
        bool enable_compression = true;
    };

    explicit DataEncryption(const Config& config = Config());
    
    // Data operations
    std::vector<uint8_t> encrypt_data(const std::vector<uint8_t>& data);
    std::vector<uint8_t> decrypt_data(const std::vector<uint8_t>& encrypted_data);
    
    // String operations
    std::string encrypt_string(const std::string& data);
    std::string decrypt_string(const std::string& encrypted_data);
    
    // Key management
    void rotate_keys();
    bool export_key(std::vector<uint8_t>& key) const;
    bool import_key(const std::vector<uint8_t>& key);
    
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

class SecurityManager {
public:
    struct Config {
        uint32_t token_validity_period = 3600;    // 1 hour
        uint32_t max_failed_attempts = 5;
        uint32_t lockout_duration = 900;          // 15 minutes
        std::string token_secret;
    };

    explicit SecurityManager(const Config& config);
    
    // Authentication
    std::string create_auth_token(uint32_t host_id);
    bool verify_auth_token(const std::string& token, uint32_t& host_id);
    void invalidate_token(const std::string& token);
    
    // Access control
    bool check_rate_limit(uint32_t host_id);
    void record_failed_attempt(uint32_t host_id);
    bool is_host_locked_out(uint32_t host_id);
    
    // Security operations
    std::string hash_data(const std::string& data);
    bool verify_hash(const std::string& data, const std::string& hash);
    
private:
    struct AuthState {
        uint32_t failed_attempts;
        uint64_t lockout_until;
        std::vector<std::string> active_tokens;
    };
    
    std::unordered_map<uint32_t, AuthState> auth_states_;
    Config config_;
};

} // namespace p2p
} // namespace rathena

#endif // P2P_COMMON_CRYPT_HPP