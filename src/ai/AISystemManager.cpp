#include "ai/AISystemManager.hpp"
#include "common/showmsg.hpp"
#include "common/timer.hpp"
#include "common/utilities.hpp"

std::unique_ptr<AISystemManager> AISystemManager::instance;

AISystemManager& AISystemManager::getInstance() {
    if (!instance) {
        instance = std::unique_ptr<AISystemManager>(new AISystemManager());
    }
    return *instance;
}

AISystemManager::AISystemManager() {
    // Initialize core components with nullptr
    model_manager = nullptr;
    memory_manager = nullptr;

    // Initialize all agents with nullptr
    balance_agent = nullptr;
    reward_agent = nullptr;
    ranking_agent = nullptr;
    engagement_agent = nullptr;
    economy_agent = nullptr;
    party_guild_agent = nullptr;
    coach_agent = nullptr;
    emotional_agent = nullptr;
    monster_agent = nullptr;
    social_agent = nullptr;
    quest_agent = nullptr;
}

bool AISystemManager::loadConfig() {
    // Load main AI configuration file
    if (!libconfig->read_file("conf/ai_agents.conf")) {
        ShowError("Failed to load AI agents configuration file.\n");
        return false;
    }

    // Load global AI system settings
    config.enabled = libconfig->get_bool("ai_system/enabled", false);
    if (!config.enabled) {
        ShowInfo("AI System is disabled in configuration.\n");
        return true;
    }

    // Load AI model settings
    config.primary_model = libconfig->get_string("ai_system/primary_model", "azure_openai");
    config.fallback_model = libconfig->get_string("ai_system/fallback_model", "deepseek_v3");

    // Load memory settings
    config.memory.short_term_capacity = libconfig->get_int("ai_system/memory/short_term_capacity", 1000);
    config.memory.long_term_capacity = libconfig->get_int("ai_system/memory/long_term_capacity", 10000);
    config.memory.short_term_retention = libconfig->get_int("ai_system/memory/short_term_retention", 86400);
    config.memory.long_term_retention = libconfig->get_int("ai_system/memory/long_term_retention", 7776000);
    config.memory.cleanup_interval = libconfig->get_int("ai_system/memory/cleanup_interval", 3600);

    // Load performance settings
    config.performance.max_concurrent_requests = libconfig->get_int("ai_system/performance/max_concurrent_requests", 100);
    config.performance.request_timeout = libconfig->get_int("ai_system/performance/request_timeout", 30);
    config.performance.batch_size = libconfig->get_int("ai_system/performance/batch_size", 32);
    config.performance.cache_duration = libconfig->get_int("ai_system/performance/cache_duration", 300);

    // Initialize agent status tracking
    agent_status["balance"] = {libconfig->get_bool("adaptive_balance/enabled", false), 0, 
                             libconfig->get_int("adaptive_balance/update_interval", 300), false};
    agent_status["reward"] = {libconfig->get_bool("dynamic_reward/enabled", false), 0,
                            libconfig->get_int("dynamic_reward/update_interval", 300), false};
    agent_status["ranking"] = {libconfig->get_bool("competitive_ranking/enabled", false), 0,
                             libconfig->get_int("competitive_ranking/update_interval", 300), false};
    agent_status["engagement"] = {libconfig->get_bool("player_engagement/enabled", false), 0,
                                libconfig->get_int("player_engagement/update_interval", 300), false};
    agent_status["economy"] = {libconfig->get_bool("economy/enabled", false), 0,
                             libconfig->get_int("economy/update_interval", 300), false};
    agent_status["party_guild"] = {libconfig->get_bool("party_guild/enabled", false), 0,
                                 libconfig->get_int("party_guild/update_interval", 300), false};
    agent_status["coach"] = {libconfig->get_bool("ai_coach/enabled", false), 0,
                           libconfig->get_int("ai_coach/update_interval", 300), false};
    agent_status["emotional"] = {libconfig->get_bool("emotional_investment/enabled", false), 0,
                               libconfig->get_int("emotional_investment/update_interval", 300), false};
    agent_status["monster"] = {libconfig->get_bool("dynamic_monster/enabled", false), 0,
                             libconfig->get_int("dynamic_monster/update_interval", 300), false};
    agent_status["social"] = {libconfig->get_bool("social_influence/enabled", false), 0,
                            libconfig->get_int("social_influence/update_interval", 300), false};
    agent_status["quest"] = {libconfig->get_bool("adaptive_quest/enabled", false), 0,
                           libconfig->get_int("adaptive_quest/update_interval", 300), false};

    ShowStatus("AI System configuration loaded successfully.\n");
    return true;
}

bool AISystemManager::initialize() {
    if (!loadConfig()) {
        return false;
    }

    if (!config.enabled) {
        return true;
    }

    // Initialize AI Model Manager
    model_manager = std::make_unique<AIModelManager>(config.primary_model, config.fallback_model);
    if (!model_manager->initialize()) {
        ShowError("Failed to initialize AI Model Manager.\n");
        return false;
    }

    // Initialize Memory Manager
    memory_manager = std::make_unique<MemoryManager>(
        config.memory.short_term_capacity,
        config.memory.long_term_capacity,
        config.memory.short_term_retention,
        config.memory.long_term_retention
    );
    if (!memory_manager->initialize()) {
        ShowError("Failed to initialize Memory Manager.\n");
        return false;
    }

    // Initialize enabled agents
    return initializeAgents();
}

