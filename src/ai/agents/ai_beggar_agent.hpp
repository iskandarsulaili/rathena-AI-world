#ifndef AI_BEGGAR_AGENT_HPP
#define AI_BEGGAR_AGENT_HPP

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <optional>

#include "../common/ai_agent.hpp"
#include "../common/ai_types.hpp"
#include "../common/ai_request.hpp"
#include "../common/ai_response.hpp"
#include "../memory/ai_memory.hpp"

namespace rathena {
namespace ai {

/**
 * @brief AI Beggar Agent
 * 
 * This agent is responsible for managing beggar NPCs in the game world.
 * Beggars have unique personalities, stories, and quests that are generated
 * dynamically based on the game world and player interactions.
 */
class AIBeggarAgent : public AIAgent {
private:
    // Configuration
    ConfigMap config_;
    
    // Memory manager
    std::shared_ptr<AIMemory> memory_;
    
    // Cache for beggar data
    std::map<int, std::map<std::string, std::string>> beggarCache_;
    std::mutex beggarCacheMutex_;
    
    // Cache for beggar stories
    std::map<int, std::string> storyCache_;
    std::mutex storyCacheMutex_;
    
    // Cache for beggar quests
    std::map<int, std::vector<std::map<std::string, std::string>>> questCache_;
    std::mutex questCacheMutex_;
    
    // Cache for conversation history
    std::map<std::pair<int, int>, std::vector<std::pair<std::string, std::string>>> conversationCache_;
    std::mutex conversationCacheMutex_;
    
    /**
     * @brief Load beggar data from the database
     * 
     * @return bool True if successful, false otherwise
     */
    bool LoadBeggarData();
    
    /**
     * @brief Generate a beggar story
     * 
     * @param beggarId The beggar ID
     * @param beggarData The beggar data
     * @return std::string The generated story
     */
    std::string GenerateBeggarStory(int beggarId, const std::map<std::string, std::string>& beggarData);
    
    /**
     * @brief Generate a beggar quest
     * 
     * @param beggarId The beggar ID
     * @param charId The character ID
     * @param beggarData The beggar data
     * @param characterData The character data
     * @return std::map<std::string, std::string> The generated quest
     */
    std::map<std::string, std::string> GenerateBeggarQuest(int beggarId, int charId, 
                                                         const std::map<std::string, std::string>& beggarData,
                                                         const std::map<std::string, std::string>& characterData);
    
    /**
     * @brief Process a conversation with a beggar
     * 
     * @param beggarId The beggar ID
     * @param charId The character ID
     * @param message The message from the character
     * @param beggarData The beggar data
     * @param characterData The character data
     * @return std::string The response from the beggar
     */
    std::string ProcessConversation(int beggarId, int charId, const std::string& message,
                                   const std::map<std::string, std::string>& beggarData,
                                   const std::map<std::string, std::string>& characterData);
    
    /**
     * @brief Get the conversation history for a character and beggar
     * 
     * @param charId The character ID
     * @param beggarId The beggar ID
     * @return std::vector<std::pair<std::string, std::string>> The conversation history
     */
    std::vector<std::pair<std::string, std::string>> GetConversationHistory(int charId, int beggarId);
    
    /**
     * @brief Add a conversation entry to the history
     * 
     * @param charId The character ID
     * @param beggarId The beggar ID
     * @param message The message from the character
     * @param response The response from the beggar
     */
    void AddConversationEntry(int charId, int beggarId, const std::string& message, const std::string& response);
    
    /**
     * @brief Check if a character has completed a beggar's quest
     * 
     * @param charId The character ID
     * @param beggarId The beggar ID
     * @param questId The quest ID
     * @return bool True if the character has completed the quest, false otherwise
     */
    bool HasCompletedQuest(int charId, int beggarId, int questId);
    
    /**
     * @brief Check if a character has the required items for a beggar's quest
     * 
     * @param charId The character ID
     * @param questData The quest data
     * @return bool True if the character has the required items, false otherwise
     */
    bool HasRequiredItems(int charId, const std::map<std::string, std::string>& questData);
    
    /**
     * @brief Remove required items from a character's inventory
     * 
     * @param charId The character ID
     * @param questData The quest data
     * @return bool True if successful, false otherwise
     */
    bool RemoveRequiredItems(int charId, const std::map<std::string, std::string>& questData);
    
    /**
     * @brief Give rewards to a character
     * 
     * @param charId The character ID
     * @param questData The quest data
     * @return bool True if successful, false otherwise
     */
    bool GiveRewards(int charId, const std::map<std::string, std::string>& questData);
    
    /**
     * @brief Get character data from the database
     * 
     * @param charId The character ID
     * @return std::map<std::string, std::string> The character data
     */
    std::map<std::string, std::string> GetCharacterData(int charId);
    
    /**
     * @brief Get beggar data from the database
     * 
     * @param beggarId The beggar ID
     * @return std::map<std::string, std::string> The beggar data
     */
    std::map<std::string, std::string> GetBeggarData(int beggarId);
    
    /**
     * @brief Get quest data from the database
     * 
     * @param questId The quest ID
     * @return std::map<std::string, std::string> The quest data
     */
    std::map<std::string, std::string> GetQuestData(int questId);
    
