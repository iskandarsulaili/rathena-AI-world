#include "ai_agent_base.hpp"
#include "ai_config.hpp"
#include <ctime>

namespace ai {

AIAgentBase::AIAgentBase(const AIConfig& config) : 
    config(config),
    agentId(config.agentId),
    agentName(config.agentName),
    isActive(config.isEnabled) {
    
    // Initialize with configuration
    if (agentId == 0) {
        // Generate unique ID if not provided
        agentId = static_cast<int>(std::time(nullptr));
    }

    if (agentName.empty()) {
        agentName = "Agent_" + std::to_string(agentId);
    }
}

} // namespace ai