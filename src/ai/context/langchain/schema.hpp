#ifndef LANGCHAIN_SCHEMA_HPP
#define LANGCHAIN_SCHEMA_HPP

#include <string>
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>
#include "base.hpp"

namespace langchain {

// Message types and roles
enum class MessageRole {
    SYSTEM,
    USER,
    ASSISTANT,
    FUNCTION
};

// Message structure
struct Message {
    MessageRole role;
    std::string content;
    time_t timestamp;
    nlohmann::json additional_kwargs;
    std::string name;  // For function messages

    Message(MessageRole r, std::string c)
        : role(r), content(std::move(c)), timestamp(time(nullptr)) {}
};

// Function calling schema
struct FunctionDefinition {
    std::string name;
    std::string description;
    nlohmann::json parameters;
    bool required;

    FunctionDefinition() : required(false) {}
};

// Response format schema
struct ResponseFormat {
    enum class Type {
        JSON_OBJECT,
        TEXT
    };

    Type type;
    nlohmann::json schema;
};

// Generation configuration
struct GenerationConfig {
    size_t max_tokens;
    float temperature;
    float top_p;
    float frequency_penalty;
    float presence_penalty;
    std::vector<std::string> stop;
    std::vector<FunctionDefinition> functions;
    ResponseFormat response_format;

    GenerationConfig()
        : max_tokens(2000)
        , temperature(0.7f)
        , top_p(1.0f)
        , frequency_penalty(0.0f)
        , presence_penalty(0.0f) {}
};

// Chat template for different models
class ChatTemplate {
public:
    virtual ~ChatTemplate() = default;
    virtual std::string formatMessages(const std::vector<Message>& messages) = 0;
    virtual std::vector<Message> parseResponse(const std::string& response) = 0;
};

// Base prompt template
class BasePromptTemplate {
protected:
    std::vector<std::string> input_variables;
    std::string template_format;
    bool validate_template;

public:
    explicit BasePromptTemplate(std::string format, bool validate = true)
        : template_format(std::move(format)), validate_template(validate) {}
    
    virtual ~BasePromptTemplate() = default;
    virtual std::string format(const nlohmann::json& values) = 0;
    virtual std::vector<std::string> getInputVariables() const {
        return input_variables;
    }
};

// Function call result
struct FunctionCallResult {
    std::string function_name;
    nlohmann::json arguments;
    bool success;
    std::string error;
    nlohmann::json result;
};

// Chain input/output schema
struct ChainValues {
    nlohmann::json inputs;
    nlohmann::json outputs;
    std::vector<std::string> return_values;
    std::map<std::string, FunctionCallResult> function_calls;
};

// Memory backend interface
class MemoryBackend {
public:
    virtual ~MemoryBackend() = default;
    virtual void store(const std::string& key, const nlohmann::json& value) = 0;
    virtual nlohmann::json retrieve(const std::string& key) = 0;
    virtual void remove(const std::string& key) = 0;
    virtual void clear() = 0;
};

// In-memory backend implementation
class InMemoryBackend : public MemoryBackend {
private:
    std::map<std::string, nlohmann::json> storage;

public:
    void store(const std::string& key, const nlohmann::json& value) override {
        storage[key] = value;
    }
    
    nlohmann::json retrieve(const std::string& key) override {
        auto it = storage.find(key);
        return it != storage.end() ? it->second : nlohmann::json(nullptr);
    }
    
    void remove(const std::string& key) override {
        storage.erase(key);
    }
    
    void clear() override {
        storage.clear();
    }
};

// JSON serialization helpers
inline void to_json(nlohmann::json& j, const Message& m) {
    j = {
        {"role", m.role},
        {"content", m.content},
        {"timestamp", m.timestamp},
        {"additional_kwargs", m.additional_kwargs}
    };
    if (!m.name.empty()) {
        j["name"] = m.name;
    }
}

inline void from_json(const nlohmann::json& j, Message& m) {
    j.at("role").get_to(m.role);
    j.at("content").get_to(m.content);
    j.at("timestamp").get_to(m.timestamp);
    j.at("additional_kwargs").get_to(m.additional_kwargs);
    if (j.contains("name")) {
        j.at("name").get_to(m.name);
    }
}

} // namespace langchain

#endif // LANGCHAIN_SCHEMA_HPP