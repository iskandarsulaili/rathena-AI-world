#include "config_loader.hpp"
#include "aiworld_utils.hpp"
#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <thread>

namespace aiworld {

ConfigLoader* ConfigLoader::instance_ = nullptr;

ConfigLoader::ConfigLoader() : reloading_(false) {
    instance_ = this;
}

ConfigLoader::~ConfigLoader() {
    instance_ = nullptr;
}

void ConfigLoader::load() {
    std::lock_guard<std::mutex> lock(reload_mutex_);
    // In a real implementation, load all config from env or files here
    log_info("ConfigLoader: Configuration loaded from environment.");
}

void ConfigLoader::set_reload_callback(std::function<void()> cb) {
    std::lock_guard<std::mutex> lock(reload_mutex_);
    reload_callback_ = std::move(cb);
}

void ConfigLoader::trigger_reload() {
    if (reloading_.exchange(true)) return; // Already reloading
    log_info("ConfigLoader: Triggering configuration reload...");
    load();
    if (reload_callback_) reload_callback_();
    reloading_ = false;
}

void ConfigLoader::install_sighup_handler() {
#ifdef SIGHUP
    std::signal(SIGHUP, sighup_handler);
    log_info("ConfigLoader: SIGHUP handler installed for config reload.");
#else
    log_warn("ConfigLoader: SIGHUP not supported on this platform.");
#endif
}

void ConfigLoader::sighup_handler(int signum) {
    if (instance_) {
        log_info("ConfigLoader: Received SIGHUP, reloading configuration...");
        instance_->trigger_reload();
    }
}

bool ConfigLoader::supports_hot_reload() {
#ifdef SIGHUP
    return true;
#else
    return false;
#endif
}

} // namespace aiworld