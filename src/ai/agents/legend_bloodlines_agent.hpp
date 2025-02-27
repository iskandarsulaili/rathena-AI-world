#ifndef LEGEND_BLOODLINES_AGENT_HPP
#define LEGEND_BLOODLINES_AGENT_HPP

#include <string>
#include <map>
#include <vector>
#include <mutex>
#include "../common/ai_agent.hpp"
#include "../common/ai_types.hpp"

namespace rathena {
namespace ai {

/**
 * @brief Legend Bloodlines Agent
 * 
 * This agent is responsible for managing AI Legend bloodlines,
 * including character progression, skills, and interactions.
 */
class LegendBloodlinesAgent : public AIAgentBase {
private:
    // Bloodline data
    struct BloodlineData {
        int id;
        std::string name;
        std::string description;
        std::vector<int> skills;
        std::map<std::string, std::string> attributes;
    };
    
    // Character bloodline data
    struct CharacterBloodlineData {
        int characterId;
        int bloodlineId;
        int level;
        int experience;
        Timestamp unlockDate;
        std::map<int, int> skillLevels; // skill_id -> level
    };
    
    // Bloodlines
    std::map<int, BloodlineData> bloodlines_;
    std::mutex bloodlinesMutex_;
    
    // Character bloodlines
    std::map<int, std::map<int, CharacterBloodlineData>> characterBloodlines_; // char_id -> bloodline_id -> data
    std::mutex characterBloodlinesMutex_;
    
    // System prompt
    std::string systemPrompt_;
    
public:
    /**
     * @brief Constructor
     * 
     * @param name The agent's name
     * @param provider The agent's provider
     * @param memory The agent's memory
     * @param config The agent's configuration
     */
    LegendBloodlinesAgent(
        const std::string& name,
        std::shared_ptr<AIProvider> provider,
        std::shared_ptr<AIMemory> memory,
        const ConfigMap& config);
    
    /**
     * @brief Destructor
     */
    ~LegendBloodlinesAgent() override = default;
    
    /**
     * @brief Initialize the agent
     * 
     * @return true if initialization was successful, false otherwise
     */
    bool Initialize() override;
    
    /**
     * @brief Process an event
     * 
     * @param event The AI event to process
     * @return true if the event was processed successfully, false otherwise
     */
    bool ProcessEvent(const AIEvent& event) override;
    
private:
    /**
     * @brief Load bloodlines from the database
     * 
     * @return true if bloodlines were loaded successfully, false otherwise
     */
    bool LoadBloodlines();
    
    /**
     * @brief Load character bloodlines from the database
     * 
     * @param characterId The character ID
     * @return true if character bloodlines were loaded successfully, false otherwise
     */
    bool LoadCharacterBloodlines(int characterId);
    
    /**
     * @brief Save character bloodline to the database
     * 
     * @param characterId The character ID
     * @param bloodlineId The bloodline ID
     * @return true if the character bloodline was saved successfully, false otherwise
     */
    bool SaveCharacterBloodline(int characterId, int bloodlineId);
    
    /**
     * @brief Handle a character interaction event
     * 
     * @param event The AI event
     * @return true if the event was handled successfully, false otherwise
     */
    bool HandleCharacterInteraction(const AIEvent& event);
    
    /**
     * @brief Handle a bloodline unlock event
     * 
     * @param event The AI event
     * @return true if the event was handled successfully, false otherwise
     */
    bool HandleBloodlineUnlock(const AIEvent& event);
    
    /**
     * @brief Handle a bloodline experience event
     * 
     * @param event The AI event
     * @return true if the event was handled successfully, false otherwise
     */
    bool HandleBloodlineExperience(const AIEvent& event);
    
    /**
     * @brief Handle a bloodline skill event
     * 
     * @param event The AI event
     * @return true if the event was handled successfully, false otherwise
     */
    bool HandleBloodlineSkill(const AIEvent& event);
    
    /**
     * @brief Generate a bloodline story
     * 
     * @param characterId The character ID
     * @param bloodlineId The bloodline ID
     * @return std::string The generated story
     */
    std::string GenerateBloodlineStory(int characterId, int bloodlineId);
    
    /**
     * @brief Generate a bloodline skill description
     * 
     * @param characterId The character ID
     * @param bloodlineId The bloodline ID
     * @param skillId The skill ID
     * @return std::string The generated skill description
     */
    std::string GenerateBloodlineSkillDescription(int characterId, int bloodlineId, int skillId);
    
    /**
     * @brief Generate a bloodline level-up message
     * 
     * @param characterId The character ID
     * @param bloodlineId The bloodline ID
     * @param newLevel The new level
     * @return std::string The generated level-up message
     */
    std::string GenerateBloodlineLevelUpMessage(int characterId, int bloodlineId, int newLevel);
    
    /**
     * @brief Get a character's bloodline data
     * 
     * @param characterId The character ID
     * @param bloodlineId The bloodline ID
     * @return std::optional<CharacterBloodlineData> The character bloodline data, or std::nullopt if not found
     */
    std::optional<CharacterBloodlineData> GetCharacterBloodline(int characterId, int bloodlineId);
    
    /**
     * @brief Get a bloodline's data
     * 
     * @param bloodlineId The bloodline ID
     * @return std::optional<BloodlineData> The bloodline data, or std::nullopt if not found
     */
    std::optional<BloodlineData> GetBloodline(int bloodlineId);
    
    /**
     * @brief Add experience to a character's bloodline
     * 
     * @param characterId The character ID
     * @param bloodlineId The bloodline ID
     * @param experience The experience to add
     * @return int The new level, or -1 if the bloodline was not found
     */
    int AddBloodlineExperience(int characterId, int bloodlineId, int experience);
    
    /**
     * @brief Unlock a bloodline for a character
     * 
     * @param characterId The character ID
     * @param bloodlineId The bloodline ID
     * @return true if the bloodline was unlocked successfully, false otherwise
     */
    bool UnlockBloodline(int characterId, int bloodlineId);
    
    /**
     * @brief Learn a bloodline skill for a character
     * 
     * @param characterId The character ID
     * @param bloodlineId The bloodline ID
     * @param skillId The skill ID
     * @param level The skill level
     * @return true if the skill was learned successfully, false otherwise
     */
    bool LearnBloodlineSkill(int characterId, int bloodlineId, int skillId, int level);
    
    /**
     * @brief Calculate the experience required for a bloodline level
     * 
     * @param level The level
     * @return int The experience required
     */
    int CalculateBloodlineLevelExperience(int level);
    
    /**
     * @brief Build a conversation history for a character and bloodline
     * 
     * @param characterId The character ID
     * @param bloodlineId The bloodline ID
     * @return std::vector<std::pair<std::string, std::string>> The conversation history
     */
    std::vector<std::pair<std::string, std::string>> BuildConversationHistory(int characterId, int bloodlineId);
};

} // namespace ai
} // namespace rathena

#endif // LEGEND_BLOODLINES_AGENT_HPP