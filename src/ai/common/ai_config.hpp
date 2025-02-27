#ifndef AI_CONFIG_HPP
#define AI_CONFIG_HPP

#include <string>
#include <map>

namespace ai {

/**
 * Configuration structure for AI agents
 * Contains settings for initialization and runtime behavior
 */
class AIConfig {
public:
    AIConfig() = default;
    explicit AIConfig(const std::string& name);

    // Basic agent info
    int agentId = 0;              // Unique identifier for the agent
    std::string agentName;        // Display name of the agent
    std::string agentType;        // Type of agent (e.g., "beggar", "legend")
    bool isEnabled = true;        // Whether the agent is active

    // Location settings
    int mapId = 0;               // Starting map ID
    int x = 0;                   // Starting X coordinate
    int y = 0;                   // Starting Y coordinate
    float moveSpeed = 1.0f;      // Movement speed multiplier

    // AI provider settings
    std::string providerType = "azure_openai";  // AI backend to use
    std::string modelName = "gpt-4";           // Model name for AI requests
    int contextWindow = 4096;                  // Context window size
    float temperature = 0.7f;                  // Response temperature

    // Memory settings
    bool usePersistentMemory = true;          // Whether to use persistent memory
    std::string memoryProvider = "chroma";     // Vector store provider
    std::string embeddingModel = "text-embedding-3-small"; // Embedding model

    // Timing settings
    int updateInterval = 1000;    // Update interval in milliseconds
    int responseTimeout = 5000;   // AI response timeout in milliseconds

    // Custom parameters
    std::map<std::string, std::string> customParams;

    // Helper methods
    bool getCustomParamBool(const std::string& key, bool defaultValue = false) const;
    int getCustomParamInt(const std::string& key, int defaultValue = 0) const;
    float getCustomParamFloat(const std::string& key, float defaultValue = 0.0f) const;
    std::string getCustomParamString(const std::string& key, const std::string& defaultValue = "") const;

    void setCustomParam(const std::string& key, const std::string& value);
    bool hasCustomParam(const std::string& key) const;

    // Load/save methods
    bool loadFromFile(const std::string& filename);
    bool saveToFile(const std::string& filename) const;
};

} // namespace ai

#endif // AI_CONFIG_HPP