bool AISystemManager::initializeAgents() {
    if (agent_status["balance"].enabled) {
        balance_agent = std::make_unique<AdaptiveBalanceAgent>(*model_manager, *memory_manager);
        if (!balance_agent->initialize()) {
            ShowError("Failed to initialize Adaptive Balance Agent.\n");
            return false;
        }
    }

    if (agent_status["reward"].enabled) {
        reward_agent = std::make_unique<DynamicRewardAgent>(*model_manager, *memory_manager);
        if (!reward_agent->initialize()) {
            ShowError("Failed to initialize Dynamic Reward Agent.\n");
            return false;
        }
    }

    // Initialize other agents similarly
    // [Code for initializing other agents omitted for brevity but follows same pattern]

    ShowStatus("All enabled AI agents initialized successfully.\n");
    return true;
}

void AISystemManager::update(time_t current_time) {
    if (!config.enabled) {
        return;
    }

    // Update memory system
    memory_manager->update(current_time);

    // Update each enabled agent if needed
    for (auto& [agent_name, status] : agent_status) {
        if (status.enabled && status.last_update + status.update_interval <= current_time) {
            status.needs_update = true;
        }
    }

    // Process agent updates
    if (agent_status["balance"].needs_update && balance_agent) {
        balance_agent->update(current_time);
        agent_status["balance"].last_update = current_time;
        agent_status["balance"].needs_update = false;
    }

    if (agent_status["reward"].needs_update && reward_agent) {
        reward_agent->update(current_time);
        agent_status["reward"].last_update = current_time;
        agent_status["reward"].needs_update = false;
    }

    // Update other agents similarly
    // [Code for updating other agents omitted for brevity but follows same pattern]
}

void AISystemManager::shutdown() {
    // Shut down agents in reverse order of dependency
    quest_agent.reset();
    social_agent.reset();
    monster_agent.reset();
    emotional_agent.reset();
    coach_agent.reset();
    party_guild_agent.reset();
    economy_agent.reset();
    engagement_agent.reset();
    ranking_agent.reset();
    reward_agent.reset();
    balance_agent.reset();

    // Shut down core systems last
    memory_manager.reset();
    model_manager.reset();

    ShowStatus("AI System shut down successfully.\n");
}

bool AISystemManager::isEnabled() const {
    return config.enabled;
}

bool AISystemManager::isAgentEnabled(const std::string& agent_name) const {
    auto it = agent_status.find(agent_name);
    return it != agent_status.end() && it->second.enabled;
}

AISystemManager::SystemStatus AISystemManager::getSystemStatus() const {
    SystemStatus status;
    status.system_enabled = config.enabled;
    status.active_agents = 0;
    status.memory_usage = memory_manager ? memory_manager->getCurrentUsage() : 0;
    status.api_requests_pending = model_manager ? model_manager->getPendingRequests() : 0;
    status.system_load = 0.0f;

    for (const auto& [name, agent_status] : agent_status) {
        status.agent_status[name] = agent_status.enabled;
        if (agent_status.enabled) {
            status.active_agents++;
        }
    }

    return status;
}

bool AISystemManager::reloadConfig() {
    return loadConfig() && validateSystemState();
}

bool AISystemManager::setAgentEnabled(const std::string& agent_name, bool enabled) {
    auto it = agent_status.find(agent_name);
    if (it == agent_status.end()) {
        return false;
    }

    if (it->second.enabled == enabled) {
        return true;
    }

    it->second.enabled = enabled;
    if (enabled) {
        return initializeAgents();
    } else {
        // Shutdown specific agent
        if (agent_name == "balance") balance_agent.reset();
        else if (agent_name == "reward") reward_agent.reset();
        // [Continue for other agents]
    }

    return true;
}

bool AISystemManager::validateSystemState() {
    // Validate core systems
    if (!model_manager || !memory_manager) {
        return false;
    }

    // Validate enabled agents
    for (const auto& [name, status] : agent_status) {
        if (status.enabled) {
            if (name == "balance" && !balance_agent) return false;
            else if (name == "reward" && !reward_agent) return false;
            // [Continue for other agents]
        }
    }

    return true;
}

void AISystemManager::onServerTick(time_t tick) {
    update(tick);
}

void AISystemManager::onPlayerLogin(int account_id) {
    if (!config.enabled) return;

    // Notify relevant agents of player login
    if (engagement_agent) engagement_agent->onPlayerLogin(account_id);
    if (social_agent) social_agent->onPlayerLogin(account_id);
    // [Continue for other relevant agents]
}

void AISystemManager::onPlayerLogout(int account_id) {
    if (!config.enabled) return;

    // Notify relevant agents of player logout
    if (engagement_agent) engagement_agent->onPlayerLogout(account_id);
    if (social_agent) social_agent->onPlayerLogout(account_id);
    // [Continue for other relevant agents]
}

void AISystemManager::onMapStart(int map_id) {
    if (!config.enabled) return;

    // Notify relevant agents of map start
    if (monster_agent) monster_agent->onMapStart(map_id);
    // [Continue for other relevant agents]
}

void AISystemManager::onBattleStart(int battle_id) {
    if (!config.enabled) return;

    // Notify relevant agents of battle start
    if (monster_agent) monster_agent->onBattleStart(battle_id);
    if (balance_agent) balance_agent->onBattleStart(battle_id);
    // [Continue for other relevant agents]
}