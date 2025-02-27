#ifndef AI_REQUEST_HPP
#define AI_REQUEST_HPP

#include <string>
#include <vector>
#include <map>

namespace ai {

class AIRequest {
public:
    AIRequest() = default;
    explicit AIRequest(const std::string& prompt);

    // Request content
    void setPrompt(const std::string& prompt) { this->prompt = prompt; }
    std::string getPrompt() const { return prompt; }

    // Context management
    void addContext(const std::string& key, const std::string& value);
    void addContext(const std::string& key, int value);
    void addContext(const std::string& key, float value);
    void addContext(const std::string& key, bool value);
    
    bool hasContext(const std::string& key) const;
    std::string getContext(const std::string& key) const;
    
    // System messages for AI
    void addSystemMessage(const std::string& message);
    const std::vector<std::string>& getSystemMessages() const;

    // Request parameters
    void setTemperature(float temp) { temperature = temp; }
    float getTemperature() const { return temperature; }
    
    void setMaxTokens(int tokens) { maxTokens = tokens; }
    int getMaxTokens() const { return maxTokens; }

    // Request metadata
    void setRequestId(const std::string& id) { requestId = id; }
    std::string getRequestId() const { return requestId; }
    
    void setAgentId(int id) { agentId = id; }
    int getAgentId() const { return agentId; }

private:
    std::string prompt;
    std::map<std::string, std::string> context;
    std::vector<std::string> systemMessages;
    std::string requestId;
    int agentId = 0;
    float temperature = 0.7f;
    int maxTokens = 512;
};

} // namespace ai

#endif // AI_REQUEST_HPP