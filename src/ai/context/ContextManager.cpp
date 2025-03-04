#include "ContextManager.hpp"
#include "LangChainMemory.hpp"
#include "LangChainChains.hpp"
#include "common/showmsg.hpp"
#include "common/timer.hpp"

ContextManager::ContextManager() {
    window_config = {
        .max_messages = 100,
        .max_tokens = 2000,
        .retention_period = 86400, // 1 day
        .importance_threshold = 0.5f
    };
}

bool ContextManager::initialize(const WindowConfig& config) {
    window_config = config;

    // Initialize memory components
    recent_memory = std::make_unique<langchain::ConversationBufferWindowMemory>(
        config.max_messages
    );

    summary_memory = std::make_unique<langchain::ConversationSummaryMemory>(
        std::make_unique<langchain::SummarizationChain>(
            "Summarize the conversation context keeping key information and insights.",
            500,
            0.7f
        )
    );

    token_memory = std::make_unique<langchain::ConversationTokenBufferMemory>(
        config.max_tokens
    );

    // Initialize chain components
    summarization_chain = std::make_unique<langchain::SequentialChain>();
    summarization_chain->addChain(
        ChainFactory::createSummarizationChain()
    );

    reasoning_chain = std::make_unique<langchain::SequentialChain>();
    reasoning_chain->addChain(
        ChainFactory::createAnalysisChain()
    );
    reasoning_chain->addChain(
        ChainFactory::createDecisionChain()
    );

    ShowStatus("Context Manager initialized with window size: %d, max tokens: %d\n",
              config.max_messages, config.max_tokens);
    return true;
}

int64_t ContextManager::addContext(const std::string& content,
                                 ContextType type,
                                 const std::map<std::string, std::string>& metadata) {
    ContextEntry entry{
        .content = content,
        .type = type,
        .timestamp = time(nullptr),
        .importance_score = calculateImportance({
            .content = content,
            .type = type,
            .timestamp = time(nullptr),
            .metadata = metadata
        }),
        .metadata = metadata
    };

    // Add to appropriate storage based on type
    switch (type) {
        case ContextType::CONVERSATION:
            conversation_window.push_back(entry);
            if (conversation_window.size() > window_config.max_messages) {
                consolidateContext();
            }
            break;
        case ContextType::GAME_STATE:
            game_state_context[metadata.at("key")] = entry;
            break;
        default:
            break;
    }

    // Update memory systems
    langchain::Message msg(
        langchain::Message::Role::SYSTEM,
        content,
        nlohmann::json(metadata)
    );
    recent_memory->addMessage(msg);
    token_memory->addMessage(msg);

    return static_cast<int64_t>(entry.timestamp);
}

std::vector<ContextEntry> ContextManager::getRecentContext(
    ContextType type,
    size_t max_entries
) {
    std::vector<ContextEntry> results;
    
    switch (type) {
        case ContextType::CONVERSATION: {
            size_t count = std::min(max_entries, conversation_window.size());
            auto start = conversation_window.end() - count;
            results.insert(results.end(), start, conversation_window.end());
            break;
        }
        case ContextType::GAME_STATE: {
            for (const auto& [key, entry] : game_state_context) {
                results.push_back(entry);
                if (results.size() >= max_entries) break;
            }
            break;
        }
        default:
            break;
    }

    return results;
}

ContextManager::ContextSummary ContextManager::getContextSummary(
    ContextType type,
    time_t start_time,
    time_t end_time
) {
    std::vector<ContextEntry> entries = getRecentContext(type);
    
    // Filter by time range if specified
    if (start_time > 0 || end_time > 0) {
        entries.erase(
            std::remove_if(
                entries.begin(),
                entries.end(),
                [start_time, end_time](const ContextEntry& entry) {
                    return (start_time > 0 && entry.timestamp < start_time) ||
                           (end_time > 0 && entry.timestamp > end_time);
                }
            ),
            entries.end()
        );
    }

    // Generate summary using summary chain
    std::string summary = generateSummary(entries);

    return ContextSummary{
        .summary = summary,
        .key_points = extractKeyPoints(summary),
        .start_time = start_time > 0 ? start_time : entries.front().timestamp,
        .end_time = end_time > 0 ? end_time : entries.back().timestamp,
        .num_entries = entries.size()
    };
}

