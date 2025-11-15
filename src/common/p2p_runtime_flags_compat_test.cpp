// p2p_runtime_flags_compat_test.cpp
// Test/verification: P2P runtime flags and fallback do not break legacy, AI, or ML logic

#include "p2p_runtime_flags.hpp"
#include <cassert>
#include <iostream>
#include <string>

// Simulate legacy, AI, and ML service logic
void legacy_gameplay_logic(const std::string& service, const std::string& zone) {
    auto& flags = P2PRuntimeFlags::instance();
    // Should always work regardless of P2P state
    if (!flags.is_service_p2p_enabled(service) || flags.is_service_in_fallback(service)) {
        std::cout << "[LEGACY] " << service << " running in SERVER-ONLY mode for zone " << zone << std::endl;
    } else {
        std::cout << "[LEGACY] " << service << " running with P2P enabled for zone " << zone << std::endl;
    }
}

void ai_logic(const std::string& service, const std::string& zone) {
    auto& flags = P2PRuntimeFlags::instance();
    // Should always work regardless of P2P state
    if (!flags.is_zone_p2p_enabled(zone) || flags.is_zone_in_fallback(zone)) {
        std::cout << "[AI] " << service << " running in SERVER-ONLY mode for zone " << zone << std::endl;
    } else {
        std::cout << "[AI] " << service << " running with P2P enabled for zone " << zone << std::endl;
    }
}

void ml_logic(const std::string& service, const std::string& zone) {
    auto& flags = P2PRuntimeFlags::instance();
    // Should always work regardless of P2P state
    if (!flags.is_service_p2p_enabled(service) || !flags.is_zone_p2p_enabled(zone)) {
        std::cout << "[ML] " << service << " running in SERVER-ONLY mode for zone " << zone << std::endl;
    } else {
        std::cout << "[ML] " << service << " running with P2P enabled for zone " << zone << std::endl;
    }
}

void test_compatibility() {
    auto& flags = P2PRuntimeFlags::instance();
    flags.reset();

    // Enable P2P for all
    flags.set_service_p2p_enabled("map-server", true);
    flags.set_zone_p2p_enabled("prontera", true);

    legacy_gameplay_logic("map-server", "prontera");
    ai_logic("aiworld", "prontera");
    ml_logic("aiworld", "prontera");

    // Disable P2P for service
    flags.set_service_p2p_enabled("map-server", false);
    legacy_gameplay_logic("map-server", "prontera");
    ai_logic("aiworld", "prontera");
    ml_logic("aiworld", "prontera");

    // Enable P2P for service, disable for zone
    flags.set_service_p2p_enabled("map-server", true);
    flags.set_zone_p2p_enabled("prontera", false);
    legacy_gameplay_logic("map-server", "prontera");
    ai_logic("aiworld", "prontera");
    ml_logic("aiworld", "prontera");

    // Fallback for service
    flags.set_service_p2p_enabled("map-server", true);
    flags.set_zone_p2p_enabled("prontera", true);
    flags.force_service_fallback("map-server");
    legacy_gameplay_logic("map-server", "prontera");
    ai_logic("aiworld", "prontera");
    ml_logic("aiworld", "prontera");

    // Fallback for zone
    flags.reset();
    flags.set_service_p2p_enabled("map-server", true);
    flags.set_zone_p2p_enabled("prontera", true);
    flags.force_zone_fallback("prontera");
    legacy_gameplay_logic("map-server", "prontera");
    ai_logic("aiworld", "prontera");
    ml_logic("aiworld", "prontera");

    std::cout << "[P2PRuntimeFlags] Compatibility test completed." << std::endl;
}

int main() {
    test_compatibility();
    return 0;
}