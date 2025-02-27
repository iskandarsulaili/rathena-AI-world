#ifndef AI_RESPONSE_HPP
#define AI_RESPONSE_HPP

#include <string>
#include <vector>
#include <map>

namespace ai {

class AIResponse {
public:
    AIResponse() = default;
    explicit AIResponse(const std::string& content);

    // Response content
    void setContent(const std::string& content) { this->content = content; }
    std::string getContent() const { return content; }

    // Response status
    enum class Status {
        SUCCESS,
        ERROR,
        TIMEOUT,
        INVALID_REQUEST
    };

    void setStatus(Status status) { this->status = status; }
    Status getStatus() const { return status; }

    // Error handling
    void setErrorMessage(const std::string& message) { errorMessage = message; }
    std::string getErrorMessage() const { return errorMessage; }

    // Response metadata
    void setRequestId(const std::string& id) { requestId = id; }
    std::string getRequestId() const { return requestId; }

    void setAgentId(int id) { agentId = id; }
    int getAgentId() const { return agentId; }

    // Emotion/emote support for visual feedback
    void setEmoteId(int id) { emoteId = id; }
    int getEmoteId() const { return emoteId; }
    bool shouldShowEmote() const { return emoteId > 0; }

    // Performance metrics
    void setProcessingTime(float time) { processingTime = time; }
    float getProcessingTime() const { return processingTime; }

    void setTokenCount(int count) { tokenCount = count; }
    int getTokenCount() const { return tokenCount; }

    // Response actions
    struct Action {
        std::string type;
        std::map<std::string, std::string> params;
    };

    void addAction(const Action& action) { actions.push_back(action); }
    const std::vector<Action>& getActions() const { return actions; }

    // Custom parameters
    void setParam(const std::string& key, const std::string& value) {
        customParams[key] = value;
    }

    std::string getParam(const std::string& key, const std::string& defaultValue = "") const {
        auto it = customParams.find(key);
        return it != customParams.end() ? it->second : defaultValue;
    }

    bool hasParam(const std::string& key) const {
        return customParams.find(key) != customParams.end();
    }

private:
    std::string content;
    Status status = Status::SUCCESS;
    std::string errorMessage;
    std::string requestId;
    int agentId = 0;
    int emoteId = 0;
    float processingTime = 0.0f;
    int tokenCount = 0;
    std::vector<Action> actions;
    std::map<std::string, std::string> customParams;
};

} // namespace ai

#endif // AI_RESPONSE_HPP