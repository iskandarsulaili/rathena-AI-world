#ifndef AI_MODULE_HPP
#define AI_MODULE_HPP

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <atomic>
#include <thread>
#include <queue>
#include <condition_variable>
#include "common/ai_types.hpp"
#include "common/ai_provider.hpp"
#include "common/ai_agent.hpp"
#include "memory/ai_memory.hpp"

namespace rathena {
namespace ai {

/**
 * @brief AI Module
 * 
 * This class is the main entry point for the AI system. It manages
 * AI providers, agents, and processes events.
 */
class AIModule {
private:
    // Providers
    std::map<std::string, std::shared_ptr<AIProvider>> providers_;
    std::mutex providersMutex_;
    
    // Agents
    std::map<std::string, std::shared_ptr<AIAgent>> agents_;
    std::mutex agentsMutex_;
    
    // Memory systems
    std::map<std::string, std::shared_ptr<AIMemory>> memories_;
    std::mutex memoriesMutex_;
    
    // Event queue
    std::queue<AIEvent> eventQueue_;
    std::mutex eventQueueMutex_;
    std::condition_variable eventQueueCV_;
    
    // Event processing thread
    std::thread eventProcessingThread_;
    std::atomic<bool> running_;
    
    // Configuration
    std::string providersConfigPath_;
    std::string agentsConfigPath_;
    
public:
    /**
     * @brief Constructor
     * 
     * @param providersConfigPath Path to the providers configuration file
     * @param agentsConfigPath Path to the agents configuration file
     */
    AIModule(const std::string& providersConfigPath = "conf/ai_providers.conf",
             const std::string& agentsConfigPath = "conf/ai_agents.conf");
    
    /**
     * @brief Destructor
     */
    ~AIModule();
    
    /**
     * @brief Initialize the AI module
     * 
     * @return true if initialization was successful, false otherwise
     */
    bool Initialize();
    
    /**
     * @brief Shutdown the AI module
     */
    void Shutdown();
    
    /**
     * @brief Load AI providers from configuration
     * 
     * @param configPath Path to the configuration file
     * @return true if providers were loaded successfully, false otherwise
     */
    bool LoadProviders(const std::string& configPath);
    
    /**
     * @brief Load AI agents from configuration
     * 
     * @param configPath Path to the configuration file
     * @return true if agents were loaded successfully, false otherwise
     */
    bool LoadAgents(const std::string& configPath);
    
    /**
     * @brief Add a provider
     * 
     * @param name The provider's name
     * @param provider The provider to add
     * @return true if the provider was added successfully, false otherwise
     */
    bool AddProvider(const std::string& name, std::shared_ptr<AIProvider> provider);
    
    /**
     * @brief Get a provider by name
     * 
     * @param name The provider's name
     * @return std::shared_ptr<AIProvider> The provider, or nullptr if not found
     */
    std::shared_ptr<AIProvider> GetProvider(const std::string& name);
    
    /**
     * @brief Get all providers
     * 
     * @return std::vector<std::shared_ptr<AIProvider>> The providers
     */
    std::vector<std::shared_ptr<AIProvider>> GetProviders();
    
    /**
     * @brief Remove a provider
     * 
     * @param name The provider's name
     * @return true if the provider was removed successfully, false otherwise
     */
    bool RemoveProvider(const std::string& name);
    
    /**
     * @brief Add an agent
     * 
     * @param name The agent's name
     * @param agent The agent to add
     * @return true if the agent was added successfully, false otherwise
     */
    bool AddAgent(const std::string& name, std::shared_ptr<AIAgent> agent);
    
    /**
     * @brief Get an agent by name
     * 
     * @param name The agent's name
     * @return std::shared_ptr<AIAgent> The agent, or nullptr if not found
     */
    std::shared_ptr<AIAgent> GetAgent(const std::string& name);
    
    /**
     * @brief Get all agents
     * 
     * @return std::vector<std::shared_ptr<AIAgent>> The agents
     */
    std::vector<std::shared_ptr<AIAgent>> GetAgents();
    
    /**
     * @brief Get agents by type
     * 
     * @param type The agent type
     * @return std::vector<std::shared_ptr<AIAgent>> The agents of the specified type
     */
    std::vector<std::shared_ptr<AIAgent>> GetAgentsByType(AIAgentType type);
    
    /**
     * @brief Remove an agent
     * 
     * @param name The agent's name
     * @return true if the agent was removed successfully, false otherwise
     */
    bool RemoveAgent(const std::string& name);
    
    /**
     * @brief Add a memory system
     * 
     * @param prefix The memory prefix
     * @param memory The memory system to add
     * @return true if the memory system was added successfully, false otherwise
     */
    bool AddMemory(const std::string& prefix, std::shared_ptr<AIMemory> memory);
    
    /**
     * @brief Get a memory system by prefix
     * 
     * @param prefix The memory prefix
     * @return std::shared_ptr<AIMemory> The memory system, or nullptr if not found
     */
    std::shared_ptr<AIMemory> GetMemory(const std::string& prefix);
    
    /**
     * @brief Remove a memory system
     * 
     * @param prefix The memory prefix
     * @return true if the memory system was removed successfully, false otherwise
     */
    bool RemoveMemory(const std::string& prefix);
    
    /**
     * @brief Queue an event for processing
     * 
     * @param event The event to queue
     * @return true if the event was queued successfully, false otherwise
     */
    bool QueueEvent(const AIEvent& event);
    
    /**
     * @brief Process an event immediately
     * 
     * @param event The event to process
     * @return true if the event was processed successfully, false otherwise
     */
    bool ProcessEvent(const AIEvent& event);
    
    /**
     * @brief Generate a response using a provider
     * 
     * @param providerName The provider's name
     * @param request The AI request
     * @return AIResponse The generated response
     */
    AIResponse GenerateResponse(const std::string& providerName, const AIRequest& request);
    
private:
    /**
     * @brief Event processing thread function
     */
    void EventProcessingThread();
    
    /**
     * @brief Process the next event in the queue
     * 
     * @return true if an event was processed, false if the queue was empty
     */
    bool ProcessNextEvent();
    
    /**
     * @brief Create a memory system for an agent
     * 
     * @param memoryType The memory system type
     * @param prefix The memory prefix
     * @param config The memory system configuration
     * @return std::shared_ptr<AIMemory> The created memory system
     */
    std::shared_ptr<AIMemory> CreateMemory(
        const std::string& memoryType,
        const std::string& prefix,
        const ConfigMap& config);
};

// Global AI module instance
extern AIModule* g_aiModule;

// Initialize the AI module
bool ai_init();

// Shutdown the AI module
void ai_final();

// Queue an event for processing
bool ai_queue_event(const AIEvent& event);

// Process an event immediately
bool ai_process_event(const AIEvent& event);

// Generate a response using a provider
AIResponse ai_generate_response(const std::string& providerName, const AIRequest& request);

// Get a provider by name
std::shared_ptr<AIProvider> ai_get_provider(const std::string& name);

// Get an agent by name
std::shared_ptr<AIAgent> ai_get_agent(const std::string& name);

// Get a memory system by prefix
std::shared_ptr<AIMemory> ai_get_memory(const std::string& prefix);

} // namespace ai
} // namespace rathena

#endif // AI_MODULE_HPP