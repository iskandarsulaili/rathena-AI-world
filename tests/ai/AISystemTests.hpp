#ifndef AI_SYSTEM_TESTS_HPP
#define AI_SYSTEM_TESTS_HPP

#include <catch2/catch.hpp>
#include "ai/AISystemManager.hpp"
#include "ai/memory/MemoryManager.hpp"
#include "ai/model/AIModelManager.hpp"

// Mock classes for testing
class MockAIModel : public AIModel {
private:
    bool m_available;
    std::string m_last_error;
    ModelType m_type;

public:
    explicit MockAIModel(ModelType type = ModelType::AZURE_OPENAI)
        : m_available(true), m_type(type) {}
    
    bool initialize(const ModelConfig& config) override { return true; }
    void shutdown() override {}
    AIResponse sendRequest(const AIRequest& request) override {
        AIResponse response;
        response.success = true;
        response.result = "Mock response for: " + request.prompt;
        return response;
    }
    bool isAvailable() const override { return m_available; }
    void setRateLimit(int requests_per_minute) override {}
    bool checkRateLimit() const override { return true; }
    ModelType getType() const override { return m_type; }
    std::string getModelVersion() const override { return "mock-1.0"; }
    size_t getMaxTokens() const override { return 2000; }
    std::string getLastError() const override { return m_last_error; }
    void clearError() override { m_last_error.clear(); }
    float getTokenCost() const override { return 0.0f; }
    size_t estimateTokens(const std::string& text) const override { return text.length() / 4; }
    
    // Test control methods
    void setAvailable(bool available) { m_available = available; }
    void setLastError(const std::string& error) { m_last_error = error; }
};

class MockMemoryManager : public MemoryManager {
private:
    std::map<int64_t, std::shared_ptr<MemoryEntry>> m_memories;
    int64_t m_next_id;

public:
    MockMemoryManager() : m_next_id(1) {}
    
    int64_t storeMemory(const std::string& agent_id, const std::string& category,
                        const std::string& data, MemoryType type, MemoryPriority priority) override {
        auto entry = std::make_shared<MemoryEntry>(agent_id, category, data, type, priority);
        int64_t id = m_next_id++;
        m_memories[id] = entry;
        return id;
    }
    
    std::shared_ptr<MemoryEntry> getMemory(int64_t memory_id) override {
        auto it = m_memories.find(memory_id);
        return it != m_memories.end() ? it->second : nullptr;
    }
    
    bool removeMemory(int64_t memory_id) override {
        return m_memories.erase(memory_id) > 0;
    }
    
    void cleanup() override {
        m_memories.clear();
    }
    
    size_t getCurrentUsage() const override {
        return m_memories.size();
    }
};

// Test fixture for AI system tests
class AISystemTest {
protected:
    std::unique_ptr<MockAIModel> mock_model;
    std::unique_ptr<MockMemoryManager> mock_memory;
    std::unique_ptr<AISystemManager> system_manager;
    
    void SetUp() {
        mock_model = std::make_unique<MockAIModel>();
        mock_memory = std::make_unique<MockMemoryManager>();
        
        // Create system manager with mock components
        system_manager = std::make_unique<AISystemManager>();
        
        // Initialize with test configuration
        AISystemManager::Config config;
        config.enabled = true;
        config.primary_model = "azure_openai";
        system_manager->initialize();
    }
    
    void TearDown() {
        system_manager->shutdown();
        system_manager.reset();
        mock_memory.reset();
        mock_model.reset();
    }
};

// Helper functions for tests
namespace test_helpers {
    // Create test AI request
    inline AIRequest createTestRequest(const std::string& prompt, 
                                     const std::string& agent_id = "test_agent",
                                     ModelType model = ModelType::AZURE_OPENAI) {
        AIRequest request;
        request.prompt = prompt;
        request.agent_id = agent_id;
        request.preferred_model = model;
        request.max_tokens = 100;
        request.temperature = 0.7f;
        request.priority = 1;
        request.timeout = 30;
        return request;
    }
    
    // Create test memory entry
    inline MemoryEntry createTestMemory(const std::string& agent_id,
                                      const std::string& category,
                                      const std::string& data) {
        return MemoryEntry(agent_id, category, data, 
                          MemoryType::SHORT_TERM, MemoryPriority::MEDIUM);
    }
    
    // Verify AI response
    inline void verifyResponse(const AIResponse& response, bool expected_success) {
        REQUIRE(response.success == expected_success);
        if (expected_success) {
            REQUIRE_FALSE(response.result.empty());
            REQUIRE(response.tokens_used > 0);
        } else {
            REQUIRE_FALSE(response.error.empty());
        }
    }
    
    // Wait for async operations
    inline void waitForAsync(std::function<bool()> condition, 
                           int timeout_ms = 5000, int check_interval_ms = 100) {
        auto start = std::chrono::steady_clock::now();
        while (!condition()) {
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);
            if (elapsed.count() >= timeout_ms) {
                FAIL("Async operation timed out");
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(check_interval_ms));
        }
    }
}

#endif // AI_SYSTEM_TESTS_HPP