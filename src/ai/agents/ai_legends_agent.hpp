#ifndef AI_LEGENDS_AGENT_HPP
#define AI_LEGENDS_AGENT_HPP

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
 * @brief AI Legends Agent
 * 
 * This agent is responsible for managing the AI Legends system, which provides
 * bloodline-related functionality to characters. This includes generating stories,
 * managing skills, and processing conversations with bloodline spirits.
 */
class AILegendsAgent : public AIAgent {
private:
    // Configuration
    ConfigMap config_;
    
    // Memory manager
    std::shared_ptr<AIMemory> memory_;
    
    // Cache for bloodline data
    std::map<int, std::map<std::string, std::string>> bloodlineCache_;
    std::mutex bloodlineCacheMutex_;
    
    // Cache for bloodline skills
    std::map<int, std::vector<std::map<std::string, std::string>>> skillCache_;
    std::mutex skillCacheMutex_;
    
    // Cache for character bloodlines
    std::map<int, std::vector<std::map<std::string, std::string>>> characterBloodlineCache_;
    std::mutex characterBloodlineCacheMutex_;
    
    // Cache for generated stories
    std::map<std::pair<int, int>, std::string> storyCache_;
    std::mutex storyCacheMutex_;
    
    // Cache for conversation history
    std::map<std::pair<int, int>, std::vector<std::pair<std::string, std::string>>> conversationCache_;
    std::mutex conversationCacheMutex_;
    
    /**
     * @brief Load bloodline data from the database
     * 
     * @return bool True if successful, false otherwise
     */
    bool LoadBloodlineData();
    
    /**
     * @brief Load bloodline skills from the database
     * 
     * @return bool True if successful, false otherwise
     */
    bool LoadBloodlineSkills();
    
    /**
     * @brief Generate a bloodline story
     * 
     * @param charId The character ID
     * @param bloodlineId The bloodline ID
     * @param characterData The character data
     * @param bloodlineData The bloodline data
     * @return std::string The generated story
     */
    std::string GenerateBloodlineStory(int charId, int bloodlineId, 
                                      const std::map<std::string, std::string>& characterData,
                                      const std::map<std::string, std::string>& bloodlineData);
    
    /**
     * @brief Generate a skill description
     * 
     * @param charId The character ID
     * @param bloodlineId The bloodline ID
     * @param skillId The skill ID
     * @param skillLevel The skill level
     * @param characterData The character data
     * @param bloodlineData The bloodline data
     * @param skillData The skill data
     * @return std::string The generated skill description
     */
    std::string GenerateSkillDescription(int charId, int bloodlineId, int skillId, int skillLevel,
                                        const std::map<std::string, std::string>& characterData,
                                        const std::map<std::string, std::string>& bloodlineData,
                                        const std::map<std::string, std::string>& skillData);
    
    /**
     * @brief Generate a level-up message
     * 
     * @param charId The character ID
     * @param bloodlineId The bloodline ID
     * @param newLevel The new bloodline level
     * @param characterData The character data
     * @param bloodlineData The bloodline data
     * @return std::string The generated level-up message
     */
    std::string GenerateLevelUpMessage(int charId, int bloodlineId, int newLevel,
                                      const std::map<std::string, std::string>& characterData,
                                      const std::map<std::string, std::string>& bloodlineData);
    
    /**
     * @brief Process a conversation with a bloodline spirit
     * 
     * @param charId The character ID
     * @param bloodlineId The bloodline ID
     * @param message The message from the character
     * @param characterData The character data
     * @param bloodlineData The bloodline data
     * @return std::string The response from the bloodline spirit
     */
    std::string ProcessConversation(int charId, int bloodlineId, const std::string& message,
                                   const std::map<std::string, std::string>& characterData,
                                   const std::map<std::string, std::string>& bloodlineData);
    
    /**
     * @brief Get the conversation history for a character and bloodline
     * 
     * @param charId The character ID
     * @param bloodlineId The bloodline ID
     * @return std::vector<std::pair<std::string, std::string>> The conversation history
     */
    std::vector<std::pair<std::string, std::string>> GetConversationHistory(int charId, int bloodlineId);
    
