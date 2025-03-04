#ifndef LANGCHAIN_CHAINS_HPP
#define LANGCHAIN_CHAINS_HPP

#include <string>
#include <vector>
#include <functional>
#include <nlohmann/json.hpp>
#include "LangChainMemory.hpp"

namespace langchain {

// Base chain interface
class BaseChain {
public:
    virtual ~BaseChain() = default;
    virtual nlohmann::json run(const nlohmann::json& inputs) = 0;
    virtual std::string getName() const = 0;
};

// Sequential chain for multi-step processing
class SequentialChain : public BaseChain {
private:
    std::vector<std::unique_ptr<BaseChain>> chains;
    std::map<std::string, std::string> input_mappings;
    std::map<std::string, std::string> output_mappings;

public:
    void addChain(std::unique_ptr<BaseChain> chain);
    void setInputMapping(const std::string& from, const std::string& to);
    void setOutputMapping(const std::string& from, const std::string& to);
    
    nlohmann::json run(const nlohmann::json& inputs) override;
    std::string getName() const override { return "SequentialChain"; }
};

// Chain for game state analysis
class GameStateAnalysisChain : public BaseChain {
private:
    std::string analysis_template;
    std::unique_ptr<LLMChain> llm_chain;
    std::map<std::string, float> feature_weights;

public:
    explicit GameStateAnalysisChain(std::unique_ptr<LLMChain> llm);
    
    void setFeatureWeight(const std::string& feature, float weight);
    nlohmann::json run(const nlohmann::json& inputs) override;
    std::string getName() const override { return "GameStateAnalysisChain"; }
};

// Chain for player behavior analysis
class PlayerBehaviorChain : public BaseChain {
private:
    std::unique_ptr<ConversationSummaryMemory> player_memory;
    std::string behavior_template;
    std::vector<std::string> behavior_patterns;

public:
    explicit PlayerBehaviorChain(std::unique_ptr<ConversationSummaryMemory> memory);
    
    void addBehaviorPattern(const std::string& pattern);
    nlohmann::json run(const nlohmann::json& inputs) override;
    std::string getName() const override { return "PlayerBehaviorChain"; }
};

// Chain for context summarization
class ContextSummarizationChain : public BaseChain {
private:
    size_t max_summary_length;
    float compression_ratio;
    std::string summary_template;
    std::unique_ptr<LLMChain> summarizer;

public:
    explicit ContextSummarizationChain(
        std::unique_ptr<LLMChain> summarizer,
        size_t max_length = 500,
        float compression = 0.3
    );

    nlohmann::json run(const nlohmann::json& inputs) override;
    std::string getName() const override { return "ContextSummarizationChain"; }
};

// Chain for decision making
class DecisionChain : public BaseChain {
private:
    struct Decision {
        std::string action;
        float confidence;
        std::vector<std::string> reasoning;
    };

    std::vector<std::string> decision_criteria;
    float confidence_threshold;
    std::unique_ptr<LLMChain> decision_maker;

public:
    explicit DecisionChain(
        std::unique_ptr<LLMChain> llm,
        float threshold = 0.7
    );

    void addDecisionCriterion(const std::string& criterion);
    nlohmann::json run(const nlohmann::json& inputs) override;
    std::string getName() const override { return "DecisionChain"; }
};

// Chain for context relevance scoring
class RelevanceChain : public BaseChain {
private:
    struct RelevanceScore {
        float score;
        std::string reason;
    };

    float relevance_threshold;
    std::vector<std::string> relevance_features;
    std::unique_ptr<LLMChain> scorer;

public:
    explicit RelevanceChain(
        std::unique_ptr<LLMChain> llm,
        float threshold = 0.5
    );

    void addRelevanceFeature(const std::string& feature);
    nlohmann::json run(const nlohmann::json& inputs) override;
    std::string getName() const override { return "RelevanceChain"; }
};

// Utility functions for chains
namespace chain_utils {
    nlohmann::json mergeOutputs(const std::vector<nlohmann::json>& outputs);
    bool validateInputs(const nlohmann::json& inputs, const std::vector<std::string>& required_keys);
    std::string formatTemplate(const std::string& template_str, const nlohmann::json& values);
    float calculateConfidence(const std::vector<float>& scores);
}

// Chain factory for creating common chain configurations
class ChainFactory {
public:
    static std::unique_ptr<SequentialChain> createAnalysisChain();
    static std::unique_ptr<SequentialChain> createDecisionChain();
    static std::unique_ptr<SequentialChain> createSummarizationChain();
    static std::unique_ptr<SequentialChain> createPlayerAnalysisChain();
    
    static std::unique_ptr<BaseChain> createCustomChain(
        const std::string& chain_type,
        const nlohmann::json& config
    );
};

} // namespace langchain

#endif // LANGCHAIN_CHAINS_HPP