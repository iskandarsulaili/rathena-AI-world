#include "ai_config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <stdexcept>

namespace ai {

AIConfig::AIConfig(const std::string& name) : agentName(name) {}

bool AIConfig::getCustomParamBool(const std::string& key, bool defaultValue) const {
    auto it = customParams.find(key);
    if (it == customParams.end()) {
        return defaultValue;
    }
    
    std::string value = it->second;
    std::transform(value.begin(), value.end(), value.begin(), ::tolower);
    return value == "true" || value == "1" || value == "yes";
}

int AIConfig::getCustomParamInt(const std::string& key, int defaultValue) const {
    auto it = customParams.find(key);
    if (it == customParams.end()) {
        return defaultValue;
    }
    
    try {
        return std::stoi(it->second);
    } catch (...) {
        return defaultValue;
    }
}

float AIConfig::getCustomParamFloat(const std::string& key, float defaultValue) const {
    auto it = customParams.find(key);
    if (it == customParams.end()) {
        return defaultValue;
    }
    
    try {
        return std::stof(it->second);
    } catch (...) {
        return defaultValue;
    }
}

std::string AIConfig::getCustomParamString(const std::string& key, const std::string& defaultValue) const {
    auto it = customParams.find(key);
    return it != customParams.end() ? it->second : defaultValue;
}

void AIConfig::setCustomParam(const std::string& key, const std::string& value) {
    customParams[key] = value;
}

bool AIConfig::hasCustomParam(const std::string& key) const {
    return customParams.find(key) != customParams.end();
}

bool AIConfig::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        // Skip comments and empty lines
        if (line.empty() || line[0] == '#' || line[0] == ';') {
            continue;
        }

        // Find key-value separator
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            continue;
        }

        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);

        // Trim whitespace
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);

        // Process known fields
        if (key == "agent_id") agentId = std::stoi(value);
        else if (key == "agent_name") agentName = value;
        else if (key == "agent_type") agentType = value;
        else if (key == "enabled") isEnabled = (value == "true" || value == "1");
        else if (key == "map_id") mapId = std::stoi(value);
        else if (key == "x") x = std::stoi(value);
        else if (key == "y") y = std::stoi(value);
        else if (key == "move_speed") moveSpeed = std::stof(value);
        else if (key == "provider_type") providerType = value;
        else if (key == "model_name") modelName = value;
        else if (key == "context_window") contextWindow = std::stoi(value);
        else if (key == "temperature") temperature = std::stof(value);
        else if (key == "use_persistent_memory") usePersistentMemory = (value == "true" || value == "1");
        else if (key == "memory_provider") memoryProvider = value;
        else if (key == "embedding_model") embeddingModel = value;
        else if (key == "update_interval") updateInterval = std::stoi(value);
        else if (key == "response_timeout") responseTimeout = std::stoi(value);
        else {
            // Store unknown fields as custom parameters
            customParams[key] = value;
        }
    }

    return true;
}

bool AIConfig::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    file << "# AI Agent Configuration\n\n";

    // Write core settings
    file << "agent_id = " << agentId << "\n";
    file << "agent_name = " << agentName << "\n";
    file << "agent_type = " << agentType << "\n";
    file << "enabled = " << (isEnabled ? "true" : "false") << "\n\n";

    // Write location settings
    file << "map_id = " << mapId << "\n";
    file << "x = " << x << "\n";
    file << "y = " << y << "\n";
    file << "move_speed = " << moveSpeed << "\n\n";

    // Write AI settings
    file << "provider_type = " << providerType << "\n";
    file << "model_name = " << modelName << "\n";
    file << "context_window = " << contextWindow << "\n";
    file << "temperature = " << temperature << "\n\n";

    // Write memory settings
    file << "use_persistent_memory = " << (usePersistentMemory ? "true" : "false") << "\n";
    file << "memory_provider = " << memoryProvider << "\n";
    file << "embedding_model = " << embeddingModel << "\n\n";

    // Write timing settings
    file << "update_interval = " << updateInterval << "\n";
    file << "response_timeout = " << responseTimeout << "\n\n";

    // Write custom parameters
    if (!customParams.empty()) {
        file << "# Custom parameters\n";
        for (const auto& param : customParams) {
            file << param.first << " = " << param.second << "\n";
        }
    }

    return true;
}

} // namespace ai