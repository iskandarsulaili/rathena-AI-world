#ifndef AI_TYPES_HPP
#define AI_TYPES_HPP

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <optional>

namespace rathena {
namespace ai {

// Type aliases
using ConfigMap = std::map<std::string, std::string>;
using Timestamp = std::chrono::system_clock::time_point;

/**
 * @brief AI provider status
 */
enum class AIProviderStatus {
    UNINITIALIZED = 0,
    INITIALIZING = 1,
    READY = 2,
    ERROR = 3,
    DISABLED = 4
};

/**
 * @brief AI agent type
 */
enum class AIAgentType {
    UNKNOWN = 0,
    NPC = 1,
    MOB = 2,
    WORLD = 3,
    QUEST = 4,
    BLOODLINE = 5,
    MAYOR = 6,
    BEGGAR = 7,
    CROSS_CLASS = 8
};

/**
 * @brief AI agent status
 */
enum class AIAgentStatus {
    UNINITIALIZED = 0,
    INITIALIZING = 1,
    READY = 2,
    ERROR = 3,
    DISABLED = 4
};

/**
 * @brief AI event type
 */
enum class AIEventType {
    UNKNOWN = 0,
    CHAT = 1,
    NPC_INTERACTION = 2,
    MOB_INTERACTION = 3,
    QUEST = 4,
    SKILL_USE = 5,
    BLOODLINE = 6,
    WORLD = 7,
    CUSTOM = 100
};

/**
 * @brief AI response type
 */
enum class AIResponseType {
    UNKNOWN = 0,
    TEXT = 1,
    ACTION = 2,
    SKILL = 3,
    QUEST = 4,
    BLOODLINE = 5,
    WORLD = 6,
    CUSTOM = 100
};

/**
 * @brief AI capabilities
 */
struct AICapabilities {
    bool textGeneration;
    bool imageGeneration;
    bool audioGeneration;
    bool textToSpeech;
    bool speechToText;
    bool translation;
    bool embedding;
    int maxTokens;
    std::vector<std::string> supportedModels;
    std::map<std::string, std::string> additionalCapabilities;
};

/**
 * @brief AI character data
 */
struct AICharacter {
    int id;
    std::string name;
    int level;
    int job;
    int gender;
    std::map<std::string, int> stats;
    std::map<int, int> skills;
    std::map<std::string, std::string> attributes;
};

/**
 * @brief AI NPC data
 */
struct AINPC {
    int id;
    std::string name;
    std::string displayName;
    std::string sprite;
    int mapId;
    int x;
    int y;
    AIAgentType agentType;
    std::map<std::string, std::string> attributes;
};

/**
 * @brief AI mob data
 */
struct AIMob {
    int id;
    int mobId;
    std::string name;
    int level;
    int hp;
    int mapId;
    int x;
    int y;
    AIAgentType agentType;
    std::map<std::string, std::string> attributes;
};

/**
 * @brief AI skill data
 */
struct AISkill {
    int id;
    std::string name;
    int level;
    std::string description;
    std::map<std::string, std::string> attributes;
};

/**
 * @brief AI quest data
 */
struct AIQuest {
    int id;
    std::string name;
    std::string description;
    int minLevel;
    std::vector<int> requiredQuests;
    std::map<std::string, std::string> attributes;
};

/**
 * @brief AI memory data
 */
struct AIMemoryData {
    std::string key;
    std::string data;
    Timestamp timestamp;
    std::map<std::string, std::string> metadata;
};

/**
 * @brief AI memory query
 */
struct AIMemoryQuery {
    std::string keyPrefix;
    std::optional<Timestamp> startTime;
    std::optional<Timestamp> endTime;
    std::map<std::string, std::string> metadata;
    size_t limit;
};

/**
 * @brief AI request
 */
struct AIRequest {
    std::string prompt;
    std::string model;
    int maxTokens;
    float temperature;
    float topP;
    float frequencyPenalty;
    float presencePenalty;
    std::vector<std::pair<std::string, std::string>> conversationHistory;
    std::map<std::string, std::string> metadata;
};

/**
 * @brief AI response
 */
struct AIResponse {
    AIResponseType type;
    std::string text;
    std::map<std::string, std::string> actions;
    std::map<std::string, std::string> metadata;
    bool success;
    std::string errorMessage;
};

/**
 * @brief AI event
 */
struct AIEvent {
    AIEventType type;
    std::string eventId;
    Timestamp timestamp;
    
    // Character data
    std::optional<AICharacter> character;
    
    // NPC data
    std::optional<AINPC> npc;
    
    // Mob data
    std::optional<AIMob> mob;
    
    // Chat data
    std::optional<std::string> message;
    std::optional<int> targetId;
    
    // Skill data
    std::optional<AISkill> skill;
    
    // Quest data
    std::optional<AIQuest> quest;
    std::optional<std::string> questEventType;
    
    // Bloodline data
    std::optional<int> bloodlineId;
    std::optional<std::string> bloodlineEventType;
    
    // World data
    std::optional<std::string> worldEventType;
    
    // Additional data
    std::map<std::string, std::string> metadata;
};

// Convert an AI provider status to a string
std::string ai_provider_status_to_string(AIProviderStatus status);

// Convert a string to an AI provider status
AIProviderStatus string_to_ai_provider_status(const std::string& str);

// Convert an AI agent type to a string
std::string ai_agent_type_to_string(AIAgentType type);

// Convert a string to an AI agent type
AIAgentType string_to_ai_agent_type(const std::string& str);

// Convert an AI agent status to a string
std::string ai_agent_status_to_string(AIAgentStatus status);

// Convert a string to an AI agent status
AIAgentStatus string_to_ai_agent_status(const std::string& str);

// Convert an AI event type to a string
std::string ai_event_type_to_string(AIEventType type);

// Convert a string to an AI event type
AIEventType string_to_ai_event_type(const std::string& str);

// Convert an AI response type to a string
std::string ai_response_type_to_string(AIResponseType type);

// Convert a string to an AI response type
AIResponseType string_to_ai_response_type(const std::string& str);

} // namespace ai
} // namespace rathena

#endif // AI_TYPES_HPP