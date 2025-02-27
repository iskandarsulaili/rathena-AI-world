#ifndef NETWORK_SECURITY_HPP
#define NETWORK_SECURITY_HPP

#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <memory>
#include <atomic>
#include <optional>

namespace rathena {

// Forward declarations
class P2PNetworkManager;
class Database;

/**
 * @brief Authentication data
 */
struct AuthenticationData {
    std::string peerId;
    std::string challenge;
    std::string response;
    std::string signature;
    std::map<std::string, std::string> metadata;
};

/**
 * @brief Encryption algorithm
 */
enum class EncryptionAlgorithm {
    NONE = 0,
    AES_128_CBC = 1,
    AES_256_CBC = 2,
    AES_128_GCM = 3,
    AES_256_GCM = 4
};

/**
 * @brief Packet security level
 */
enum class PacketSecurityLevel {
    NONE = 0,
    INTEGRITY = 1,
    ENCRYPTION = 2,
    ENCRYPTION_AND_INTEGRITY = 3
};

/**
 * @brief Network security manager
 * 
 * This class manages security for network communications, including
 * authentication, encryption, and integrity checking.
 */
class NetworkSecurityManager {
private:
    // Network manager
    std::shared_ptr<P2PNetworkManager> network_;
    
    // Database
    std::shared_ptr<Database> db_;
    
    // Security mutex
    std::mutex securityMutex_;
    
    // Peer keys
    std::map<std::string, std::string> peerSharedSecrets_;
    std::mutex peerSharedSecretsMutex_;
    
    // Configuration
    bool encryptionEnabled_;
    EncryptionAlgorithm encryptionAlgorithm_;
    bool authenticationRequired_;
    std::string sharedSecret_;
    std::string certificatePath_;
    std::string privateKeyPath_;
    
    // Session keys
    std::map<std::string, std::vector<uint8_t>> sessionKeys_;
    std::mutex sessionKeysMutex_;
    
public:
    /**
     * @brief Constructor
     * 
     * @param network The P2P network manager
     * @param db The database
     */
    NetworkSecurityManager(std::shared_ptr<P2PNetworkManager> network, std::shared_ptr<Database> db);
    
    /**
     * @brief Destructor
     */
    ~NetworkSecurityManager();
    
    /**
     * @brief Initialize the security manager
     * 
     * @return true if initialization was successful, false otherwise
     */
    bool Initialize();
    
    /**
     * @brief Shutdown the security manager
     */
    void Shutdown();
    
    /**
     * @brief Load configuration
     * 
     * @param configPath Path to the configuration file
     * @return true if configuration was loaded successfully, false otherwise
     */
    bool LoadConfig(const std::string& configPath);
    
    /**
     * @brief Authenticate a peer
     * 
     * @param peerId The peer ID
     * @param authData The authentication data
     * @return true if authentication was successful, false otherwise
     */
    bool AuthenticatePeer(const std::string& peerId, const AuthenticationData& authData);
    
    /**
     * @brief Generate a challenge for a peer
     * 
     * @param peerId The peer ID
     * @return std::string The challenge
     */
    std::string GenerateChallenge(const std::string& peerId);
    
    /**
     * @brief Generate a response to a challenge
     * 
     * @param challenge The challenge
     * @return std::string The response
     */
    std::string GenerateResponse(const std::string& challenge);
    
    /**
     * @brief Verify a packet
     * 
     * @param peerId The peer ID
     * @param packetData The packet data
     * @param signature The packet signature
     * @return true if the packet is valid, false otherwise
     */
    bool VerifyPacket(const std::string& peerId, const std::vector<uint8_t>& packetData, const std::string& signature);
    
    /**
     * @brief Sign a packet
     * 
     * @param packetData The packet data
     * @return std::string The packet signature
     */
    std::string SignPacket(const std::vector<uint8_t>& packetData);
    
    /**
     * @brief Encrypt a packet
     * 
     * @param peerId The peer ID
     * @param packetData The packet data
     * @param securityLevel The packet security level
     * @return std::vector<uint8_t> The encrypted packet data
     */
    std::vector<uint8_t> EncryptPacket(const std::string& peerId, const std::vector<uint8_t>& packetData, PacketSecurityLevel securityLevel);
    
    /**
     * @brief Decrypt a packet
     * 
     * @param peerId The peer ID
     * @param encryptedData The encrypted packet data
     * @param securityLevel The packet security level
     * @return std::optional<std::vector<uint8_t>> The decrypted packet data, or std::nullopt if decryption failed
     */
    std::optional<std::vector<uint8_t>> DecryptPacket(const std::string& peerId, const std::vector<uint8_t>& encryptedData, PacketSecurityLevel securityLevel);
    
