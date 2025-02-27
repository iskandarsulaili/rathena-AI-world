#ifndef AI_RESPONSE_HPP
#define AI_RESPONSE_HPP

#include <string>
#include <vector>
#include <map>
#include "ai_types.hpp"

namespace rathena {
namespace ai {

/**
 * @brief AI response builder
 * 
 * This class is used to build AI responses.
 */
class AIResponseBuilder {
private:
    AIResponse response_;
    
public:
    /**
     * @brief Constructor
     */
    AIResponseBuilder();
    
    /**
     * @brief Set the response type
     * 
     * @param type The response type
     * @return AIResponseBuilder& The builder
     */
    AIResponseBuilder& WithType(AIResponseType type);
    
    /**
     * @brief Set the response text
     * 
     * @param text The response text
     * @return AIResponseBuilder& The builder
     */
    AIResponseBuilder& WithText(const std::string& text);
    
    /**
     * @brief Add an action
     * 
     * @param key The action key
     * @param value The action value
     * @return AIResponseBuilder& The builder
     */
    AIResponseBuilder& WithAction(const std::string& key, const std::string& value);
    
    /**
     * @brief Set the actions
     * 
     * @param actions The actions
     * @return AIResponseBuilder& The builder
     */
    AIResponseBuilder& WithActions(const std::map<std::string, std::string>& actions);
    
    /**
     * @brief Add metadata
     * 
     * @param key The metadata key
     * @param value The metadata value
     * @return AIResponseBuilder& The builder
     */
    AIResponseBuilder& WithMetadata(const std::string& key, const std::string& value);
    
    /**
     * @brief Set the metadata
     * 
     * @param metadata The metadata
     * @return AIResponseBuilder& The builder
     */
    AIResponseBuilder& WithMetadata(const std::map<std::string, std::string>& metadata);
    
    /**
     * @brief Set the success flag
     * 
     * @param success The success flag
     * @return AIResponseBuilder& The builder
     */
    AIResponseBuilder& WithSuccess(bool success);
    
    /**
     * @brief Set the error message
     * 
     * @param errorMessage The error message
     * @return AIResponseBuilder& The builder
     */
    AIResponseBuilder& WithErrorMessage(const std::string& errorMessage);
    
    /**
     * @brief Build the response
     * 
     * @return AIResponse The built response
     */
    AIResponse Build() const;
};

/**
 * @brief Create a text response
 * 
 * @param text The response text
 * @return AIResponse The text response
 */
AIResponse CreateTextResponse(const std::string& text);

/**
 * @brief Create an action response
 * 
 * @param actions The actions
 * @return AIResponse The action response
 */
AIResponse CreateActionResponse(const std::map<std::string, std::string>& actions);

/**
 * @brief Create a skill response
 * 
 * @param skillId The skill ID
 * @param skillLevel The skill level
 * @param targetId The target ID
 * @return AIResponse The skill response
 */
AIResponse CreateSkillResponse(int skillId, int skillLevel, int targetId);

/**
 * @brief Create a quest response
 * 
 * @param questId The quest ID
 * @param action The quest action
 * @param metadata Additional metadata
 * @return AIResponse The quest response
 */
AIResponse CreateQuestResponse(int questId, const std::string& action, const std::map<std::string, std::string>& metadata);

/**
 * @brief Create a bloodline response
 * 
 * @param bloodlineId The bloodline ID
 * @param action The bloodline action
 * @param metadata Additional metadata
 * @return AIResponse The bloodline response
 */
AIResponse CreateBloodlineResponse(int bloodlineId, const std::string& action, const std::map<std::string, std::string>& metadata);

/**
 * @brief Create a world response
 * 
 * @param action The world action
 * @param metadata Additional metadata
 * @return AIResponse The world response
 */
AIResponse CreateWorldResponse(const std::string& action, const std::map<std::string, std::string>& metadata);

/**
 * @brief Create an error response
 * 
 * @param errorMessage The error message
 * @return AIResponse The error response
 */
AIResponse CreateErrorResponse(const std::string& errorMessage);

/**
 * @brief Parse a response from a provider
 * 
 * @param responseText The response text
 * @param responseType The response type
 * @return AIResponse The parsed response
 */
AIResponse ParseProviderResponse(const std::string& responseText, AIResponseType responseType);

/**
 * @brief Extract actions from a response text
 * 
 * @param responseText The response text
 * @return std::map<std::string, std::string> The extracted actions
 */
std::map<std::string, std::string> ExtractActionsFromText(const std::string& responseText);

/**
 * @brief Format a response for display
 * 
 * @param response The response
 * @return std::string The formatted response
 */
std::string FormatResponseForDisplay(const AIResponse& response);

/**
 * @brief Check if a response is successful
 * 
 * @param response The response
 * @return true if the response is successful, false otherwise
 */
bool IsResponseSuccessful(const AIResponse& response);

/**
 * @brief Check if a response has actions
 * 
 * @param response The response
 * @return true if the response has actions, false otherwise
 */
bool HasResponseActions(const AIResponse& response);

/**
 * @brief Get an action from a response
 * 
 * @param response The response
 * @param key The action key
 * @return std::string The action value, or an empty string if not found
 */
std::string GetResponseAction(const AIResponse& response, const std::string& key);

/**
 * @brief Get metadata from a response
 * 
 * @param response The response
 * @param key The metadata key
 * @return std::string The metadata value, or an empty string if not found
 */
std::string GetResponseMetadata(const AIResponse& response, const std::string& key);

} // namespace ai
} // namespace rathena

#endif // AI_RESPONSE_HPP