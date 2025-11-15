#pragma once
#include <string>
#include <functional>
#include <mutex>
#include <atomic>
#include <csignal>

namespace aiworld {

class ConfigLoader {
public:
    // Loads config from environment variables (thread-safe)
    void load();

    // Register a callback to be called on config reload
    void set_reload_callback(std::function<void()> cb);

    // Install SIGHUP handler (POSIX only)
    void install_sighup_handler();

    // Trigger reload (from ZeroMQ admin or SIGHUP)
    void trigger_reload();

    // Returns true if hot-reload is supported
    static bool supports_hot_reload();

    ConfigLoader();
    ~ConfigLoader();

private:
    std::function<void()> reload_callback_;
    std::mutex reload_mutex_;
    std::atomic<bool> reloading_;
    static ConfigLoader* instance_;
    static void sighup_handler(int signum);
};

} // namespace aiworld