std::string ContextManager::processWithContext(
    const std::string& input,
    ContextType context_type,
    const std::map<std::string, std::string>& params
) {
    // Prepare input with context
    nlohmann::json chain_input;
    chain_input["input"] = input;
    chain_input["context"] = getRecentContext(context_type);
    chain_input["parameters"] = params;

    // Process through reasoning chain
    auto result = reasoning_chain->run(chain_input);
    
    // Store the interaction
    addContext(
        input,
        ContextType::CONVERSATION,
        {{"type", "input"}, {"result", result.dump()}}
    );

    return result["output"].get<std::string>();
}

void ContextManager::consolidateContext() {
    if (conversation_window.empty()) return;

    // Generate summary of oldest entries
    size_t consolidation_size = conversation_window.size() / 4;
    std::vector<ContextEntry> to_consolidate(
        conversation_window.begin(),
        conversation_window.begin() + consolidation_size
    );

    std::string summary = generateSummary(to_consolidate);

    // Create consolidated entry
    ContextEntry consolidated{
        .content = summary,
        .type = ContextType::CONVERSATION,
        .timestamp = to_consolidate.back().timestamp,
        .importance_score = 1.0f,  // Consolidated entries are important
        .metadata = {
            {"type", "consolidated"},
            {"entries", std::to_string(to_consolidate.size())},
            {"start_time", std::to_string(to_consolidate.front().timestamp)},
            {"end_time", std::to_string(to_consolidate.back().timestamp)}
        }
    };

    // Remove consolidated entries and add summary
    conversation_window.erase(
        conversation_window.begin(),
        conversation_window.begin() + consolidation_size
    );
    conversation_window.push_front(consolidated);

    ShowDebug("Consolidated %zu context entries into summary\n", consolidation_size);
}

float ContextManager::calculateImportance(const ContextEntry& entry) {
    // Basic importance calculation based on content and metadata
    float importance = 0.5f;  // Base importance

    // Adjust based on type
    switch (entry.type) {
        case ContextType::CONVERSATION:
            importance += 0.2f;
            break;
        case ContextType::GAME_STATE:
            importance += 0.3f;
            break;
        case ContextType::PLAYER_HISTORY:
            importance += 0.4f;
            break;
        default:
            break;
    }

    // Adjust based on metadata
    if (entry.metadata.count("priority")) {
        importance += std::stof(entry.metadata.at("priority"));
    }

    return std::min(importance, 1.0f);
}

std::string ContextManager::generateSummary(
    const std::vector<ContextEntry>& entries
) {
    nlohmann::json input;
    input["entries"] = entries;
    auto result = summarization_chain->run(input);
    return result["summary"].get<std::string>();
}

std::vector<std::string> ContextManager::extractKeyPoints(
    const std::string& summary
) {
    // Extract key points using reasoning chain
    nlohmann::json input;
    input["summary"] = summary;
    input["task"] = "extract_key_points";
    
    auto result = reasoning_chain->run(input);
    return result["key_points"].get<std::vector<std::string>>();
}

ContextManager::ContextStats ContextManager::getStats() const {
    return ContextStats{
        .total_entries = conversation_window.size() + game_state_context.size(),
        .active_contexts = getCurrentContextSize(ContextType::CONVERSATION),
        .summarized_contexts = 0,  // TODO: Track this
        .entries_by_type = {
            {ContextType::CONVERSATION, conversation_window.size()},
            {ContextType::GAME_STATE, game_state_context.size()}
        },
        .memory_usage = 0.0f  // TODO: Implement memory usage tracking
    };
}