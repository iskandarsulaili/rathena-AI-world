#ifndef AI_MEMORY_HPP
#define AI_MEMORY_HPP

#include <string>
#include <vector>
#include <optional>
#include <memory>
#include "../common/ai_types.hpp"

namespace rathena {
namespace ai {

/**
 * @brief Interface for AI memory systems
 * 
 * This interface defines the contract for AI memory systems that can be used
 * by AI agents to store and retrieve memory data.
 */
class AIMemory {
public:
    /**
     * @brief Virtual destructor
     */
    virtual ~AIMemory() = default;
    
    /**
     * @brief Initialize the memory system
     * 
     * @return true if initialization was successful, false otherwise
     */
    virtual bool Initialize() = 0;
    
    /**
     * @brief Store a memory
     * 
     * @param key The memory key
     * @param data The memory data
     * @return true if the memory was stored successfully, false otherwise
     */
    virtual bool StoreMemory(const std::string& key, const AIMemoryData& data) = 0;
    
    /**
     * @brief Retrieve a memory
     * 
     * @param key The memory key
     * @return std::optional<AIMemoryData> The memory data, or std::nullopt if not found
     */
    virtual std::optional<AIMemoryData> RetrieveMemory(const std::string& key) = 0;
    
    /**
     * @brief Search memories
     * 
     * @param query The memory query
     * @return std::vector<AIMemoryData> The matching memories
     */
    virtual std::vector<AIMemoryData> SearchMemories(const AIMemoryQuery& query) = 0;
    
    /**
     * @brief Clear memories
     * 
     * @param keyPrefix The key prefix to clear
     * @return true if the memories were cleared successfully, false otherwise
     */
    virtual bool ClearMemories(const std::string& keyPrefix) = 0;
    
    /**
     * @brief Get the memory prefix
     * 
     * @return std::string The memory prefix
     */
    virtual std::string GetPrefix() const = 0;
};

/**
 * @brief Factory for creating AI memory systems
 */
class AIMemoryFactory {
public:
    /**
     * @brief Create a memory system of the specified type
     * 
     * @param memoryType The type of memory system to create
     * @param prefix The memory prefix
     * @param config The configuration for the memory system
     * @return std::unique_ptr<AIMemory> The created memory system
     */
    static std::unique_ptr<AIMemory> CreateMemory(
        const std::string& memoryType,
        const std::string& prefix,
        const ConfigMap& config);
};

} // namespace ai
} // namespace rathena

#endif // AI_MEMORY_HPP