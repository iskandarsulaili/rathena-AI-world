#ifndef NETWORK_CONFIG_HPP
#define NETWORK_CONFIG_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include "ConfigManager.hpp"

namespace rathena {
namespace p2p {

class NetworkConfig {
public:
    struct Thresholds {
        int latency_warning;
        int latency_critical;
        int latency_max;
        int bandwidth_min;
        int bandwidth_optimal;
        int bandwidth_warning;
        float packet_loss_warning;
        float packet_loss_critical;
        float packet_loss_max;
        int jitter_warning;
        int jitter_critical;
    };

    struct ScoringWeights {
        float latency;
        float bandwidth;
        float stability;
        float vps_bonus;
        float dedicated_bonus;
    };

    struct TestingParams {
        int ping_interval;
        int full_test_interval;
        int bandwidth_test_interval;
        int ping_samples;
        int bandwidth_duration;
        int jitter_samples;
    };

    struct QualityZone {
        int max_latency;
        int min_bandwidth;
        float max_packet_loss;
    };

    static NetworkConfig& getInstance() {
        static NetworkConfig instance;
        return instance;
    }

    bool load(const std::string& config_path = "conf/p2p/network_quality.conf");
    void reload();

    // Threshold getters
    const Thresholds& getThresholds() const { return thresholds; }
    const ScoringWeights& getScoringWeights() const { return scoring; }
    const TestingParams& getTestingParams() const { return testing; }
    const QualityZone& getZoneRequirements(const std::string& zone) const;

    // Utility functions
    bool isLatencyCritical(float latency) const {
        return latency > thresholds.latency_critical;
    }

    bool isBandwidthCritical(float bandwidth) const {
        return bandwidth < thresholds.bandwidth_warning;
    }

    float calculateQualityScore(float latency, float bandwidth, float packet_loss) const;

private:
    NetworkConfig() = default;
    ~NetworkConfig() = default;
    NetworkConfig(const NetworkConfig&) = delete;
    NetworkConfig& operator=(const NetworkConfig&) = delete;

    bool parseThresholds(const ConfigSection& section);
    bool parseScoring(const ConfigSection& section);
    bool parseTestingParams(const ConfigSection& section);
    bool parseQualityZones(const ConfigSection& section);

    Thresholds thresholds;
    ScoringWeights scoring;
    TestingParams testing;
    std::unordered_map<std::string, QualityZone> zone_requirements;
};

// Global accessor
inline NetworkConfig& g_NetworkConfig() {
    return NetworkConfig::getInstance();
}

} // namespace p2p
} // namespace rathena

#endif // NETWORK_CONFIG_HPP