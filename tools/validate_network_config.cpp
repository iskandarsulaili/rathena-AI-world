#include "../src/p2p/common/NetworkConfig.hpp"
#include "../src/p2p/common/ConfigManager.hpp"
#include <iostream>
#include <iomanip>

using namespace rathena::p2p;

// Color codes for output formatting
const std::string RED = "\033[31m";
const std::string GREEN = "\033[32m";
const std::string YELLOW = "\033[33m";
const std::string RESET = "\033[0m";
const std::string BOLD = "\033[1m";

struct ValidationResult {
    bool success;
    std::string message;
    std::string section;
    std::string key;
    std::string expected;
    std::string actual;
};

class NetworkConfigValidator {
public:
    std::vector<ValidationResult> validateConfig(const std::string& config_path) {
        std::vector<ValidationResult> results;
        NetworkConfig& config = NetworkConfig::getInstance();
        
        if (!config.load(config_path)) {
            results.push_back({
                false,
                "Failed to load configuration file",
                "",
                "",
                "Valid configuration file",
                "File not found or invalid"
            });
            return results;
        }

        // Validate thresholds
        validateThresholds(config.getThresholds(), results);

        // Validate scoring weights
        validateScoringWeights(config.getScoringWeights(), results);

        // Validate testing parameters
        validateTestingParams(config.getTestingParams(), results);

        // Validate quality zones
        validateQualityZones(config, results);

        return results;
    }

private:
    void validateThresholds(const NetworkConfig::Thresholds& thresholds, 
                          std::vector<ValidationResult>& results) {
        // Latency validations
        if (thresholds.latency_warning >= thresholds.latency_critical) {
            results.push_back({
                false,
                "Warning threshold should be lower than critical threshold",
                "thresholds",
                "latency_warning",
                "< " + std::to_string(thresholds.latency_critical),
                std::to_string(thresholds.latency_warning)
            });
        }

        // Bandwidth validations
        if (thresholds.bandwidth_warning >= thresholds.bandwidth_min) {
            results.push_back({
                false,
                "Warning bandwidth should be lower than minimum required bandwidth",
                "thresholds",
                "bandwidth_warning",
                "< " + std::to_string(thresholds.bandwidth_min),
                std::to_string(thresholds.bandwidth_warning)
            });
        }

        // Packet loss validations
        if (thresholds.packet_loss_warning >= thresholds.packet_loss_critical) {
            results.push_back({
                false,
                "Warning packet loss should be lower than critical threshold",
                "thresholds",
                "packet_loss_warning",
                "< " + std::to_string(thresholds.packet_loss_critical),
                std::to_string(thresholds.packet_loss_warning)
            });
        }
    }

    void validateScoringWeights(const NetworkConfig::ScoringWeights& weights,
                              std::vector<ValidationResult>& results) {
        float sum = weights.latency + weights.bandwidth + weights.stability;
        if (std::abs(sum - 1.0f) > 0.001f) {
            results.push_back({
                false,
                "Scoring weights must sum to 1.0",
                "scoring",
                "weights_sum",
                "1.0",
                std::to_string(sum)
            });
        }

        if (weights.vps_bonus < 1.0f) {
            results.push_back({
                false,
                "VPS bonus multiplier should be >= 1.0",
                "scoring",
                "vps_bonus",
                ">= 1.0",
                std::to_string(weights.vps_bonus)
            });
        }
    }

    void validateTestingParams(const NetworkConfig::TestingParams& params,
                             std::vector<ValidationResult>& results) {
        if (params.ping_interval < 5) {
            results.push_back({
                false,
                "Ping interval too low, may cause excessive network traffic",
                "testing",
                "ping_interval",
                ">= 5",
                std::to_string(params.ping_interval)
            });
        }

        if (params.bandwidth_test_interval < 300) {
            results.push_back({
                false,
                "Bandwidth test interval too low, may impact performance",
                "testing",
                "bandwidth_test_interval",
                ">= 300",
                std::to_string(params.bandwidth_test_interval)
            });
        }
    }

    void validateQualityZones(const NetworkConfig& config,
                            std::vector<ValidationResult>& results) {
        // Validate premium zone requirements
        auto premium = config.getZoneRequirements("premium");
        if (premium.max_latency > 50) {
            results.push_back({
                false,
                "Premium zone latency threshold too high",
                "geographic",
                "premium.max_latency",
                "<= 50",
                std::to_string(premium.max_latency)
            });
        }
    }
};

void printResults(const std::vector<ValidationResult>& results) {
    std::cout << BOLD << "\nNetwork Configuration Validation Results\n"
              << std::string(40, '=') << RESET << "\n\n";

    int issues = 0;
    for (const auto& result : results) {
        if (!result.success) {
            issues++;
            std::cout << RED << "ERROR" << RESET << ": " << result.message << "\n"
                     << "Section: " << result.section << "\n"
                     << "Key: " << result.key << "\n"
                     << "Expected: " << result.expected << "\n"
                     << "Actual: " << result.actual << "\n\n";
        }
    }

    if (issues == 0) {
        std::cout << GREEN << "Configuration validation passed! No issues found.\n" << RESET;
    } else {
        std::cout << RED << "Found " << issues << " issue(s) in configuration.\n" << RESET;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <config_file_path>\n";
        return 1;
    }

    NetworkConfigValidator validator;
    auto results = validator.validateConfig(argv[1]);
    printResults(results);

    return results.empty() ? 0 : 1;
}