#pragma once
// p2p_runtime_flags.hpp
// Runtime P2P enable/disable and fallback flags for all rAthena core services

#include <atomic>
#include <string>
#include <unordered_map>
#include <mutex>

// Per-service and per-zone/region P2P flags
class P2PRuntimeFlags {
public:
    // Singleton access
    static P2PRuntimeFlags& instance();

    // Enable/disable P2P globally for a service (map-server, char-server, login-server, aiworld, etc.)
    void set_service_p2p_enabled(const std::string& service, bool enabled);
    bool is_service_p2p_enabled(const std::string& service) const;

    // Enable/disable P2P for a specific zone/region
    void set_zone_p2p_enabled(const std::string& zone, bool enabled);
    bool is_zone_p2p_enabled(const std::string& zone) const;

    // Fallback: forcibly disable P2P for a service (e.g., on error)
    void force_service_fallback(const std::string& service);

    // Fallback: forcibly disable P2P for a zone
    void force_zone_fallback(const std::string& zone);

    // Query fallback state
    bool is_service_in_fallback(const std::string& service) const;
    bool is_zone_in_fallback(const std::string& zone) const;

    // Reset all flags (for test/verification)
    void reset();

private:
    P2PRuntimeFlags();
    mutable std::mutex mutex_;
    std::unordered_map<std::string, bool> service_p2p_enabled_;
    std::unordered_map<std::string, bool> zone_p2p_enabled_;
    std::unordered_map<std::string, bool> service_fallback_;
    std::unordered_map<std::string, bool> zone_fallback_;
};

#endif // P2P_RUNTIME_FLAGS_HPP