    /**
     * @brief Establish a session key with a peer
     * 
     * @param peerId The peer ID
     * @return true if a session key was established successfully, false otherwise
     */
    bool EstablishSessionKey(const std::string& peerId);
    
    /**
     * @brief Get a peer's session key
     * 
     * @param peerId The peer ID
     * @return std::optional<std::vector<uint8_t>> The session key, or std::nullopt if not found
     */
    std::optional<std::vector<uint8_t>> GetSessionKey(const std::string& peerId);
    
    /**
     * @brief Set a peer's shared secret
     * 
     * @param peerId The peer ID
     * @param sharedSecret The shared secret
     */
    void SetPeerSharedSecret(const std::string& peerId, const std::string& sharedSecret);
    
    /**
     * @brief Get a peer's shared secret
     * 
     * @param peerId The peer ID
     * @return std::optional<std::string> The shared secret, or std::nullopt if not found
     */
    std::optional<std::string> GetPeerSharedSecret(const std::string& peerId);
    
    /**
     * @brief Set whether encryption is enabled
     * 
     * @param enabled Whether encryption is enabled
     */
    void SetEncryptionEnabled(bool enabled);
    
    /**
     * @brief Set the encryption algorithm
     * 
     * @param algorithm The encryption algorithm
     */
    void SetEncryptionAlgorithm(EncryptionAlgorithm algorithm);
    
    /**
     * @brief Set whether authentication is required
     * 
     * @param required Whether authentication is required
     */
    void SetAuthenticationRequired(bool required);
    
    /**
     * @brief Set the shared secret
     * 
     * @param sharedSecret The shared secret
     */
    void SetSharedSecret(const std::string& sharedSecret);
    
    /**
     * @brief Set the certificate path
     * 
     * @param certificatePath The certificate path
     */
    void SetCertificatePath(const std::string& certificatePath);
    
    /**
     * @brief Set the private key path
     * 
     * @param privateKeyPath The private key path
     */
    void SetPrivateKeyPath(const std::string& privateKeyPath);
    
private:
    /**
     * @brief Generate a random key
     * 
     * @param length The key length in bytes
     * @return std::vector<uint8_t> The generated key
     */
    std::vector<uint8_t> GenerateRandomKey(size_t length);
    
    /**
     * @brief Derive a key from a shared secret
     * 
     * @param sharedSecret The shared secret
     * @param salt The salt
     * @param keyLength The key length in bytes
     * @return std::vector<uint8_t> The derived key
     */
    std::vector<uint8_t> DeriveKey(const std::string& sharedSecret, const std::vector<uint8_t>& salt, size_t keyLength);
    
    /**
     * @brief Compute an HMAC
     * 
     * @param key The key
     * @param data The data
     * @return std::string The HMAC
     */
    std::string ComputeHMAC(const std::vector<uint8_t>& key, const std::vector<uint8_t>& data);
    
    /**
     * @brief Encrypt data with AES
     * 
     * @param key The key
     * @param iv The initialization vector
     * @param data The data to encrypt
     * @param algorithm The encryption algorithm
     * @return std::vector<uint8_t> The encrypted data
     */
    std::vector<uint8_t> EncryptAES(const std::vector<uint8_t>& key, const std::vector<uint8_t>& iv, const std::vector<uint8_t>& data, EncryptionAlgorithm algorithm);
    
    /**
     * @brief Decrypt data with AES
     * 
     * @param key The key
     * @param iv The initialization vector
     * @param encryptedData The encrypted data
     * @param algorithm The encryption algorithm
     * @return std::optional<std::vector<uint8_t>> The decrypted data, or std::nullopt if decryption failed
     */
    std::optional<std::vector<uint8_t>> DecryptAES(const std::vector<uint8_t>& key, const std::vector<uint8_t>& iv, const std::vector<uint8_t>& encryptedData, EncryptionAlgorithm algorithm);
};

// Global network security manager instance
extern NetworkSecurityManager* g_networkSecurityManager;

// Initialize the network security manager
bool network_security_init(std::shared_ptr<P2PNetworkManager> network, std::shared_ptr<Database> db);

// Shutdown the network security manager
void network_security_final();

// Convert an encryption algorithm to a string
std::string encryption_algorithm_to_string(EncryptionAlgorithm algorithm);

// Convert a string to an encryption algorithm
EncryptionAlgorithm string_to_encryption_algorithm(const std::string& str);

// Convert a packet security level to a string
std::string packet_security_level_to_string(PacketSecurityLevel level);

// Convert a string to a packet security level
PacketSecurityLevel string_to_packet_security_level(const std::string& str);

} // namespace rathena

#endif // NETWORK_SECURITY_HPP