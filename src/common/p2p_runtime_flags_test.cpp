// p2p_runtime_flags_test.cpp
// Test/verification hooks for P2PRuntimeFlags

#include "p2p_runtime_flags.hpp"
#include <cassert>
#include <iostream>

void test_p2p_runtime_flags() {
    auto& flags = P2PRuntimeFlags::instance();

    // Reset all
    flags.reset();

    // Service-level enable/disable
    flags.set_service_p2p_enabled("map-server", true);
    assert(flags.is_service_p2p_enabled("map-server"));
    flags.set_service_p2p_enabled("map-server", false);
    assert(!flags.is_service_p2p_enabled("map-server"));

    // Zone-level enable/disable
    flags.set_zone_p2p_enabled("prontera", true);
    assert(flags.is_zone_p2p_enabled("prontera"));
    flags.set_zone_p2p_enabled("prontera", false);
    assert(!flags.is_zone_p2p_enabled("prontera"));

    // Fallback logic
    flags.set_service_p2p_enabled("char-server", true);
    flags.force_service_fallback("char-server");
    assert(!flags.is_service_p2p_enabled("char-server"));
    assert(flags.is_service_in_fallback("char-server"));

    flags.set_zone_p2p_enabled("geffen", true);
    flags.force_zone_fallback("geffen");
    assert(!flags.is_zone_p2p_enabled("geffen"));
    assert(flags.is_zone_in_fallback("geffen"));

    // Reset and verify
    flags.reset();
    assert(!flags.is_service_p2p_enabled("map-server"));
    assert(!flags.is_zone_p2p_enabled("prontera"));
    assert(!flags.is_service_in_fallback("char-server"));
    assert(!flags.is_zone_in_fallback("geffen"));

    std::cout << "[P2PRuntimeFlags] All tests passed." << std::endl;
}

int main() {
    test_p2p_runtime_flags();
    return 0;
}