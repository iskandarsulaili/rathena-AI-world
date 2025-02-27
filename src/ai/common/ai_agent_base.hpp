#ifndef AI_AGENT_BASE_HPP
#define AI_AGENT_BASE_HPP

#include "ai_types.hpp"
#include "ai_request.hpp"
#include "ai_response.hpp"
#include "ai_config.hpp"

namespace ai {

class AIAgentBase {
public:
    explicit AIAgentBase(const AIConfig& config);
    virtual ~AIAgentBase() = default;

    // Core AI agent interface
    virtual void update() = 0;
    virtual AIResponse processRequest(const AIRequest& request) = 0;
    virtual AIRequest createRequest() = 0;

protected:
    const AIConfig& config;
    int agentId;
    std::string agentName;
    bool isActive;
};

} // namespace ai

#endif // AI_AGENT_BASE_HPP