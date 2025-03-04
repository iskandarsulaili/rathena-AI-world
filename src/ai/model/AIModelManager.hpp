#ifndef AI_MODEL_MANAGER_HPP
#define AI_MODEL_MANAGER_HPP

#include <string>
#include <vector>
#include <queue>
#include <map>
#include <memory>
#include <mutex>
#include <future>
#include "common/cbasetypes.hpp"
#include "common/timer.hpp"

// Forward declarations
class AIModel;
class RequestQueue;
class ResponseCache;

enum class ModelType {
    AZURE_OPENAI,
    OPENAI_GPT4,
    DEEPSEEK_V3
};

struct ModelConfig {
    std::string api_key;
    std::string endpoint;
    std::string model_version;
    int max_tokens;
    float temperature;
    bool stream;
    std::map<std::string, std::string> additional_params;
};

struct AIRequest {
    std::string prompt;
    std::string agent_id;
    ModelType preferred_model;
    int max_tokens;
    float temperature;
    int priority;
    time_t timeout;
    std::function<void(const std::string&)> callback;
};

struct AIResponse {
    std::string result;
    bool success;
    std::string error;
    ModelType model_used;
    int tokens_used;
    time_t response_time;
};

class AIModelManager {
private:
    // Model instances
    std::unique_ptr<AIModel> azure_openai;
    std::unique_ptr<AIModel> openai_gpt4;
    std::unique_ptr<AIModel> deepseek_v3;
    
    // Configuration
    ModelType primary_model;
    ModelType fallback_model;
    std::map<ModelType, ModelConfig> model_configs;
    
    // Request management
    std::unique_ptr<RequestQueue> request_queue;
    std::unique_ptr<ResponseCache> response_cache;
    
    // Performance tracking
    struct ModelStats {
        size_t requests_processed;
        size_t requests_failed;
        size_t tokens_used;
        float average_response_time;
        time_t last_error;
        std::string last_error_message;
    };
    std::map<ModelType, ModelStats> model_stats;
    
    // Thread management
    std::mutex mutex;
    bool running;
    std::vector<std::future<void>> worker_threads;
    
    // Internal methods
    AIResponse processRequest(const AIRequest& request);
    bool switchToFallbackModel(ModelType current_model);
    void updateModelStats(ModelType model, const AIResponse& response);
    std::string sanitizePrompt(const std::string& prompt);
    
public:
    // Constructor and destructor
    AIModelManager(const std::string& primary = "azure_openai",
                  const std::string& fallback = "deepseek_v3");
    ~AIModelManager();
    
    // Initialization
    bool initialize();
    void shutdown();
    
    // Core operations
    AIResponse sendRequest(const AIRequest& request);
    std::future<AIResponse> sendRequestAsync(const AIRequest& request);
    void cancelRequest(const std::string& request_id);
    
    // Batch operations
    std::vector<AIResponse> sendBatchRequests(const std::vector<AIRequest>& requests);
    void streamResponse(const AIRequest& request, 
                       std::function<void(const std::string&)> callback);
    
    // Configuration
    void setModelConfig(ModelType model, const ModelConfig& config);
    bool setPrimaryModel(ModelType model);
    bool setFallbackModel(ModelType model);
    
    // Cache management
    void setCacheEnabled(bool enabled);
    void clearCache();
    void setCacheDuration(int seconds);
    
    // Status and monitoring
    struct ManagerStats {
        std::map<ModelType, ModelStats> model_stats;
        size_t pending_requests;
        size_t cache_size;
        float system_load;
        time_t uptime;
    };
    ManagerStats getStats() const;
    size_t getPendingRequests() const;
    bool isModelAvailable(ModelType model) const;
    
    // Error handling
    struct ErrorReport {
        ModelType model;
        std::string error_code;
        std::string message;
        time_t timestamp;
        std::string request_id;
    };
    std::vector<ErrorReport> getRecentErrors(ModelType model, int limit = 10) const;
    
    // Rate limiting
    void setRateLimit(ModelType model, int requests_per_minute);
    void setPriorityThreshold(int priority_level);
    
    // Cost management
    struct CostReport {
        ModelType model;
        size_t tokens_used;
        float estimated_cost;
        time_t period_start;
        time_t period_end;
    };
    CostReport getCostReport(ModelType model, time_t start_time, time_t end_time) const;
    
    // Utility methods
    static ModelType stringToModelType(const std::string& model_name);
    static std::string modelTypeToString(ModelType model);
    static bool validateApiKey(const std::string& api_key, ModelType model);
};

// Helper class for request queuing
class RequestQueue {
private:
    struct QueuedRequest {
        AIRequest request;
        time_t enqueue_time;
        int priority;
        bool operator<(const QueuedRequest& other) const {
            return priority < other.priority;
        }
    };
    
    std::priority_queue<QueuedRequest> queue;
    std::mutex mutex;
    size_t max_size;
    
public:
    RequestQueue(size_t max_size = 1000);
    bool enqueue(const AIRequest& request);
    bool dequeue(AIRequest& request);
    size_t size() const;
    void clear();
};

// Helper class for response caching
class ResponseCache {
private:
    struct CachedResponse {
        AIResponse response;
        time_t cache_time;
        time_t expiry_time;
    };
    
    std::map<std::string, CachedResponse> cache;
    std::mutex mutex;
    size_t max_size;
    int cache_duration;
    
    std::string generateCacheKey(const AIRequest& request);
    
public:
    ResponseCache(size_t max_size = 10000, int duration = 300);
    bool get(const AIRequest& request, AIResponse& response);
    void put(const AIRequest& request, const AIResponse& response);
    void clear();
    size_t size() const;
    void removeExpired();
};

#endif // AI_MODEL_MANAGER_HPP