    /**
     * @brief Add a conversation entry to the history
     * 
     * @param charId The character ID
     * @param bloodlineId The bloodline ID
     * @param message The message from the character
     * @param response The response from the bloodline spirit
     */
    void AddConversationEntry(int charId, int bloodlineId, const std::string& message, const std::string& response);
    
    /**
     * @brief Check if a character meets the requirements to unlock a bloodline
     * 
     * @param charId The character ID
     * @param bloodlineId The bloodline ID
     * @param characterData The character data
     * @param bloodlineData The bloodline data
     * @return bool True if the character meets the requirements, false otherwise
     */
    bool CheckUnlockRequirements(int charId, int bloodlineId,
                               const std::map<std::string, std::string>& characterData,
                               const std::map<std::string, std::string>& bloodlineData);
    
    /**
     * @brief Calculate the experience required for a bloodline level
     * 
     * @param level The bloodline level
     * @return int The experience required
     */
    int CalculateRequiredExperience(int level);
    
    /**
     * @brief Calculate the experience gained from an action
     * 
     * @param actionType The action type
     * @param actionLevel The action level
     * @param bloodlineLevel The bloodline level
     * @return int The experience gained
     */
    int CalculateExperienceGain(const std::string& actionType, int actionLevel, int bloodlineLevel);
    
    /**
     * @brief Check if a bloodline can learn a skill
     * 
     * @param charId The character ID
     * @param bloodlineId The bloodline ID
     * @param skillId The skill ID
     * @param characterData The character data
     * @param bloodlineData The bloodline data
     * @param skillData The skill data
     * @return bool True if the bloodline can learn the skill, false otherwise
     */
    bool CanLearnSkill(int charId, int bloodlineId, int skillId,
                      const std::map<std::string, std::string>& characterData,
                      const std::map<std::string, std::string>& bloodlineData,
                      const std::map<std::string, std::string>& skillData);
    
    /**
     * @brief Get the cooldown for a skill
     * 
     * @param skillId The skill ID
     * @param skillLevel The skill level
     * @param skillData The skill data
     * @return int The cooldown in seconds
     */
    int GetSkillCooldown(int skillId, int skillLevel, const std::map<std::string, std::string>& skillData);
    
    /**
     * @brief Check if a skill is on cooldown
     * 
     * @param charId The character ID
     * @param bloodlineId The bloodline ID
     * @param skillId The skill ID
     * @return bool True if the skill is on cooldown, false otherwise
     */
    bool IsSkillOnCooldown(int charId, int bloodlineId, int skillId);
    
    /**
     * @brief Set a skill on cooldown
     * 
     * @param charId The character ID
     * @param bloodlineId The bloodline ID
     * @param skillId The skill ID
     * @param cooldown The cooldown in seconds
     */
    void SetSkillCooldown(int charId, int bloodlineId, int skillId, int cooldown);
    
    /**
     * @brief Get character data from the database
     * 
     * @param charId The character ID
     * @return std::map<std::string, std::string> The character data
     */
    std::map<std::string, std::string> GetCharacterData(int charId);
    
    /**
     * @brief Get bloodline data from the database
     * 
     * @param bloodlineId The bloodline ID
     * @return std::map<std::string, std::string> The bloodline data
     */
    std::map<std::string, std::string> GetBloodlineData(int bloodlineId);
    
    /**
     * @brief Get character bloodline data from the database
     * 
     * @param charId The character ID
     * @param bloodlineId The bloodline ID
     * @return std::map<std::string, std::string> The character bloodline data
     */
    std::map<std::string, std::string> GetCharacterBloodlineData(int charId, int bloodlineId);
    
    /**
     * @brief Get skill data from the database
     * 
     * @param skillId The skill ID
     * @return std::map<std::string, std::string> The skill data
     */
    std::map<std::string, std::string> GetSkillData(int skillId);
    
    /**
     * @brief Get character skill data from the database
     * 
     * @param charId The character ID
     * @param bloodlineId The bloodline ID
     * @param skillId The skill ID
     * @return std::map<std::string, std::string> The character skill data
     */
    std::map<std::string, std::string> GetCharacterSkillData(int charId, int bloodlineId, int skillId);
    
public:
    /**
     * @brief Constructor
     * 
     * @param config The configuration
     * @param memory The memory manager
     */
    AILegendsAgent(const ConfigMap& config, std::shared_ptr<AIMemory> memory);
    
