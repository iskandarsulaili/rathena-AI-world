#ifndef AI_REQUEST_HPP
#define AI_REQUEST_HPP

#include <string>
#include <vector>
#include <map>
#include "ai_types.hpp"

namespace rathena {
namespace ai {

/**
 * @brief AI request builder
 * 
 * This class is used to build AI requests.
 */
class AIRequestBuilder {
private:
    AIRequest request_;
    
public:
    /**
     * @brief Constructor
     */
    AIRequestBuilder();
    
    /**
     * @brief Set the prompt
     * 
     * @param prompt The prompt
     * @return AIRequestBuilder& The builder
     */
    AIRequestBuilder& WithPrompt(const std::string& prompt);
    
    /**
     * @brief Set the model
     * 
     * @param model The model
     * @return AIRequestBuilder& The builder
     */
    AIRequestBuilder& WithModel(const std::string& model);
    
    /**
     * @brief Set the maximum number of tokens
     * 
     * @param maxTokens The maximum number of tokens
     * @return AIRequestBuilder& The builder
     */
    AIRequestBuilder& WithMaxTokens(int maxTokens);
    
    /**
     * @brief Set the temperature
     * 
     * @param temperature The temperature
     * @return AIRequestBuilder& The builder
     */
    AIRequestBuilder& WithTemperature(float temperature);
    
    /**
     * @brief Set the top P
     * 
     * @param topP The top P
     * @return AIRequestBuilder& The builder
     */
    AIRequestBuilder& WithTopP(float topP);
    
    /**
     * @brief Set the frequency penalty
     * 
     * @param frequencyPenalty The frequency penalty
     * @return AIRequestBuilder& The builder
     */
    AIRequestBuilder& WithFrequencyPenalty(float frequencyPenalty);
    
    /**
     * @brief Set the presence penalty
     * 
     * @param presencePenalty The presence penalty
     * @return AIRequestBuilder& The builder
     */
    AIRequestBuilder& WithPresencePenalty(float presencePenalty);
    
    /**
     * @brief Add a conversation history entry
     * 
     * @param role The role (e.g., "user", "assistant")
     * @param content The content
     * @return AIRequestBuilder& The builder
     */
    AIRequestBuilder& WithConversationHistoryEntry(const std::string& role, const std::string& content);
    
    /**
     * @brief Set the conversation history
     * 
     * @param conversationHistory The conversation history
     * @return AIRequestBuilder& The builder
     */
    AIRequestBuilder& WithConversationHistory(const std::vector<std::pair<std::string, std::string>>& conversationHistory);
    
    /**
     * @brief Add metadata
     * 
     * @param key The metadata key
     * @param value The metadata value
     * @return AIRequestBuilder& The builder
     */
    AIRequestBuilder& WithMetadata(const std::string& key, const std::string& value);
    
    /**
     * @brief Set the metadata
     * 
     * @param metadata The metadata
     * @return AIRequestBuilder& The builder
     */
    AIRequestBuilder& WithMetadata(const std::map<std::string, std::string>& metadata);
    
    /**
     * @brief Build the request
     * 
     * @return AIRequest The built request
     */
    AIRequest Build() const;
};

/**
 * @brief Create a chat request
 * 
 * @param character The character
 * @param message The message
 * @param conversationHistory The conversation history
 * @return AIRequest The chat request
 */
AIRequest CreateChatRequest(const AICharacter& character, const std::string& message, const std::vector<std::pair<std::string, std::string>>& conversationHistory);

/**
 * @brief Create an NPC interaction request
 * 
 * @param character The character
 * @param npc The NPC
 * @param message The message
 * @param conversationHistory The conversation history
 * @return AIRequest The NPC interaction request
 */
AIRequest CreateNPCInteractionRequest(const AICharacter& character, const AINPC& npc, const std::string& message, const std::vector<std::pair<std::string, std::string>>& conversationHistory);

/**
 * @brief Create a mob interaction request
 * 
 * @param character The character
 * @param mob The mob
 * @param message The message
 * @param conversationHistory The conversation history
 * @return AIRequest The mob interaction request
 */
AIRequest CreateMobInteractionRequest(const AICharacter& character, const AIMob& mob, const std::string& message, const std::vector<std::pair<std::string, std::string>>& conversationHistory);

/**
 * @brief Create a quest request
 * 
 * @param character The character
 * @param quest The quest
 * @param eventType The event type
 * @return AIRequest The quest request
 */
AIRequest CreateQuestRequest(const AICharacter& character, const AIQuest& quest, const std::string& eventType);

/**
 * @brief Create a bloodline request
 * 
 * @param character The character
 * @param bloodlineId The bloodline ID
 * @param eventType The event type
 * @param metadata Additional metadata
 * @return AIRequest The bloodline request
 */
AIRequest CreateBloodlineRequest(const AICharacter& character, int bloodlineId, const std::string& eventType, const std::map<std::string, std::string>& metadata);

/**
 * @brief Create a world event request
 * 
 * @param eventType The event type
 * @param metadata Additional metadata
 * @return AIRequest The world event request
 */
AIRequest CreateWorldEventRequest(const std::string& eventType, const std::map<std::string, std::string>& metadata);

} // namespace ai
} // namespace rathena

#endif // AI_REQUEST_HPP