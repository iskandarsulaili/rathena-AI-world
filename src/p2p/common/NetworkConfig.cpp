#include "NetworkConfig.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>

namespace rathena {
namespace p2p {

bool NetworkConfig::load(const std::string& config_path) {
    try {
        std::ifstream file(config_path);
        if (!file.is_open()) {
            std::cerr << "Failed to open config file: " << config_path << std::endl;
            return false;
        }

        std::string line, section;
        ConfigSection current_section;

        while (std::getline(file, line)) {
            // Skip comments and empty lines
            if (line.empty() || line[0] == '#' || line[0] == ';') {
                continue;
            }

            // Check for section header
            if (line[0] == '[') {
                if (!section.empty()) {
                    // Process previous section
                    if (!parseSection(section, current_section)) {
                        return false;
                    }
                }
                size_t end = line.find(']');
                if (end == std::string::npos) {
                    std::cerr << "Invalid section header: " << line << std::endl;
                    return false;
                }
                section = line.substr(1, end - 1);
                current_section.clear();
                continue;
            }

            // Parse key-value pairs
            size_t delimiter = line.find('=');
            if (delimiter == std::string::npos) {
                continue;
            }

            std::string key = trim(line.substr(0, delimiter));
            std::string value = trim(line.substr(delimiter + 1));
            current_section[key] = value;
        }

        // Process last section
        if (!section.empty()) {
            if (!parseSection(section, current_section)) {
                return false;
            }
        }

        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading network config: " << e.what() << std::endl;
        return false;
    }
}

void NetworkConfig::reload() {
    load(); // Reload with default path
}

bool NetworkConfig::parseSection(const std::string& section, const ConfigSection& values) {
    try {
        if (section == "thresholds") {
            return parseThresholds(values);
        } else if (section == "scoring") {
            return parseScoring(values);
        } else if (section == "testing") {
            return parseTestingParams(values);
        } else if (section == "geographic") {
            return parseQualityZones(values);
        }
        return true; // Skip unknown sections
    } catch (const std::exception& e) {
        std::cerr << "Error parsing section " << section << ": " << e.what() << std::endl;
        return false;
    }
}

bool NetworkConfig::parseThresholds(const ConfigSection& section) {
    thresholds.latency_warning = getIntValue(section, "latency_warning", 100);
    thresholds.latency_critical = getIntValue(section, "latency_critical", 200);
    thresholds.latency_max = getIntValue(section, "latency_max", 300);
    thresholds.bandwidth_min = getIntValue(section, "bandwidth_min", 5);
    thresholds.bandwidth_optimal = getIntValue(section, "bandwidth_optimal", 10);
    thresholds.bandwidth_warning = getIntValue(section, "bandwidth_warning", 3);
    thresholds.packet_loss_warning = getFloatValue(section, "packet_loss_warning", 1.0f);
    thresholds.packet_loss_critical = getFloatValue(section, "packet_loss_critical", 2.0f);
    thresholds.packet_loss_max = getFloatValue(section, "packet_loss_max", 5.0f);
    thresholds.jitter_warning = getIntValue(section, "jitter_warning", 20);
    thresholds.jitter_critical = getIntValue(section, "jitter_critical", 50);
    return true;
}

bool NetworkConfig::parseScoring(const ConfigSection& section) {
    scoring.latency = getFloatValue(section, "latency_weight", 0.4f);
    scoring.bandwidth = getFloatValue(section, "bandwidth_weight", 0.3f);
    scoring.stability = getFloatValue(section, "stability_weight", 0.3f);
    scoring.vps_bonus = getFloatValue(section, "vps_bonus", 1.2f);
    scoring.dedicated_bonus = getFloatValue(section, "dedicated_bonus", 1.1f);

    // Validate weights sum to 1.0
    float sum = scoring.latency + scoring.bandwidth + scoring.stability;
    if (std::abs(sum - 1.0f) > 0.001f) {
        std::cerr << "Warning: Scoring weights do not sum to 1.0 (" << sum << ")" << std::endl;
        return false;
    }
    return true;
}

bool NetworkConfig::parseTestingParams(const ConfigSection& section) {
    testing.ping_interval = getIntValue(section, "ping_interval", 30);
    testing.full_test_interval = getIntValue(section, "full_test_interval", 300);
    testing.bandwidth_test_interval = getIntValue(section, "bandwidth_test_interval", 3600);
    testing.ping_samples = getIntValue(section, "ping_samples", 10);
    testing.bandwidth_duration = getIntValue(section, "bandwidth_duration", 10);
    testing.jitter_samples = getIntValue(section, "jitter_samples", 100);
    return true;
}

bool NetworkConfig::parseQualityZones(const ConfigSection& section) {
    auto zones = parseArray(getValue(section, "quality_zones", "[]"));
    
    for (const auto& zone : zones) {
        QualityZone requirements;
        auto zone_config = section.find("zone_requirements." + zone);
        if (zone_config != section.end()) {
            requirements.max_latency = getIntValue(section, zone + ".max_latency", 200);
            requirements.min_bandwidth = getIntValue(section, zone + ".min_bandwidth", 5);
            requirements.max_packet_loss = getFloatValue(section, zone + ".max_packet_loss", 1.0f);
            zone_requirements[zone] = requirements;
        }
    }
    return true;
}

const NetworkConfig::QualityZone& NetworkConfig::getZoneRequirements(const std::string& zone) const {
    static const QualityZone default_zone = {200, 5, 1.0f};
    auto it = zone_requirements.find(zone);
    return it != zone_requirements.end() ? it->second : default_zone;
}

float NetworkConfig::calculateQualityScore(float latency, float bandwidth, float packet_loss) const {
    // Normalize metrics
    float latency_score = std::max(0.0f, 100.0f - (latency / thresholds.latency_max) * 100.0f);
    float bandwidth_score = std::min(100.0f, (bandwidth / thresholds.bandwidth_optimal) * 100.0f);
    float stability_score = std::max(0.0f, 100.0f - (packet_loss / thresholds.packet_loss_max) * 100.0f);

    // Apply weights
    return latency_score * scoring.latency +
           bandwidth_score * scoring.bandwidth +
           stability_score * scoring.stability;
}

// Utility functions
std::string NetworkConfig::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

int NetworkConfig::getIntValue(const ConfigSection& section, const std::string& key, int default_value) {
    auto it = section.find(key);
    if (it == section.end()) return default_value;
    return std::stoi(it->second);
}

float NetworkConfig::getFloatValue(const ConfigSection& section, const std::string& key, float default_value) {
    auto it = section.find(key);
    if (it == section.end()) return default_value;
    return std::stof(it->second);
}

std::string NetworkConfig::getValue(const ConfigSection& section, const std::string& key, const std::string& default_value) {
    auto it = section.find(key);
    return it != section.end() ? it->second : default_value;
}

std::vector<std::string> NetworkConfig::parseArray(const std::string& str) {
    std::vector<std::string> result;
    std::stringstream ss(str.substr(1, str.length() - 2)); // Remove [ ]
    std::string item;
    while (std::getline(ss, item, ',')) {
        result.push_back(trim(item));
    }
    return result;
}

} // namespace p2p
} // namespace rathena