#ifndef AI_PROVIDER_HPP
#define AI_PROVIDER_HPP

#include <memory>
#include <string>
#include "ai_types.hpp"

namespace rathena {
namespace ai {

/**
 * @brief Interface for AI providers
 * 
 * This interface defines the contract for AI providers that can be used
 * by the AI module to generate responses.
 */
class AIProvider {
public:
    /**
     * @brief Virtual destructor
     */
    virtual ~AIProvider() = default;
    
    /**
     * @brief Initialize the provider
     * 
     * @return true if initialization was successful, false otherwise
     */
    virtual bool Initialize() = 0;
    
    /**
     * @brief Generate a response to a request
     * 
     * @param request The AI request
     * @return AIResponse The generated response
     */
    virtual AIResponse GenerateResponse(const AIRequest& request) = 0;
    
    /**
     * @brief Get the provider's capabilities
     * 
     * @return AICapabilities The provider's capabilities
     */
    virtual AICapabilities GetCapabilities() const = 0;
    
    /**
     * @brief Get the provider's status
     * 
     * @return AIProviderStatus The provider's status
     */
    virtual AIProviderStatus GetStatus() const = 0;
    
    /**
     * @brief Set the provider's status
     * 
     * @param status The new status
     */
    virtual void SetStatus(AIProviderStatus status) = 0;
    
    /**
     * @brief Get the provider's name
     * 
     * @return std::string The provider's name
     */
    virtual std::string GetName() const = 0;
    
    /**
     * @brief Get the provider's type
     * 
     * @return std::string The provider's type
     */
    virtual std::string GetType() const = 0;
};

/**
 * @brief Factory for creating AI providers
 */
class AIProviderFactory {
public:
    /**
     * @brief Create a provider of the specified type
     * 
     * @param providerType The type of provider to create
     * @param config The configuration for the provider
     * @return std::unique_ptr<AIProvider> The created provider
     */
    static std::unique_ptr<AIProvider> CreateProvider(const std::string& providerType, const ConfigMap& config);
};

} // namespace ai
} // namespace rathena

#endif // AI_PROVIDER_HPP