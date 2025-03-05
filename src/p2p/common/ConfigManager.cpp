#include "ConfigManager.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>

namespace rathena {
namespace p2p {

bool ConfigManager::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open config file: " << filepath << std::endl;
        return false;
    }

    clear();
    std::string line;
    std::string current_section;

    try {
        while (std::getline(file, line)) {
            if (!parseLine(line, current_section)) {
                std::cerr << "Error parsing line: " << line << std::endl;
                return false;
            }
        }
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading config: " << e.what() << std::endl;
        return false;
    }
}

ConfigSection* ConfigManager::getSection(const std::string& section_name) {
    auto it = sections_.find(section_name);
    return it != sections_.end() ? &it->second : nullptr;
}

bool ConfigManager::hasSection(const std::string& section_name) const {
    return sections_.find(section_name) != sections_.end();
}

std::vector<std::string> ConfigManager::getSectionNames() const {
    std::vector<std::string> names;
    names.reserve(sections_.size());
    for (const auto& section : sections_) {
        names.push_back(section.first);
    }
    return names;
}

void ConfigManager::clear() {
    sections_.clear();
}

bool ConfigManager::parseLine(const std::string& line, std::string& current_section) {
    // Skip empty lines and comments
    std::string trimmed = trim(line);
    if (trimmed.empty() || trimmed[0] == '#' || trimmed[0] == ';') {
        return true;
    }

    // Section header
    if (trimmed[0] == '[') {
        size_t end = trimmed.find(']');
        if (end == std::string::npos) {
            return false;
        }
        current_section = trim(trimmed.substr(1, end - 1));
        return true;
    }

    // Key-value pair
    if (!current_section.empty()) {
        size_t delimiter = trimmed.find('=');
        if (delimiter != std::string::npos) {
            std::string key = trim(trimmed.substr(0, delimiter));
            std::string value = trim(trimmed.substr(delimiter + 1));
            
            // Handle quoted values
            if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
                value = value.substr(1, value.size() - 2);
            }
            
            addKeyValue(current_section, key, value);
        }
    }

    return true;
}

void ConfigManager::addKeyValue(const std::string& section, 
                              const std::string& key, 
                              const std::string& value) {
    sections_[section][key] = value;
}

std::string ConfigManager::trim(const std::string& str) {
    const std::string whitespace = " \t\n\r";
    size_t start = str.find_first_not_of(whitespace);
    if (start == std::string::npos) {
        return "";
    }
    size_t end = str.find_last_not_of(whitespace);
    return str.substr(start, end - start + 1);
}

std::vector<std::string> ConfigManager::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    
    while (std::getline(ss, token, delimiter)) {
        token = trim(token);
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    
    return tokens;
}

} // namespace p2p
} // namespace rathena