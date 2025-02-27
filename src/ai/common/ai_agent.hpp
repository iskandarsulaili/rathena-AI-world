#ifndef AI_AGENT_HPP
#define AI_AGENT_HPP

#include <memory>
#include <string>
#include "ai_types.hpp"
#include "ai_provider.hpp"

namespace rathena {
namespace ai {

// Forward declaration
class AIMemory;

/**
 * @brief Interface for AI agents
 * 
 * This interface defines the contract for AI agents that can be used
 * by the AI module to process events and generate responses.
 */
class AIAgent {
public:
    /**
     * @brief Virtual destructor
     */
    virtual ~AIAgent() = default;
    
    /**
     * @brief Initialize the agent
     * 
     * @return true if initialization was successful, false otherwise
     */
    virtual bool Initialize() = 0;
    
    /**
     * @brief Process an event
     * 
     * @param event The AI event to process
     * @return true if the event was processed successfully, false otherwise
     */
    virtual bool ProcessEvent(const AIEvent& event) = 0;
    
    /**
     * @brief Get the agent's type
     * 
     * @return AIAgentType The agent's type
     */
    virtual AIAgentType GetType() const = 0;
    
    /**
     * @brief Get the agent's status
     * 
     * @return AIAgentStatus The agent's status
     */
    virtual AIAgentStatus GetStatus() const = 0;
    
    /**
     * @brief Set the agent's status
     * 
     * @param status The new status
     */
    virtual void SetStatus(AIAgentStatus status) = 0;
    
    /**
     * @brief Get the agent's name
     * 
     * @return std::string The agent's name
     */
    virtual std::string GetName() const = 0;
    
    /**
     * @brief Get the agent's provider
     * 
     * @return std::shared_ptr<AIProvider> The agent's provider
     */
    virtual std::shared_ptr<AIProvider> GetProvider() const = 0;
    
    /**
     * @brief Get the agent's memory
     * 
     * @return std::shared_ptr<AIMemory> The agent's memory
     */
    virtual std::shared_ptr<AIMemory> GetMemory() const = 0;
};

/**
 * @brief Base class for AI agents
 * 
 * This class provides a base implementation for AI agents.
 */
class AIAgentBase : public AIAgent {
protected:
    std::string name_;
    AIAgentType type_;
    std::shared_ptr<AIProvider> provider_;
    std::shared_ptr<AIMemory> memory_;
    ConfigMap config_;
    std::atomic<AIAgentStatus> status_;
    
public:
    /**
     * @brief Constructor
     * 
     * @param name The agent's name
     * @param type The agent's type
     * @param provider The agent's provider
     * @param memory The agent's memory
     * @param config The agent's configuration
     */
    AIAgentBase(
        const std::string& name,
        AIAgentType type,
        std::shared_ptr<AIProvider> provider,
        std::shared_ptr<AIMemory> memory,
        const ConfigMap& config);
    
    /**
     * @brief Destructor
     */
    ~AIAgentBase() override = default;
    
    /**
     * @brief Initialize the agent
     * 
     * @return true if initialization was successful, false otherwise
     */
    bool Initialize() override;
    
    /**
     * @brief Get the agent's type
     * 
     * @return AIAgentType The agent's type
     */
    AIAgentType GetType() const override;
    
    /**
     * @brief Get the agent's status
     * 
     * @return AIAgentStatus The agent's status
     */
    AIAgentStatus GetStatus() const override;
    
    /**
     * @brief Set the agent's status
     * 
     * @param status The new status
     */
    void SetStatus(AIAgentStatus status) override;
    
    /**
     * @brief Get the agent's name
     * 
     * @return std::string The agent's name
     */
    std::string GetName() const override;
    
    /**
     * @brief Get the agent's provider
     * 
     * @return std::shared_ptr<AIProvider> The agent's provider
     */
    std::shared_ptr<AIProvider> GetProvider() const override;
    
    /**
     * @brief Get the agent's memory
     * 
     * @return std::shared_ptr<AIMemory> The agent's memory
     */
    std::shared_ptr<AIMemory> GetMemory() const override;
    
protected:
    /**
     * @brief Get a configuration value
     * 
     * @param key The configuration key
     * @param defaultValue The default value to return if the key is not found
     * @return std::string The configuration value
     */
    std::string GetConfigValue(const std::string& key, const std::string& defaultValue = "") const;
    
    /**
     * @brief Get a configuration value as an integer
     * 
     * @param key The configuration key
     * @param defaultValue The default value to return if the key is not found
     * @return int The configuration value as an integer
     */
    int GetConfigValueInt(const std::string& key, int defaultValue = 0) const;
    
    /**
     * @brief Get a configuration value as a float
     * 
     * @param key The configuration key
     * @param defaultValue The default value to return if the key is not found
     * @return float The configuration value as a float
     */
    float GetConfigValueFloat(const std::string& key, float defaultValue = 0.0f) const;
    
    /**
     * @brief Get a configuration value as a boolean
     * 
     * @param key The configuration key
     * @param defaultValue The default value to return if the key is not found
     * @return bool The configuration value as a boolean
     */
    bool GetConfigValueBool(const std::string& key, bool defaultValue = false) const;
};

/**
 * @brief Factory for creating AI agents
 */
class AIAgentFactory {
public:
    /**
     * @brief Create an agent of the specified type
     * 
     * @param agentType The type of agent to create
     * @param name The agent's name
     * @param provider The agent's provider
     * @param memory The agent's memory
     * @param config The agent's configuration
     * @return std::unique_ptr<AIAgent> The created agent
     */
    static std::unique_ptr<AIAgent> CreateAgent(
        const std::string& agentType,
        const std::string& name,
        std::shared_ptr<AIProvider> provider,
        std::shared_ptr<AIMemory> memory,
        const ConfigMap& config);
};

} // namespace ai
} // namespace rathena

#endif // AI_AGENT_HPP