    /**
     * @brief Generate a random personality for a beggar
     * 
     * @return std::map<std::string, std::string> The personality traits
     */
    std::map<std::string, std::string> GenerateRandomPersonality();
    
    /**
     * @brief Generate a random appearance for a beggar
     * 
     * @param personality The personality traits
     * @return std::map<std::string, std::string> The appearance traits
     */
    std::map<std::string, std::string> GenerateRandomAppearance(const std::map<std::string, std::string>& personality);
    
    /**
     * @brief Generate a random location for a beggar
     * 
     * @return std::map<std::string, std::string> The location data
     */
    std::map<std::string, std::string> GenerateRandomLocation();
    
public:
    /**
     * @brief Constructor
     * 
     * @param config The configuration
     * @param memory The memory manager
     */
    AIBeggarAgent(const ConfigMap& config, std::shared_ptr<AIMemory> memory);
    
    /**
     * @brief Destructor
     */
    virtual ~AIBeggarAgent();
    
    /**
     * @brief Initialize the agent
     * 
     * @return bool True if successful, false otherwise
     */
    bool Initialize() override;
    
    /**
     * @brief Process an event
     * 
     * @param event The event
     * @return AIResponse The response
     */
    AIResponse ProcessEvent(const AIEvent& event) override;
    
    /**
     * @brief Get the agent type
     * 
     * @return AIAgentType The agent type
     */
    AIAgentType GetType() const override;
    
    /**
     * @brief Get the agent name
     * 
     * @return std::string The agent name
     */
    std::string GetName() const override;
    
    /**
     * @brief Get the agent status
     * 
     * @return AIAgentStatus The agent status
     */
    AIAgentStatus GetStatus() const override;
    
    /**
     * @brief Create a new beggar
     * 
     * @param mapId The map ID
     * @param x The x coordinate
     * @param y The y coordinate
     * @return int The beggar ID
     */
    int CreateBeggar(int mapId, int x, int y);
    
    /**
     * @brief Delete a beggar
     * 
     * @param beggarId The beggar ID
     * @return bool True if successful, false otherwise
     */
    bool DeleteBeggar(int beggarId);
    
    /**
     * @brief List all beggars
     * 
     * @return std::vector<std::map<std::string, std::string>> The beggars
     */
    std::vector<std::map<std::string, std::string>> ListBeggars();
    
    /**
     * @brief Get beggar information
     * 
     * @param beggarId The beggar ID
     * @return std::map<std::string, std::string> The beggar information
     */
    std::map<std::string, std::string> GetBeggarInfo(int beggarId);
    
    /**
     * @brief Get beggar story
     * 
     * @param beggarId The beggar ID
     * @return std::string The beggar story
     */
    std::string GetBeggarStory(int beggarId);
    
    /**
     * @brief List beggar quests
     * 
     * @param beggarId The beggar ID
     * @return std::vector<std::map<std::string, std::string>> The beggar quests
     */
    std::vector<std::map<std::string, std::string>> ListBeggarQuests(int beggarId);
    
    /**
     * @brief Get beggar quest
     * 
     * @param beggarId The beggar ID
     * @param questId The quest ID
     * @return std::map<std::string, std::string> The beggar quest
     */
    std::map<std::string, std::string> GetBeggarQuest(int beggarId, int questId);
    
    /**
     * @brief Talk to beggar
     * 
     * @param charId The character ID
     * @param beggarId The beggar ID
     * @param message The message
     * @return std::string The response
     */
    std::string TalkToBeggar(int charId, int beggarId, const std::string& message);
    
    /**
     * @brief Accept beggar quest
     * 
     * @param charId The character ID
     * @param beggarId The beggar ID
     * @param questId The quest ID
     * @return bool True if successful, false otherwise
     */
    bool AcceptBeggarQuest(int charId, int beggarId, int questId);
    
    /**
     * @brief Complete beggar quest
     * 
     * @param charId The character ID
     * @param beggarId The beggar ID
     * @param questId The quest ID
     * @return bool True if successful, false otherwise
     */
    bool CompleteBeggarQuest(int charId, int beggarId, int questId);
    
    /**
     * @brief Give item to beggar
     * 
     * @param charId The character ID
     * @param beggarId The beggar ID
     * @param itemId The item ID
     * @param amount The amount
     * @return std::string The response
     */
    std::string GiveItemToBeggar(int charId, int beggarId, int itemId, int amount);
    
    /**
     * @brief Give zeny to beggar
     * 
     * @param charId The character ID
     * @param beggarId The beggar ID
     * @param amount The amount
     * @return std::string The response
     */
    std::string GiveZenyToBeggar(int charId, int beggarId, int amount);
    
    /**
     * @brief Move beggar
     * 
     * @param beggarId The beggar ID
     * @param mapId The map ID
     * @param x The x coordinate
     * @param y The y coordinate
     * @return bool True if successful, false otherwise
     */
    bool MoveBeggar(int beggarId, int mapId, int x, int y);
    
    /**
     * @brief Update beggar
     * 
     * @param beggarId The beggar ID
     * @param data The data to update
     * @return bool True if successful, false otherwise
     */
    bool UpdateBeggar(int beggarId, const std::map<std::string, std::string>& data);
};

} // namespace ai
} // namespace rathena

#endif // AI_BEGGAR_AGENT_HPP