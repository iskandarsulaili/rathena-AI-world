#ifndef AI_TYPES_HPP
#define AI_TYPES_HPP

#include <string>
#include <map>
#include <vector>

namespace rathena {
namespace ai {

/**
 * @brief AI agent type
 */
enum class AIAgentType {
    UNKNOWN = 0,
    WORLD_EVOLUTION = 1,
    LEGEND_BLOODLINES = 2,
    CROSS_CLASS_SYNTHESIS = 3,
    NPC_INTELLIGENCE = 4,
    MAYOR = 5,
    BEGGAR = 6
};

/**
 * @brief AI agent status
 */
enum class AIAgentStatus {
    UNKNOWN = 0,
    INITIALIZING = 1,
    ACTIVE = 2,
    PAUSED = 3,
    ERROR = 4,
    SHUTDOWN = 5
};

/**
 * @brief AI event type
 */
enum class AIEventType {
    UNKNOWN = 0,
    
    // World evolution events
    WORLD_EVOLUTION_UPDATE = 100,
    WORLD_EVOLUTION_GENERATE = 101,
    
    // Legend bloodlines events
    LEGEND_BLOODLINES_CREATE = 200,
    LEGEND_BLOODLINES_UPDATE = 201,
    LEGEND_BLOODLINES_DELETE = 202,
    LEGEND_BLOODLINES_QUERY = 203,
    
    // Cross class synthesis events
    CROSS_CLASS_SYNTHESIS_GENERATE = 300,
    CROSS_CLASS_SYNTHESIS_QUERY = 301,
    
    // NPC intelligence events
    NPC_INTELLIGENCE_TALK = 400,
    NPC_INTELLIGENCE_QUEST = 401,
    
    // Mayor events
    MAYOR_TALK = 500,
    MAYOR_QUEST = 501,
    MAYOR_POLICY = 502,
    
    // Beggar events
    BEGGAR_TALK = 600,
    BEGGAR_QUEST_ACCEPT = 601,
    BEGGAR_QUEST_COMPLETE = 602,
    BEGGAR_GIVE_ITEM = 603,
    BEGGAR_GIVE_ZENY = 604,
    BEGGAR_CREATE = 605,
    BEGGAR_DELETE = 606,
    BEGGAR_MOVE = 607,
    BEGGAR_UPDATE = 608
};

/**
 * @brief AI event
 */
class AIEvent {
public:
    AIEventType type;
    std::map<std::string, std::string> data;
    
    AIEvent() : type(AIEventType::UNKNOWN) {}
    
    AIEvent(AIEventType type) : type(type) {}
    
    AIEvent(AIEventType type, const std::map<std::string, std::string>& data)
        : type(type), data(data) {}
};

/**
 * @brief AI response
 */
class AIResponse {
public:
    bool success;
    std::map<std::string, std::string> data;
    
    AIResponse() : success(false) {}
    
    AIResponse(bool success) : success(success) {}
    
    AIResponse(bool success, const std::map<std::string, std::string>& data)
        : success(success), data(data) {}
};

/**
 * @brief AI provider type
 */
enum class AIProviderType {
    UNKNOWN = 0,
    AZURE_OPENAI = 1,
    OPENAI = 2,
    DEEPSEEK = 3,
    LOCAL = 4
};

/**
 * @brief AI provider status
 */
enum class AIProviderStatus {
    UNKNOWN = 0,
    INITIALIZING = 1,
    ACTIVE = 2,
    PAUSED = 3,
    ERROR = 4,
    SHUTDOWN = 5
};

/**
 * @brief AI model type
 */
enum class AIModelType {
    UNKNOWN = 0,
    GPT_3_5_TURBO = 1,
    GPT_4 = 2,
    GPT_4_TURBO = 3,
    DEEPSEEK_CODER = 4,
    CLAUDE_3_OPUS = 5,
    CLAUDE_3_SONNET = 6,
    CLAUDE_3_HAIKU = 7,
    LLAMA_3 = 8
};

/**
 * @brief AI memory type
 */
enum class AIMemoryType {
    UNKNOWN = 0,
    SHORT_TERM = 1,
    LONG_TERM = 2,
    EPISODIC = 3,
    SEMANTIC = 4
};

/**
 * @brief AI memory entry
 */
struct AIMemoryEntry {
    std::string id;
    std::string content;
    std::string timestamp;
    AIMemoryType type;
    std::map<std::string, std::string> metadata;
};

/**
 * @brief Configuration map
 */
typedef std::map<std::string, std::string> ConfigMap;

} // namespace ai
} // namespace rathena

#endif // AI_TYPES_HPP