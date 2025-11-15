// p2p_runtime_flags.cpp
// Implementation of P2PRuntimeFlags for rAthena core services

#include "p2p_runtime_flags.hpp"

P2PRuntimeFlags& P2PRuntimeFlags::instance() {
    static P2PRuntimeFlags inst;
    return inst;
}

P2PRuntimeFlags::P2PRuntimeFlags() {}

void P2PRuntimeFlags::set_service_p2p_enabled(const std::string& service, bool enabled) {
    std::lock_guard<std::mutex> lock(mutex_);
    service_p2p_enabled_[service] = enabled;
    if (!enabled) service_fallback_[service] = false;
}

bool P2PRuntimeFlags::is_service_p2p_enabled(const std::string& service) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = service_p2p_enabled_.find(service);
    return it != service_p2p_enabled_.end() && it->second && !is_service_in_fallback(service);
}

void P2PRuntimeFlags::set_zone_p2p_enabled(const std::string& zone, bool enabled) {
    std::lock_guard<std::mutex> lock(mutex_);
    zone_p2p_enabled_[zone] = enabled;
    if (!enabled) zone_fallback_[zone] = false;
}

bool P2PRuntimeFlags::is_zone_p2p_enabled(const std::string& zone) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = zone_p2p_enabled_.find(zone);
    return it != zone_p2p_enabled_.end() && it->second && !is_zone_in_fallback(zone);
}

void P2PRuntimeFlags::force_service_fallback(const std::string& service) {
    std::lock_guard<std::mutex> lock(mutex_);
    service_fallback_[service] = true;
}

void P2PRuntimeFlags::force_zone_fallback(const std::string& zone) {
    std::lock_guard<std::mutex> lock(mutex_);
    zone_fallback_[zone] = true;
}

bool P2PRuntimeFlags::is_service_in_fallback(const std::string& service) const {
    auto it = service_fallback_.find(service);
    return it != service_fallback_.end() && it->second;
}

bool P2PRuntimeFlags::is_zone_in_fallback(const std::string& zone) const {
    auto it = zone_fallback_.find(zone);
    return it != zone_fallback_.end() && it->second;
}

void P2PRuntimeFlags::reset() {
    std::lock_guard<std::mutex> lock(mutex_);
    service_p2p_enabled_.clear();
    zone_p2p_enabled_.clear();
    service_fallback_.clear();
    zone_fallback_.clear();
}