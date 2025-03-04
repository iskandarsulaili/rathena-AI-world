#ifndef AI_MODEL_HPP
#define AI_MODEL_HPP

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>
#include "common/cbasetypes.hpp"

// Forward declare model-related types
enum class ModelType {
    AZURE_OPENAI,
    OPENAI_GPT4,
    DEEPSEEK_V3
};

// Model configuration structure
struct ModelConfig {
    std::string api_key;
    std::string endpoint;
    std::string model_version;
    int max_tokens;
    float temperature;
    bool stream;
    std::map<std::string, std::string> additional_params;

    ModelConfig() : max_tokens(2000), temperature(0.7), stream(false) {}
};

// Request structure
struct AIRequest {
    std::string prompt;
    std::string agent_id;
    ModelType preferred_model;
    int max_tokens;
    float temperature;
    int priority;
    time_t timeout;
    std::function<void(const std::string&)> callback;

    AIRequest() : max_tokens(2000), temperature(0.7), priority(0), timeout(30) {}
};

// Response structure
struct AIResponse {
    std::string result;
    bool success;
    std::string error;
    ModelType model_used;
    int tokens_used;
    time_t response_time;

    AIResponse() : success(false), tokens_used(0), response_time(0) {}
};

class AIModel {
public:
    virtual ~AIModel() = default;

    // Core operations
    virtual bool initialize(const ModelConfig& config) = 0;
    virtual void shutdown() = 0;
    virtual AIResponse sendRequest(const AIRequest& request) = 0;
    virtual bool isAvailable() const = 0;

    // Optional streaming support
    virtual bool supportsStreaming() const { return false; }
    virtual bool streamResponse(const AIRequest& request,
                              std::function<void(const std::string&)> callback) {
        return false;
    }

    // Rate limiting
    virtual void setRateLimit(int requests_per_minute) = 0;
    virtual bool checkRateLimit() const = 0;

    // Model information
    virtual ModelType getType() const = 0;
    virtual std::string getModelVersion() const = 0;
    virtual size_t getMaxTokens() const = 0;

    // Error handling
    virtual std::string getLastError() const = 0;
    virtual void clearError() = 0;

    // Cost tracking
    virtual float getTokenCost() const = 0;
    virtual size_t estimateTokens(const std::string& text) const = 0;
};

class AzureOpenAIModel : public AIModel {
private:
    ModelConfig config;
    std::string last_error;
    time_t last_request;
    int requests_per_minute;
    size_t requests_this_minute;
    std::map<time_t, size_t> request_history;

    // Azure OpenAI specific members
    std::string access_token;
    time_t token_expiry;
    std::string deployment_id;

    bool refreshAccessToken();
    void updateRequestHistory();
    AIResponse processResponse(const std::string& response);

public:
    AzureOpenAIModel();
    ~AzureOpenAIModel() override;

    // Implement AIModel interface
    bool initialize(const ModelConfig& config) override;
    void shutdown() override;
    AIResponse sendRequest(const AIRequest& request) override;
    bool isAvailable() const override;
    void setRateLimit(int requests_per_minute) override;
    bool checkRateLimit() const override;
    ModelType getType() const override;
    std::string getModelVersion() const override;
    size_t getMaxTokens() const override;
    std::string getLastError() const override;
    void clearError() override;
    float getTokenCost() const override;
    size_t estimateTokens(const std::string& text) const override;
};

class OpenAIGPT4Model : public AIModel {
private:
    ModelConfig config;
    std::string last_error;
    time_t last_request;
    int requests_per_minute;
    size_t requests_this_minute;
    std::map<time_t, size_t> request_history;

    void updateRequestHistory();
    AIResponse processResponse(const std::string& response);

public:
    OpenAIGPT4Model();
    ~OpenAIGPT4Model() override;

    // Implement AIModel interface
    bool initialize(const ModelConfig& config) override;
    void shutdown() override;
    AIResponse sendRequest(const AIRequest& request) override;
    bool isAvailable() const override;
    void setRateLimit(int requests_per_minute) override;
    bool checkRateLimit() const override;
    ModelType getType() const override;
    std::string getModelVersion() const override;
    size_t getMaxTokens() const override;
    std::string getLastError() const override;
    void clearError() override;
    float getTokenCost() const override;
    size_t estimateTokens(const std::string& text) const override;
};

class DeepSeekModel : public AIModel {
private:
    ModelConfig config;
    std::string last_error;
    time_t last_request;
    int requests_per_minute;
    size_t requests_this_minute;
    std::map<time_t, size_t> request_history;

    std::string api_base_url;
    bool validate_certificates;

    void updateRequestHistory();
    AIResponse processResponse(const std::string& response);

public:
    DeepSeekModel();
    ~DeepSeekModel() override;

    // Implement AIModel interface
    bool initialize(const ModelConfig& config) override;
    void shutdown() override;
    AIResponse sendRequest(const AIRequest& request) override;
    bool isAvailable() const override;
    void setRateLimit(int requests_per_minute) override;
    bool checkRateLimit() const override;
    ModelType getType() const override;
    std::string getModelVersion() const override;
    size_t getMaxTokens() const override;
    std::string getLastError() const override;
    void clearError() override;
    float getTokenCost() const override;
    size_t estimateTokens(const std::string& text) const override;

    // DeepSeek specific methods
    void setValidateCertificates(bool validate);
    void setApiBaseUrl(const std::string& url);
};

// Helper functions
namespace ai_model_utils {
    std::string escapeJsonString(const std::string& input);
    std::string sanitizePrompt(const std::string& prompt);
    size_t estimateTokenCount(const std::string& text);
    std::string formatError(const std::string& message, const std::string& details = "");
    bool validateApiKey(const std::string& api_key, ModelType model_type);
    std::string getDefaultEndpoint(ModelType model_type);
}

#endif // AI_MODEL_HPP