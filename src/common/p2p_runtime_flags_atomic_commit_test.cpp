// p2p_runtime_flags_atomic_commit_test.cpp
// Test: atomic commit and verification for P2P runtime flags and fallback logic

#include "p2p_runtime_flags.hpp"
#include <cassert>
#include <iostream>
#include <thread>
#include <vector>

void atomic_flag_setter(const std::string& service, int n) {
    auto& flags = P2PRuntimeFlags::instance();
    for (int i = 0; i < n; ++i) {
        flags.set_service_p2p_enabled(service, i % 2 == 0);
        if (i % 5 == 0) flags.force_service_fallback(service);
    }
}

void atomic_flag_checker(const std::string& service, int n) {
    auto& flags = P2PRuntimeFlags::instance();
    for (int i = 0; i < n; ++i) {
        bool enabled = flags.is_service_p2p_enabled(service);
        bool fallback = flags.is_service_in_fallback(service);
        // Should never crash or deadlock
        (void)enabled;
        (void)fallback;
    }
}

void test_atomic_commit_and_verification() {
    auto& flags = P2PRuntimeFlags::instance();
    flags.reset();

    const std::string service = "map-server";
    const int iterations = 10000;

    std::thread setter(atomic_flag_setter, service, iterations);
    std::thread checker(atomic_flag_checker, service, iterations);

    setter.join();
    checker.join();

    // Final state: fallback should be true (last setter sets fallback)
    assert(flags.is_service_in_fallback(service));
    std::cout << "[P2PRuntimeFlags] Atomic commit and verification test passed." << std::endl;
}

int main() {
    test_atomic_commit_and_verification();
    return 0;
}