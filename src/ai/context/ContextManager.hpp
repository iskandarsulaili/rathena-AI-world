#ifndef CONTEXT_MANAGER_HPP
#define CONTEXT_MANAGER_HPP

#include <string>
#include <vector>
#include <map>
#include <deque>
#include <memory>
#include "common/cbasetypes.hpp"
#include "langchain/memory.hpp"
#include "langchain/chain.hpp"

class ContextManager {
public:
    // Context types
    enum class ContextType {
        CONVERSATION,
        GAME_STATE,
        PLAYER_HISTORY,
        SYSTEM_STATE,
        AGENT_MEMORY
    };

    // Context window configuration
    struct WindowConfig {
        size_t max_messages;
        size_t max_tokens;
        time_t retention_period;
        float importance_threshold;
    };

    // Context entry structure
    struct ContextEntry {
        std::string content;
        ContextType type;
        time_t timestamp;
        float importance_score;
        std::map<std::string, std::string> metadata;
    };

    // Context summary structure
    struct ContextSummary {
        std::string summary;
        std::vector<std::string> key_points;
        time_t start_time;
        time_t end_time;
        size_t num_entries;
    };

private:
    // Memory components
    std::unique_ptr<langchain::ConversationBufferWindowMemory> recent_memory;
    std::unique_ptr<langchain::ConversationSummaryMemory> summary_memory;
    std::unique_ptr<langchain::ConversationTokenBufferMemory> token_memory;

    // Context windows
    std::deque<ContextEntry> conversation_window;
    std::map<std::string, ContextEntry> game_state_context;
    std::map<int, std::deque<ContextEntry>> player_context;
    
    // Chain components
    std::unique_ptr<langchain::LLMChain> summarization_chain;
    std::unique_ptr<langchain::SequentialChain> reasoning_chain;
    
    // Configuration
    WindowConfig window_config;
    
    // Internal methods
    void pruneOldEntries();
    void consolidateContext();
    float calculateImportance(const ContextEntry& entry);
    std::string generateSummary(const std::vector<ContextEntry>& entries);

public:
    // Constructor and destructor
    ContextManager();
    ~ContextManager();

    // Initialization
    bool initialize(const WindowConfig& config);
    void shutdown();

    // Context management
    int64_t addContext(const std::string& content, 
                      ContextType type,
                      const std::map<std::string, std::string>& metadata = {});
    
    bool removeContext(int64_t context_id);
    
    bool updateContext(int64_t context_id,
                      const std::string& new_content,
                      float importance_score = -1.0f);

    // Context retrieval
    std::vector<ContextEntry> getRecentContext(ContextType type,
                                             size_t max_entries = 10);
    
    ContextSummary getContextSummary(ContextType type,
                                   time_t start_time = 0,
                                   time_t end_time = 0);
    
    std::vector<ContextEntry> queryContext(const std::string& query,
                                         ContextType type,
                                         size_t max_results = 5);

    // Player-specific context
    bool addPlayerContext(int account_id,
                         const std::string& content,
                         const std::map<std::string, std::string>& metadata = {});
    
    std::vector<ContextEntry> getPlayerContext(int account_id,
                                             size_t max_entries = 10);
    
    ContextSummary getPlayerSummary(int account_id);

    // Game state context
    bool updateGameState(const std::string& key,
                        const std::string& state,
                        const std::map<std::string, std::string>& metadata = {});
    
    std::string getGameState(const std::string& key);

    // Chain operations
    std::string processWithContext(const std::string& input,
                                 ContextType context_type,
                                 const std::map<std::string, std::string>& params = {});
    
    std::vector<std::string> reasonWithContext(const std::string& query,
                                             const std::vector<ContextType>& context_types);

    // Memory management
    void clearContext(ContextType type);
    void clearPlayerContext(int account_id);
    size_t getCurrentContextSize(ContextType type);

    // Status and monitoring
    struct ContextStats {
        size_t total_entries;
        size_t active_contexts;
        size_t summarized_contexts;
        std::map<ContextType, size_t> entries_by_type;
        float memory_usage;
    };
    ContextStats getStats() const;

    // Configuration
    void setWindowConfig(const WindowConfig& config);
    const WindowConfig& getWindowConfig() const;
};

#endif // CONTEXT_MANAGER_HPP