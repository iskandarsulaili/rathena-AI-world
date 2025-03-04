#ifndef LANGCHAIN_CHAIN_HPP
#define LANGCHAIN_CHAIN_HPP

#include <string>
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>
#include "base.hpp"
#include "schema.hpp"
#include "memory.hpp"

namespace langchain {

// Forward declarations
class LLM;
class PromptTemplate;
class Memory;

// Chain interface
class Chain {
public:
    virtual ~Chain() = default;
    virtual nlohmann::json run(const nlohmann::json& inputs) = 0;
    virtual std::vector<std::string> getInputKeys() const = 0;
    virtual std::vector<std::string> getOutputKeys() const = 0;
};

// LLM Chain implementation
class LLMChain : public Chain {
protected:
    std::shared_ptr<LLM> llm;
    std::unique_ptr<PromptTemplate> prompt_template;
    std::vector<std::string> output_keys;
    std::unique_ptr<Memory> memory;

public:
    explicit LLMChain(std::shared_ptr<LLM> model);
    
    void setPromptTemplate(std::unique_ptr<PromptTemplate> template_);
    void setMemory(std::unique_ptr<Memory> mem);
    
    nlohmann::json run(const nlohmann::json& inputs) override;
    std::vector<std::string> getInputKeys() const override;
    std::vector<std::string> getOutputKeys() const override;
    
    virtual std::string predict(const nlohmann::json& inputs);
    virtual nlohmann::json predictAndParse(const nlohmann::json& inputs);
};

// Sequential chain for multiple steps
class SequentialChain : public Chain {
private:
    std::vector<std::unique_ptr<Chain>> chains;
    std::map<std::string, std::string> input_mappings;
    std::map<std::string, std::string> output_mappings;
    bool return_intermediate;

public:
    explicit SequentialChain(bool return_intermediate = false);
    
    void addChain(std::unique_ptr<Chain> chain);
    void setInputMapping(const std::string& from, const std::string& to);
    void setOutputMapping(const std::string& from, const std::string& to);
    
    nlohmann::json run(const nlohmann::json& inputs) override;
    std::vector<std::string> getInputKeys() const override;
    std::vector<std::string> getOutputKeys() const override;
};

// Transformation chain for data processing
class TransformationChain : public Chain {
private:
    std::function<nlohmann::json(const nlohmann::json&)> transform_func;
    std::vector<std::string> input_keys;
    std::vector<std::string> output_keys;

public:
    TransformationChain(
        std::function<nlohmann::json(const nlohmann::json&)> func,
        std::vector<std::string> inputs,
        std::vector<std::string> outputs
    );
    
    nlohmann::json run(const nlohmann::json& inputs) override;
    std::vector<std::string> getInputKeys() const override;
    std::vector<std::string> getOutputKeys() const override;
};

// Router chain for conditional processing
class RouterChain : public Chain {
private:
    std::map<std::string, std::unique_ptr<Chain>> routes;
    std::unique_ptr<LLMChain> router;
    std::string default_route;

public:
    explicit RouterChain(std::unique_ptr<LLMChain> router_chain);
    
    void addRoute(const std::string& name, std::unique_ptr<Chain> chain);
    void setDefaultRoute(const std::string& name);
    
    nlohmann::json run(const nlohmann::json& inputs) override;
    std::vector<std::string> getInputKeys() const override;
    std::vector<std::string> getOutputKeys() const override;
};

// Chain factory for common patterns
class ChainFactory {
public:
    static std::unique_ptr<LLMChain> createSimpleLLMChain(
        std::shared_ptr<LLM> llm,
        const std::string& template_str
    );
    
    static std::unique_ptr<SequentialChain> createSequentialChain(
        std::vector<std::unique_ptr<Chain>> chains,
        bool return_intermediate = false
    );
    
    static std::unique_ptr<RouterChain> createRouterChain(
        std::shared_ptr<LLM> llm,
        const std::string& router_template
    );
};

// Chain utilities
namespace chain_utils {
    nlohmann::json validateInputs(
        const nlohmann::json& inputs,
        const std::vector<std::string>& required_keys
    );
    
    nlohmann::json mergeOutputs(
        const std::vector<nlohmann::json>& outputs
    );
    
    nlohmann::json applyMappings(
        const nlohmann::json& data,
        const std::map<std::string, std::string>& mappings
    );
}

} // namespace langchain

#endif // LANGCHAIN_CHAIN_HPP