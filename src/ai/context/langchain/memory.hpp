#ifndef LANGCHAIN_MEMORY_HPP
#define LANGCHAIN_MEMORY_HPP

#include <string>
#include <vector>
#include <deque>
#include <map>
#include <memory>
#include <nlohmann/json.hpp>
#include "base.hpp"

namespace langchain {

// Memory key constants
struct MemoryKeys {
    static constexpr const char* HISTORY = "history";
    static constexpr const char* SUMMARY = "summary";
    static constexpr const char* CONTEXT = "context";
    static constexpr const char* INPUT = "input";
    static constexpr const char* OUTPUT = "output";
};

// Memory entry with metadata
struct MemoryEntry {
    std::string content;
    nlohmann::json metadata;
    time_t timestamp;
    size_t token_count;
    float importance_score;

    MemoryEntry(std::string c, nlohmann::json meta = nlohmann::json::object())
        : content(std::move(c))
        , metadata(std::move(meta))
        , timestamp(time(nullptr))
        , token_count(0)
        , importance_score(0.5f) {}
};

// Base memory class
class Memory : public BaseObject {
public:
    virtual ~Memory() = default;
    virtual void clear() = 0;
    virtual nlohmann::json load() = 0;
    virtual void save(const nlohmann::json& data) = 0;
    virtual std::vector<std::string> getMemoryKeys() const = 0;
    virtual size_t getMemorySize() const = 0;
};

// Window-based memory buffer
class ConversationBufferWindowMemory : public Memory {
private:
    size_t k;
    std::deque<MemoryEntry> buffer;
    bool return_messages;
    std::map<std::string, std::string> input_key_mapping;
    std::map<std::string, std::string> output_key_mapping;

public:
    explicit ConversationBufferWindowMemory(size_t window_size = 5);
    
    void addMessage(const std::string& content,
                   const nlohmann::json& metadata = nlohmann::json::object());
    
    std::vector<MemoryEntry> getMessages(size_t limit = 0) const;
    
    // Memory interface implementation
    void clear() override;
    nlohmann::json load() override;
    void save(const nlohmann::json& data) override;
    std::vector<std::string> getMemoryKeys() const override;
    size_t getMemorySize() const override;

    // Configuration
    void setK(size_t new_k) { k = new_k; }
    void setReturnMessages(bool value) { return_messages = value; }
    void setInputKeyMapping(const std::string& from, const std::string& to);
    void setOutputKeyMapping(const std::string& from, const std::string& to);
};

// Token-aware memory buffer
class ConversationTokenBufferMemory : public Memory {
private:
    size_t max_tokens;
    std::vector<MemoryEntry> messages;
    std::unique_ptr<Tokenizer> tokenizer;
    float truncation_ratio;

public:
    explicit ConversationTokenBufferMemory(
        size_t max_tokens = 2000,
        float truncation_ratio = 0.8f
    );

    void addMessage(const std::string& content,
                   const nlohmann::json& metadata = nlohmann::json::object());
    
    // Memory interface implementation
    void clear() override;
    nlohmann::json load() override;
    void save(const nlohmann::json& data) override;
    std::vector<std::string> getMemoryKeys() const override;
    size_t getMemorySize() const override;

    // Token management
    size_t getCurrentTokenCount() const;
    void prune();
    void setMaxTokens(size_t tokens) { max_tokens = tokens; }
    void setTruncationRatio(float ratio) { truncation_ratio = ratio; }
};

// Summary-based memory
class ConversationSummaryMemory : public Memory {
private:
    std::string current_summary;
    std::unique_ptr<LLMChain> summarizer_chain;
    size_t max_summary_length;
    float compression_ratio;
    std::vector<MemoryEntry> recent_messages;

    std::string generateSummary(const std::vector<MemoryEntry>& messages);
    bool shouldUpdateSummary(const MemoryEntry& new_message) const;

public:
    explicit ConversationSummaryMemory(
        std::unique_ptr<LLMChain> summarizer,
        size_t max_length = 500,
        float compression = 0.3f
    );

    void addMessage(const std::string& content,
                   const nlohmann::json& metadata = nlohmann::json::object());
    
    // Memory interface implementation
    void clear() override;
    nlohmann::json load() override;
    void save(const nlohmann::json& data) override;
    std::vector<std::string> getMemoryKeys() const override;
    size_t getMemorySize() const override;

    // Summary management
    std::string getSummary() const { return current_summary; }
    void forceSummarize();
    void setMaxSummaryLength(size_t length) { max_summary_length = length; }
    void setCompressionRatio(float ratio) { compression_ratio = ratio; }
};

// Utility functions for memory management
namespace memory_utils {
    // Memory entry handling
    MemoryEntry createEntry(const std::string& content,
                          const nlohmann::json& metadata = nlohmann::json::object());
    
    size_t estimateTokenCount(const MemoryEntry& entry);
    
    float calculateImportance(const MemoryEntry& entry);
    
    // Memory operations
    std::vector<MemoryEntry> filterByTimestamp(
        const std::vector<MemoryEntry>& entries,
        time_t start,
        time_t end
    );
    
    std::vector<MemoryEntry> filterByImportance(
        const std::vector<MemoryEntry>& entries,
        float min_importance
    );
    
    // Serialization
    nlohmann::json serializeEntry(const MemoryEntry& entry);
    MemoryEntry deserializeEntry(const nlohmann::json& json);
}

} // namespace langchain

#endif // LANGCHAIN_MEMORY_HPP