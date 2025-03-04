#ifndef LANGCHAIN_MEMORY_HPP
#define LANGCHAIN_MEMORY_HPP

#include <string>
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>
#include "langchain/base.hpp"
#include "langchain/schema.hpp"

namespace langchain {

// Base memory class for conversation management
class BaseMemory {
public:
    virtual ~BaseMemory() = default;
    virtual void clear() = 0;
    virtual nlohmann::json load() = 0;
    virtual void save(const nlohmann::json& data) = 0;
    virtual size_t getMemorySize() const = 0;
};

// Window-based conversation buffer
class ConversationBufferWindowMemory : public BaseMemory {
private:
    size_t k;  // window size
    std::deque<Message> messages;
    std::string memory_key;
    bool return_messages;

public:
    explicit ConversationBufferWindowMemory(
        size_t k = 5,
        std::string memory_key = "history",
        bool return_messages = false
    );

    void clear() override;
    nlohmann::json load() override;
    void save(const nlohmann::json& data) override;
    size_t getMemorySize() const override;

    void addMessage(const Message& message);
    std::vector<Message> getMessages() const;
    void setK(size_t new_k);
};

// Token-aware conversation buffer
class ConversationTokenBufferMemory : public BaseMemory {
private:
    size_t max_tokens;
    std::vector<Message> messages;
    std::unique_ptr<Tokenizer> tokenizer;
    float truncation_ratio;

public:
    explicit ConversationTokenBufferMemory(
        size_t max_tokens = 2000,
        float truncation_ratio = 0.8
    );

    void clear() override;
    nlohmann::json load() override;
    void save(const nlohmann::json& data) override;
    size_t getMemorySize() const override;

    void addMessage(const Message& message);
    std::vector<Message> getMessages() const;
    size_t getCurrentTokenCount() const;
    void prune();
};

// Summary-based conversation memory
class ConversationSummaryMemory : public BaseMemory {
private:
    std::string current_summary;
    std::unique_ptr<LLMChain> summarizer_chain;
    size_t max_summary_length;
    float compression_ratio;

    std::string generateSummary(const std::vector<Message>& messages);
    bool shouldUpdateSummary(const Message& new_message);

public:
    explicit ConversationSummaryMemory(
        std::unique_ptr<LLMChain> summarizer,
        size_t max_length = 500,
        float compression = 0.3
    );

    void clear() override;
    nlohmann::json load() override;
    void save(const nlohmann::json& data) override;
    size_t getMemorySize() const override;

    void addMessage(const Message& message);
    std::string getSummary() const;
    void forceSummarize();
};

// Message structure for conversation
struct Message {
    enum class Role {
        HUMAN,
        AI,
        SYSTEM
    };

    Role role;
    std::string content;
    time_t timestamp;
    nlohmann::json metadata;

    Message(Role r, std::string c, nlohmann::json meta = nlohmann::json::object())
        : role(r), content(std::move(c)), timestamp(time(nullptr)), 
          metadata(std::move(meta)) {}
};

// Chain for summarization
class SummarizationChain : public LLMChain {
private:
    std::string summarization_template;
    size_t max_tokens;
    float temperature;

public:
    explicit SummarizationChain(
        std::string template_str,
        size_t max_tokens = 500,
        float temperature = 0.7
    );

    std::string summarize(const std::vector<Message>& messages);
    std::string updateSummary(const std::string& current_summary, 
                             const std::vector<Message>& new_messages);
};

// Utility functions
namespace memory_utils {
    size_t estimateTokens(const std::string& text);
    std::string truncateText(const std::string& text, size_t max_tokens);
    nlohmann::json serializeMessages(const std::vector<Message>& messages);
    std::vector<Message> deserializeMessages(const nlohmann::json& json);
}

} // namespace langchain

#endif // LANGCHAIN_MEMORY_HPP