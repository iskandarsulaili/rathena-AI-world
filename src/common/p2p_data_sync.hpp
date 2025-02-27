#ifndef P2P_DATA_SYNC_HPP
#define P2P_DATA_SYNC_HPP

#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <memory>
#include <atomic>
#include <thread>
#include <queue>
#include <condition_variable>
#include <chrono>
#include <optional>

namespace rathena {

// Forward declarations
class P2PNetworkManager;
class Database;

/**
 * @brief P2P data types
 */
enum class P2PDataType {
    CHARACTER = 0,
    ITEM = 1,
    QUEST = 2,
    GUILD = 3,
    MARKET = 4,
    AI_EVENT = 5,
    AI_WEATHER = 6,
    CUSTOM = 100
};

/**
 * @brief P2P sync request
 */
struct P2PSyncRequest {
    std::string peerId;
    P2PDataType dataType;
    std::string dataId;
    std::chrono::system_clock::time_point timestamp;
};

/**
 * @brief P2P sync data
 */
struct P2PSyncData {
    P2PDataType dataType;
    std::string dataId;
    std::string data;
    std::chrono::system_clock::time_point timestamp;
    std::map<std::string, std::string> metadata;
};

/**
 * @brief P2P conflict resolution strategy
 */
enum class P2PConflictResolution {
    TIMESTAMP = 0,
    SERVER_PRIORITY = 1,
    MANUAL = 2
};

/**
 * @brief P2P data synchronization manager
 * 
 * This class manages data synchronization between peers in the P2P network.
 */
class P2PDataSyncManager {
private:
    // Network manager
    std::shared_ptr<P2PNetworkManager> network_;
    
    // Database
    std::shared_ptr<Database> db_;
    
    // Sync mutex
    std::mutex syncMutex_;
    
    // Sync queue
    std::queue<P2PSyncRequest> syncQueue_;
    std::mutex syncQueueMutex_;
    std::condition_variable syncQueueCV_;
    
    // Sync thread
    std::thread syncThread_;
    std::atomic<bool> running_;
    
    // Configuration
    P2PConflictResolution conflictResolution_;
    std::map<P2PDataType, bool> syncEnabled_;
    std::chrono::seconds syncInterval_;
    
public:
    /**
     * @brief Constructor
     * 
     * @param network The P2P network manager
     * @param db The database
     */
    P2PDataSyncManager(std::shared_ptr<P2PNetworkManager> network, std::shared_ptr<Database> db);
    
    /**
     * @brief Destructor
     */
    ~P2PDataSyncManager();
    
    /**
     * @brief Initialize the sync manager
     * 
     * @return true if initialization was successful, false otherwise
     */
    bool Initialize();
    
    /**
     * @brief Shutdown the sync manager
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
     * @brief Sync data with peers
     * 
     * @param dataType The data type
     * @param dataId The data ID
     * @return true if the data was synced successfully, false otherwise
     */
    bool SyncData(P2PDataType dataType, const std::string& dataId);
    
    /**
     * @brief Handle a sync request from a peer
     * 
     * @param peerId The peer ID
     * @param request The sync request
     * @return true if the request was handled successfully, false otherwise
     */
    bool HandleSyncRequest(const std::string& peerId, const P2PSyncRequest& request);
    
    /**
     * @brief Handle sync data from a peer
     * 
     * @param peerId The peer ID
     * @param data The sync data
     * @return true if the data was handled successfully, false otherwise
     */
    bool HandleSyncData(const std::string& peerId, const P2PSyncData& data);
    
    /**
     * @brief Set the conflict resolution strategy
     * 
     * @param strategy The conflict resolution strategy
     */
    void SetConflictResolution(P2PConflictResolution strategy);
    
    /**
     * @brief Enable or disable syncing for a data type
     * 
     * @param dataType The data type
     * @param enabled Whether syncing is enabled
     */
    void SetSyncEnabled(P2PDataType dataType, bool enabled);
    
    /**
     * @brief Set the sync interval
     * 
     * @param interval The sync interval in seconds
     */
    void SetSyncInterval(std::chrono::seconds interval);
    
private:
    /**
     * @brief Sync thread function
     */
    void SyncThread();
    
    /**
     * @brief Process the next sync request in the queue
     * 
     * @return true if a request was processed, false if the queue was empty
     */
    bool ProcessNextSyncRequest();
    
    /**
     * @brief Build sync data for a data type and ID
     * 
     * @param dataType The data type
     * @param dataId The data ID
     * @return std::optional<P2PSyncData> The sync data, or std::nullopt if not found
     */
    std::optional<P2PSyncData> BuildSyncData(P2PDataType dataType, const std::string& dataId);
    
    /**
     * @brief Apply sync data
     * 
     * @param data The sync data
     * @return true if the data was applied successfully, false otherwise
     */
    bool ApplySyncData(const P2PSyncData& data);
    
    /**
     * @brief Resolve a conflict between local and remote data
     * 
     * @param localData The local data
     * @param remoteData The remote data
     * @param peerId The peer ID
     * @return P2PSyncData The resolved data
     */
    P2PSyncData ResolveConflict(const P2PSyncData& localData, const P2PSyncData& remoteData, const std::string& peerId);
    
    /**
     * @brief Get data from the database
     * 
     * @param dataType The data type
     * @param dataId The data ID
     * @return std::optional<std::string> The data, or std::nullopt if not found
     */
    std::optional<std::string> GetDataFromDB(P2PDataType dataType, const std::string& dataId);
    
    /**
     * @brief Save data to the database
     * 
     * @param dataType The data type
     * @param dataId The data ID
     * @param data The data
     * @return true if the data was saved successfully, false otherwise
     */
    bool SaveDataToDB(P2PDataType dataType, const std::string& dataId, const std::string& data);
    
    /**
     * @brief Get the last sync timestamp for a data type and ID
     * 
     * @param dataType The data type
     * @param dataId The data ID
     * @return std::chrono::system_clock::time_point The last sync timestamp
     */
    std::chrono::system_clock::time_point GetLastSyncTimestamp(P2PDataType dataType, const std::string& dataId);
    
    /**
     * @brief Update the last sync timestamp for a data type and ID
     * 
     * @param dataType The data type
     * @param dataId The data ID
     * @param timestamp The timestamp
     * @return true if the timestamp was updated successfully, false otherwise
     */
    bool UpdateLastSyncTimestamp(P2PDataType dataType, const std::string& dataId, std::chrono::system_clock::time_point timestamp);
};

// Global P2P data sync manager instance
extern P2PDataSyncManager* g_p2pDataSyncManager;

// Initialize the P2P data sync manager
bool p2p_data_sync_init(std::shared_ptr<P2PNetworkManager> network, std::shared_ptr<Database> db);

// Shutdown the P2P data sync manager
void p2p_data_sync_final();

// Sync data with peers
bool p2p_sync_data(P2PDataType dataType, const std::string& dataId);

// Convert a P2P data type to a string
std::string p2p_data_type_to_string(P2PDataType dataType);

// Convert a string to a P2P data type
P2PDataType p2p_string_to_data_type(const std::string& str);

} // namespace rathena

#endif // P2P_DATA_SYNC_HPP