#include "AISystemTests.hpp"
#include <thread>
#include <chrono>

TEST_CASE("AI System Core Functionality", "[ai_system]") {
    AISystemTest test;
    test.SetUp();
    
    SECTION("System Initialization") {
        REQUIRE(AISystemManager::getInstance().isEnabled());
        
        auto status = AISystemManager::getInstance().getSystemStatus();
        REQUIRE(status.system_enabled);
        REQUIRE(status.active_agents > 0);
    }
    
    SECTION("AI Model Integration") {
        auto request = test_helpers::createTestRequest("Test prompt");
        auto response = AISystemManager::getInstance().getModelManager()->sendRequest(request);
        test_helpers::verifyResponse(response, true);
    }
    
    SECTION("Memory Management") {
        auto memory_manager = AISystemManager::getInstance().getMemoryManager();
        
        // Test memory storage
        auto memory_id = memory_manager->storeMemory(
            "test_agent",
            "test_category",
            "test_data",
            MemoryType::SHORT_TERM,
            MemoryPriority::MEDIUM
        );
        REQUIRE(memory_id > 0);
        
        // Test memory retrieval
        auto memory = memory_manager->getMemory(memory_id);
        REQUIRE(memory != nullptr);
        REQUIRE(memory->getData() == "test_data");
    }
    
    test.TearDown();
}

TEST_CASE("Adaptive Balance AI Agent", "[agents][balance]") {
    AISystemTest test;
    test.SetUp();
    
    auto* agent = AISystemManager::getInstance().getBalanceAgent();
    REQUIRE(agent != nullptr);
    
    SECTION("Class Balance Analysis") {
        // Simulate class performance data
        ClassPerformanceData data;
        data.class_id = 1;
        data.win_rate = 0.6f;
        data.usage_rate = 0.3f;
        
        auto adjustment = agent->analyzeClassBalance(data);
        REQUIRE(adjustment.needs_adjustment);
        REQUIRE(adjustment.adjustment_factor < 1.0f);
    }
    
    SECTION("Skill Balance Analysis") {
        // Simulate skill usage data
        SkillUsageData data;
        data.skill_id = 100;
        data.success_rate = 0.8f;
        data.damage_contribution = 0.4f;
        
        auto analysis = agent->analyzeSkillBalance(data);
        REQUIRE(analysis.is_valid);
        REQUIRE_FALSE(analysis.recommendations.empty());
    }
    
    test.TearDown();
}

TEST_CASE("Dynamic Reward AI Agent", "[agents][reward]") {
    AISystemTest test;
    test.SetUp();
    
    auto* agent = AISystemManager::getInstance().getRewardAgent();
    REQUIRE(agent != nullptr);
    
    SECTION("Reward Generation") {
        PlayerContext context;
        context.account_id = 1000;
        context.play_time = 3600;
        context.achievement_count = 5;
        
        auto reward = agent->generateReward(context);
        REQUIRE(reward.is_valid());
        REQUIRE(reward.getDopamineImpact() > 0.0f);
    }
    
    SECTION("Dopamine Management") {
        // Test dopamine tracking
        agent->recordDopamineEvent(1000, 0.5f);
        agent->recordDopamineEvent(1000, 0.3f);
        
        auto level = agent->getCurrentDopamineLevel(1000);
        REQUIRE(level > 0.0f);
        REQUIRE(level < 1.0f);
    }
    
    test.TearDown();
}

TEST_CASE("Memory System Integration", "[memory]") {
    AISystemTest test;
    test.SetUp();
    
    auto memory_manager = AISystemManager::getInstance().getMemoryManager();
    
    SECTION("Memory Type Transitions") {
        // Store short-term memory
        auto memory_id = memory_manager->storeMemory(
            "test_agent",
            "important_event",
            "critical data",
            MemoryType::SHORT_TERM,
            MemoryPriority::HIGH
        );
        
        // Force memory consolidation
        memory_manager->consolidateMemories();
        
        // Verify memory was moved to long-term
        auto memory = memory_manager->getMemory(memory_id);
        REQUIRE(memory != nullptr);
        REQUIRE(memory->getMetadata().type == MemoryType::LONG_TERM);
    }
    
    SECTION("Memory Cleanup") {
        // Store low-priority memory
        memory_manager->storeMemory(
            "test_agent",
            "temp_data",
            "temporary info",
            MemoryType::SHORT_TERM,
            MemoryPriority::LOW
        );
        
        // Force cleanup
        memory_manager->cleanup();
        
        // Verify system state
        auto stats = memory_manager->getStats();
        REQUIRE(stats.short_term_count < stats.total_size);
    }
    
    test.TearDown();
}

TEST_CASE("AI System Performance", "[performance]") {
    AISystemTest test;
    test.SetUp();
    
    SECTION("Concurrent Requests") {
        const int NUM_REQUESTS = 100;
        std::vector<std::future<AIResponse>> futures;
        
        // Send multiple requests concurrently
        for (int i = 0; i < NUM_REQUESTS; ++i) {
            auto request = test_helpers::createTestRequest(
                "Test prompt " + std::to_string(i)
            );
            futures.push_back(
                AISystemManager::getInstance().getModelManager()->sendRequestAsync(request)
            );
        }
        
        // Verify all requests complete successfully
        for (auto& future : futures) {
            auto response = future.get();
            REQUIRE(response.success);
        }
    }
    
    SECTION("Memory Performance") {
        const int NUM_ENTRIES = 1000;
        
        // Measure memory operation time
        auto start = std::chrono::steady_clock::now();
        
        for (int i = 0; i < NUM_ENTRIES; ++i) {
            auto memory = test_helpers::createTestMemory(
                "test_agent",
                "perf_test",
                "test data " + std::to_string(i)
            );
            AISystemManager::getInstance().getMemoryManager()->storeMemory(
                memory.getAgentId(),
                memory.getCategory(),
                memory.getData(),
                memory.getMetadata().type,
                memory.getMetadata().priority
            );
        }
        
        auto end = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            end - start
        ).count();
        
        // Verify performance meets requirements
        REQUIRE(duration < 1000); // Should complete within 1 second
    }
    
    test.TearDown();
}

TEST_CASE("AI System Error Handling", "[error]") {
    AISystemTest test;
    test.SetUp();
    
    SECTION("Model Failure Recovery") {
        // Simulate model failure
        test.mock_model->setAvailable(false);
        test.mock_model->setLastError("Connection failed");
        
        auto request = test_helpers::createTestRequest("Test prompt");
        auto response = AISystemManager::getInstance().getModelManager()->sendRequest(request);
        
        // Verify system falls back to alternative model
        REQUIRE(response.success);
        REQUIRE(response.model_used != ModelType::AZURE_OPENAI);
    }
    
    SECTION("Invalid Memory Operations") {
        // Try to access non-existent memory
        auto memory = AISystemManager::getInstance().getMemoryManager()->getMemory(-1);
        REQUIRE(memory == nullptr);
        
        // Try to store invalid memory
        REQUIRE_THROWS_AS(
            AISystemManager::getInstance().getMemoryManager()->storeMemory(
                "", "", "", MemoryType::SHORT_TERM, MemoryPriority::LOW
            ),
            std::invalid_argument
        );
    }
    
    test.TearDown();
}