    /**
     * @brief Destructor
     */
    virtual ~AILegendsAgent();
    
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
     * @brief List available bloodlines
     * 
     * @return std::vector<std::map<std::string, std::string>> The bloodlines
     */
    std::vector<std::map<std::string, std::string>> ListBloodlines();
    
    /**
     * @brief Get bloodline information
     * 
     * @param bloodlineId The bloodline ID
     * @return std::map<std::string, std::string> The bloodline information
     */
    std::map<std::string, std::string> GetBloodlineInfo(int bloodlineId);
    
    /**
     * @brief List character bloodlines
     * 
     * @param charId The character ID
     * @return std::vector<std::map<std::string, std::string>> The character bloodlines
     */
    std::vector<std::map<std::string, std::string>> ListCharacterBloodlines(int charId);
    
    /**
     * @brief Get character bloodline information
     * 
     * @param charId The character ID
     * @param bloodlineId The bloodline ID
     * @return std::map<std::string, std::string> The character bloodline information
     */
    std::map<std::string, std::string> GetCharacterBloodlineInfo(int charId, int bloodlineId);
    
    /**
     * @brief Unlock a bloodline for a character
     * 
     * @param charId The character ID
     * @param bloodlineId The bloodline ID
     * @return bool True if successful, false otherwise
     */
    bool UnlockBloodline(int charId, int bloodlineId);
    
    /**
     * @brief Add experience to a character's bloodline
     * 
     * @param charId The character ID
     * @param bloodlineId The bloodline ID
     * @param experience The experience to add
     * @return bool True if successful, false otherwise
     */
    bool AddBloodlineExperience(int charId, int bloodlineId, int experience);
    
    /**
     * @brief Get the bloodline story for a character
     * 
     * @param charId The character ID
     * @param bloodlineId The bloodline ID
     * @return std::string The bloodline story
     */
    std::string GetBloodlineStory(int charId, int bloodlineId);
    
    /**
     * @brief List bloodline skills
     * 
     * @param bloodlineId The bloodline ID
     * @return std::vector<std::map<std::string, std::string>> The bloodline skills
     */
    std::vector<std::map<std::string, std::string>> ListBloodlineSkills(int bloodlineId);
    
    /**
     * @brief List character bloodline skills
     * 
     * @param charId The character ID
     * @param bloodlineId The bloodline ID
     * @return std::vector<std::map<std::string, std::string>> The character bloodline skills
     */
    std::vector<std::map<std::string, std::string>> ListCharacterBloodlineSkills(int charId, int bloodlineId);
    
    /**
     * @brief Learn a bloodline skill
     * 
     * @param charId The character ID
     * @param bloodlineId The bloodline ID
     * @param skillId The skill ID
     * @return bool True if successful, false otherwise
     */
    bool LearnBloodlineSkill(int charId, int bloodlineId, int skillId);
    
    /**
     * @brief Level up a bloodline skill
     * 
     * @param charId The character ID
     * @param bloodlineId The bloodline ID
     * @param skillId The skill ID
     * @return bool True if successful, false otherwise
     */
    bool LevelUpBloodlineSkill(int charId, int bloodlineId, int skillId);
    
    /**
     * @brief Use a bloodline skill
     * 
     * @param charId The character ID
     * @param bloodlineId The bloodline ID
     * @param skillId The skill ID
     * @param targetId The target ID
     * @return bool True if successful, false otherwise
     */
    bool UseBloodlineSkill(int charId, int bloodlineId, int skillId, int targetId);
    
    /**
     * @brief Talk to a bloodline spirit
     * 
     * @param charId The character ID
     * @param bloodlineId The bloodline ID
     * @param message The message
     * @return std::string The response
     */
    std::string TalkToBloodlineSpirit(int charId, int bloodlineId, const std::string& message);
    
    /**
     * @brief Trigger a bloodline event
     * 
     * @param charId The character ID
     * @param bloodlineId The bloodline ID
     * @param eventType The event type
     * @param eventData Additional event data
     * @return std::map<std::string, std::string> The event result
     */
    std::map<std::string, std::string> TriggerBloodlineEvent(int charId, int bloodlineId, 
                                                           const std::string& eventType,
                                                           const std::map<std::string, std::string>& eventData);
};

} // namespace ai
} // namespace rathena

#endif // AI_LEGENDS_AGENT_HPP