#ifndef AI_SYSTEM_MANAGER_HPP
#define AI_SYSTEM_MANAGER_HPP

#include <memory>
#include <map>
#include <string>
#include "common/cbasetypes.hpp"
#include "common/timer.hpp"
#include "common/conf.hpp"
#include "ai/memory/MemoryManager.hpp"
#include "ai/model/AIModelManager.hpp"

// Include all AI agents
#include "ai/balance/AdaptiveBalanceAgent.hpp"
#include "ai/reward/DynamicRewardAgent.hpp"
#include "ai/ranking/CompetitiveRankingAgent.hpp"
#include "ai/engagement/PlayerEngagementAgent.hpp"
#include "ai/economy/AdaptiveEconomyAgent.hpp"
#include "ai/social/SmartPartyGuildAgent.hpp"
#include "ai/coach/PersonalizedCoachAgent.hpp"
#include "ai/emotional/EmotionalInvestmentAgent.hpp"
#include "ai/monster/DynamicMonsterAgent.hpp"
#include "ai/influence/SocialInfluenceAgent.hpp"
#include "ai/quest/AdaptiveQuestAgent.hpp"

class AISystemManager {
private:
    // Singleton instance
    static std::unique_ptr<AISystemManager> instance;

    // Core components
    std::unique_ptr<AIModelManager> model_manager;
    std::unique_ptr<MemoryManager> memory_manager;

    // AI Agents
    std::unique_ptr<AdaptiveBalanceAgent> balance_agent;
    std::unique_ptr<DynamicRewardAgent> reward_agent;
    std::unique_ptr<CompetitiveRankingAgent> ranking_agent;
    std::unique_ptr<PlayerEngagementAgent> engagement_agent;
    std::unique_ptr<AdaptiveEconomyAgent> economy_agent;
    std::unique_ptr<SmartPartyGuildAgent> party_guild_agent;
    std::unique_ptr<PersonalizedCoachAgent> coach_agent;
    std::unique_ptr<EmotionalInvestmentAgent> emotional_agent;
    std::unique_ptr<DynamicMonsterAgent> monster_agent;
    std::unique_ptr<SocialInfluenceAgent> social_agent;
    std::unique_ptr<AdaptiveQuestAgent> quest_agent;

    // Configuration
    struct Config {
        bool enabled;
        std::string primary_model;
        std::string fallback_model;
        struct {
            int short_term_capacity;
            int long_term_capacity;
            int short_term_retention;
            int long_term_retention;
            int cleanup_interval;
        } memory;
        struct {
            int max_concurrent_requests;
            int request_timeout;
            int batch_size;
            int cache_duration;
        } performance;
    } config;

    // Agent status tracking
    struct AgentStatus {
        bool enabled;
        time_t last_update;
        int update_interval;
        bool needs_update;
    };
    std::map<std::string, AgentStatus> agent_status;

    // Private constructor for singleton
    AISystemManager();

    // Internal methods
    bool loadConfig();
    bool initializeAgents();
    void updateAgentStatus();
    bool validateSystemState();

public:
    // Singleton access
    static AISystemManager& getInstance();

    // Delete copy constructor and assignment operator
    AISystemManager(const AISystemManager&) = delete;
    AISystemManager& operator=(const AISystemManager&) = delete;

    // Initialization
    bool initialize();
    void shutdown();

    // Core system methods
    void update(time_t current_time);
    bool isEnabled() const;
    bool isAgentEnabled(const std::string& agent_name) const;

    // Agent access methods
    AdaptiveBalanceAgent* getBalanceAgent() { return balance_agent.get(); }
    DynamicRewardAgent* getRewardAgent() { return reward_agent.get(); }
    CompetitiveRankingAgent* getRankingAgent() { return ranking_agent.get(); }
    PlayerEngagementAgent* getEngagementAgent() { return engagement_agent.get(); }
    AdaptiveEconomyAgent* getEconomyAgent() { return economy_agent.get(); }
    SmartPartyGuildAgent* getPartyGuildAgent() { return party_guild_agent.get(); }
    PersonalizedCoachAgent* getCoachAgent() { return coach_agent.get(); }
    EmotionalInvestmentAgent* getEmotionalAgent() { return emotional_agent.get(); }
    DynamicMonsterAgent* getMonsterAgent() { return monster_agent.get(); }
    SocialInfluenceAgent* getSocialAgent() { return social_agent.get(); }
    AdaptiveQuestAgent* getQuestAgent() { return quest_agent.get(); }

    // System status and monitoring
    struct SystemStatus {
        bool system_enabled;
        size_t active_agents;
        size_t memory_usage;
        size_t api_requests_pending;
        float system_load;
        std::map<std::string, bool> agent_status;
    };
    SystemStatus getSystemStatus() const;

    // Configuration management
    bool reloadConfig();
    bool setAgentEnabled(const std::string& agent_name, bool enabled);
    bool updateAgentConfig(const std::string& agent_name, const std::string& config_path, const std::string& value);

    // Event hooks
    void onServerTick(time_t tick);
    void onPlayerLogin(int account_id);
    void onPlayerLogout(int account_id);
    void onMapStart(int map_id);
    void onBattleStart(int battle_id);
};

#endif // AI_SYSTEM_MANAGER_HPP