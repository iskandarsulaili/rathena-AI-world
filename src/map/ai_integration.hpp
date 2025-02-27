#ifndef AI_INTEGRATION_HPP
#define AI_INTEGRATION_HPP

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <optional>

#include "../ai/ai_module.hpp"
#include "../ai/common/ai_types.hpp"

// Forward declarations for rAthena map server types
struct map_session_data;
struct mob_data;
struct npc_data;
struct s_skill_unit_group;
struct status_change;

namespace rathena {
namespace map {

/**
 * @brief AI Integration
 * 
 * This class provides integration between the AI module and the map server.
 * It allows the map server to use AI functionality for various game mechanics.
 */
class AIIntegration {
private:
    // AI module
    std::shared_ptr<ai::AIModule> aiModule_;
    
    // Configuration
    std::map<std::string, std::string> config_;
    
    // Initialization flag
    bool initialized_;
    
    // Enabled flag
    bool enabled_;
    
    // Cached agent references for quick access
    std::shared_ptr<ai::AIAgent> npcIntelligenceAgent_;
    std::shared_ptr<ai::AIAgent> worldEvolutionAgent_;
    std::shared_ptr<ai::AIAgent> legendBloodlinesAgent_;
    std::shared_ptr<ai::AIAgent> crossClassSynthesisAgent_;
    std::shared_ptr<ai::AIAgent> mayorAgent_;
    std::shared_ptr<ai::AIAgent> beggarAgent_;
    
    // Private methods
    bool LoadConfig();
    bool InitializeAgents();
    ai::AICharacter ConvertToAICharacter(const map_session_data* sd);
    ai::AINPC ConvertToAINPC(const npc_data* nd);
    ai::AIMob ConvertToAIMob(const mob_data* md);
    
public:
    // Constructor and destructor
    AIIntegration();
    ~AIIntegration();
    
    // Initialization and finalization
    bool Initialize();
    void Finalize();
    
    // Status methods
    bool IsInitialized() const;
    bool IsEnabled() const;
    void SetEnabled(bool enabled);
    
    // NPC Intelligence methods
    std::string ProcessNPCChat(int npcId, int charId, const std::string& message);
    std::string GenerateNPCResponse(int npcId, int charId, const std::string& message);
    std::string GenerateNPCDialogue(int npcId, const std::string& context);
    std::vector<std::string> GenerateNPCOptions(int npcId, const std::string& context);
    
    // World Evolution methods
    std::string GenerateWorldEvent(const std::string& location, const std::string& context);
    std::string GenerateWeatherDescription(int mapId, int weatherType);
    std::string GenerateTimeDescription(int mapId, int hour);
    std::string GenerateLocationDescription(int mapId, int x, int y);
    
    // Legend Bloodlines methods
    bool UnlockBloodline(int charId, int bloodlineId);
    bool AddBloodlineExperience(int charId, int bloodlineId, int experience);
    std::string GetBloodlineStory(int charId, int bloodlineId);
    bool LearnBloodlineSkill(int charId, int bloodlineId, int skillId);
    bool UseBloodlineSkill(int charId, int bloodlineId, int skillId, int targetId);
    std::string TalkToBloodlineSpirit(int charId, int bloodlineId, const std::string& message);
    
    // Cross Class Synthesis methods
    std::vector<int> GetCompatibleClasses(int jobId);
    std::vector<int> GetSynthesisSkills(int jobId1, int jobId2);
    bool LearnSynthesisSkill(int charId, int skillId);
    bool UseSynthesisSkill(int charId, int skillId, int targetId);
    
    // Mayor Agent methods
    std::string GetMayorDecision(int townId, const std::string& issue);
    std::string GetMayorSpeech(int townId, const std::string& occasion);
    std::vector<std::string> GetMayorPolicies(int townId);
    
    // Beggar Agent methods
    std::string GetBeggarDialogue(int beggarId, int charId);
    std::string GetBeggarStory(int beggarId);
    std::string GetBeggarRequest(int beggarId);
    std::string GetBeggarReward(int beggarId, int charId);
    
    // Event hooks
    void OnCharacterLogin(int charId);
    void OnCharacterLogout(int charId);
    void OnCharacterLevelUp(int charId, int level);
    void OnCharacterJobChange(int charId, int jobId);
    void OnCharacterDeath(int charId, int killerId);
    void OnNPCInteraction(int charId, int npcId);
    void OnMobSpawn(int mobId, int mapId, int x, int y);
    void OnMobDeath(int mobId, int killerId);
    void OnSkillUse(int charId, int skillId, int targetId);
    void OnItemUse(int charId, int itemId, int targetId);
    void OnMapChange(int charId, int mapId);
    void OnQuestStart(int charId, int questId);
    void OnQuestComplete(int charId, int questId);
    void OnChatMessage(int charId, const std::string& message, int channelId);
    void OnPartyFormation(int partyId, int leaderId);
    void OnGuildFormation(int guildId, int leaderId);
    
    // Utility methods
    std::string GetAIStatus();
    std::string GetAgentStatus(const std::string& agentName);
    std::string GetProviderStatus(const std::string& providerName);
    void ReloadConfig();
    void ReloadAgents();
    
    // Singleton instance
    static AIIntegration& GetInstance();
};

} // namespace map
} // namespace rathena

#endif // AI_INTEGRATION_HPP