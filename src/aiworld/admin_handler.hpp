#pragma once
#include <string>
#include <functional>
#include <mutex>
#include <unordered_map>
#include <zmq.h>

namespace aiworld {

class AdminHandler {
public:
    // Register an admin command (e.g., "reload", "status")
    void register_command(const std::string& cmd, std::function<std::string(const std::string&)> handler);

    // Handle an incoming admin command (returns response)
    std::string handle_command(const std::string& cmd, const std::string& args);

    // Process ZeroMQ admin messages (blocking, runs in its own thread)
    void run_admin_loop(void* zmq_context, const std::string& endpoint);

    // Trigger config reload (calls registered callback)
    void set_reload_callback(std::function<void()> cb);

    AdminHandler();
    ~AdminHandler();

private:
    std::unordered_map<std::string, std::function<std::string(const std::string&)>> commands_;
    std::function<void()> reload_callback_;
    std::mutex cmd_mutex_;
    std::atomic<bool> running_;
};

} // namespace aiworld