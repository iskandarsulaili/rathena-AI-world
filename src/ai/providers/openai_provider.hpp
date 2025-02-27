#ifndef OPENAI_PROVIDER_HPP
#define OPENAI_PROVIDER_HPP

#include <string>
#include <mutex>
#include <atomic>
#include "../common/ai_provider.hpp"
#include "../common/ai_types.hpp"
#include "../../common/http_client.hpp"

namespace rathena {
namespace ai {

/**
 * @brief OpenAI provider implementation
 * 
 * This class implements the AIProvider interface for the OpenAI API.
 */
class OpenAIProvider : public AIProvider {
private:
    std::string name_;
    std::string type_;
    std::string apiKey_;
    std::string model_;
    int maxTokens_;
    float temperature_;
    float topP_;
    float frequencyPenalty_;
    float presencePenalty_;
    std::atomic<AIProviderStatus> status_;
    mutable std::mutex mutex_;
    
    // HTTP client for API requests
    std::unique_ptr<HttpClient> httpClient_;
    
    // Capabilities
    AICapabilities capabilities_;

public:
    /**
     * @brief Constructor
     * 
     * @param name The provider name
     * @param apiKey The OpenAI API key
     * @param model The model to use
     * @param config Additional configuration
     */
    OpenAIProvider(const std::string& name, const std::string& apiKey, const std::string& model, const ConfigMap& config);
    
    /**
     * @brief Destructor
     */
    ~OpenAIProvider() override;
    
    /**
     * @brief Initialize the provider
     * 
     * @return true if initialization was successful, false otherwise
     */
    bool Initialize() override;
    
    /**
     * @brief Generate a response to a request
     * 
     * @param request The AI request
     * @return AIResponse The generated response
     */
    AIResponse GenerateResponse(const AIRequest& request) override;
    
    /**
     * @brief Get the provider's capabilities
     * 
     * @return AICapabilities The provider's capabilities
     */
    AICapabilities GetCapabilities() const override;
    
    /**
     * @brief Get the provider's status
     * 
     * @return AIProviderStatus The provider's status
     */
    AIProviderStatus GetStatus() const override;
    
    /**
     * @brief Set the provider's status
     * 
     * @param status The new status
     */
    void SetStatus(AIProviderStatus status) override;
    
    /**
     * @brief Get the provider's name
     * 
     * @return std::string The provider's name
     */
    std::string GetName() const override;
    
    /**
     * @brief Get the provider's type
     * 
     * @return std::string The provider's type
     */
    std::string GetType() const override;

private:
    /**
     * @brief Build the request payload for the OpenAI API
     * 
     * @param request The AI request
     * @return std::string The JSON payload
     */
    std::string BuildRequestPayload(const AIRequest& request);
    
    /**
     * @brief Parse the response from the OpenAI API
     * 
     * @param responseJson The JSON response
     * @return AIResponse The parsed response
     */
    AIResponse ParseResponse(const std::string& responseJson);
    
    /**
     * @brief Initialize the provider's capabilities
     */
    void InitializeCapabilities();
};

} // namespace ai
} // namespace rathena

#endif // OPENAI_PROVIDER_HPP