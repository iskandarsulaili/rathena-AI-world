#ifndef CONFIG_MANAGER_HPP
#define CONFIG_MANAGER_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

namespace rathena {
namespace p2p {

// Type alias for section data
using ConfigSection = std::unordered_map<std::string, std::string>;

class ConfigManager {
public:
    ConfigManager() = default;
    ~ConfigManager() = default;

    // Load configuration from file
    bool loadFromFile(const std::string& filepath);
    
    // Get section data
    ConfigSection* getSection(const std::string& section_name);
    
    // Get typed values with defaults
    template<typename T>
    T getValue(const std::string& section, const std::string& key, const T& default_value) const;
    
    // Check if section exists
    bool hasSection(const std::string& section_name) const;
    
    // Get all section names
    std::vector<std::string> getSectionNames() const;
    
    // Clear all configuration data
    void clear();

private:
    std::unordered_map<std::string, ConfigSection> sections_;
    
    // Utility functions
    static std::string trim(const std::string& str);
    static std::vector<std::string> split(const std::string& str, char delimiter);
    
    // Parse helpers
    bool parseLine(const std::string& line, std::string& current_section);
    void addKeyValue(const std::string& section, const std::string& key, const std::string& value);
};

// Template specializations for common types
template<>
inline std::string ConfigManager::getValue<std::string>(
    const std::string& section, const std::string& key, const std::string& default_value) const {
    auto sect_it = sections_.find(section);
    if (sect_it == sections_.end()) return default_value;
    
    auto val_it = sect_it->second.find(key);
    if (val_it == sect_it->second.end()) return default_value;
    
    return val_it->second;
}

template<>
inline int ConfigManager::getValue<int>(
    const std::string& section, const std::string& key, const int& default_value) const {
    std::string str_val = getValue<std::string>(section, key, "");
    if (str_val.empty()) return default_value;
    try {
        return std::stoi(str_val);
    } catch (...) {
        return default_value;
    }
}

template<>
inline float ConfigManager::getValue<float>(
    const std::string& section, const std::string& key, const float& default_value) const {
    std::string str_val = getValue<std::string>(section, key, "");
    if (str_val.empty()) return default_value;
    try {
        return std::stof(str_val);
    } catch (...) {
        return default_value;
    }
}

template<>
inline bool ConfigManager::getValue<bool>(
    const std::string& section, const std::string& key, const bool& default_value) const {
    std::string str_val = getValue<std::string>(section, key, "");
    if (str_val.empty()) return default_value;
    
    std::transform(str_val.begin(), str_val.end(), str_val.begin(), ::tolower);
    return str_val == "true" || str_val == "1" || str_val == "yes" || str_val == "on";
}

} // namespace p2p
} // namespace rathena

#endif // CONFIG_MANAGER_HPP