#ifndef LANGCHAIN_BASE_HPP
#define LANGCHAIN_BASE_HPP

#include <string>
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>

namespace langchain {

// Basic types and structures
class BaseObject {
public:
    virtual ~BaseObject() = default;
    virtual std::string serialize() const = 0;
    virtual void deserialize(const std::string& data) = 0;
};

// Base tokenizer interface
class Tokenizer {
public:
    virtual ~Tokenizer() = default;
    virtual std::vector<std::string> tokenize(const std::string& text) = 0;
    virtual size_t countTokens(const std::string& text) = 0;
    virtual std::string detokenize(const std::vector<std::string>& tokens) = 0;
};

// Base LLM interface
class LLM {
public:
    virtual ~LLM() = default;
    
    struct Request {
        std::string prompt;
        size_t max_tokens;
        float temperature;
        float top_p;
        int n;
        std::vector<std::string> stop;
    };
    
    struct Response {
        std::string text;
        size_t tokens_used;
        bool success;
        std::string error;
    };
    
    virtual Response generate(const Request& request) = 0;
    virtual bool isAvailable() const = 0;
    virtual std::string getModelName() const = 0;
};

// Chain interface
class Chain : public BaseObject {
public:
    virtual ~Chain() = default;
    virtual nlohmann::json run(const nlohmann::json& inputs) = 0;
    virtual std::vector<std::string> getInputKeys() const = 0;
    virtual std::vector<std::string> getOutputKeys() const = 0;
};

// Template handling
class PromptTemplate : public BaseObject {
public:
    struct Variable {
        std::string name;
        std::string description;
        bool required;
        std::string default_value;
    };
    
    virtual ~PromptTemplate() = default;
    virtual std::string format(const nlohmann::json& variables) = 0;
    virtual std::vector<Variable> getVariables() const = 0;
};

// Base LLM Chain
class LLMChain : public Chain {
protected:
    std::shared_ptr<LLM> llm;
    std::unique_ptr<PromptTemplate> prompt_template;
    
public:
    explicit LLMChain(std::shared_ptr<LLM> llm);
    virtual void setPromptTemplate(std::unique_ptr<PromptTemplate> template_) = 0;
    virtual std::string predict(const nlohmann::json& inputs) = 0;
};

// Utility functions
namespace utils {
    // String manipulation
    std::string trimString(const std::string& str);
    std::vector<std::string> splitString(const std::string& str, char delimiter);
    
    // JSON handling
    bool validateJSON(const std::string& json_str);
    nlohmann::json parseJSON(const std::string& json_str);
    
    // Template processing
    std::string processTemplate(const std::string& template_str, 
                              const nlohmann::json& variables);
    std::vector<std::string> extractVariables(const std::string& template_str);
}

// Error handling
class LangChainError : public std::runtime_error {
public:
    explicit LangChainError(const std::string& message) 
        : std::runtime_error(message) {}
};

class TemplateError : public LangChainError {
public:
    explicit TemplateError(const std::string& message)
        : LangChainError(message) {}
};

class LLMError : public LangChainError {
public:
    explicit LLMError(const std::string& message)
        : LangChainError(message) {}
};

} // namespace langchain

#endif // LANGCHAIN_BASE_HPP