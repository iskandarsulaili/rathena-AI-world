#include "admin_handler.hpp"
#include "aiworld_utils.hpp"
#include <thread>
#include <sstream>
#include <iostream>
#include <cstring>

namespace aiworld {

AdminHandler::AdminHandler() : running_(false) {}

AdminHandler::~AdminHandler() {
    running_ = false;
}

void AdminHandler::register_command(const std::string& cmd, std::function<std::string(const std::string&)> handler) {
    std::lock_guard<std::mutex> lock(cmd_mutex_);
    commands_[cmd] = std::move(handler);
}

std::string AdminHandler::handle_command(const std::string& cmd, const std::string& args) {
    std::lock_guard<std::mutex> lock(cmd_mutex_);
    auto it = commands_.find(cmd);
    if (it != commands_.end()) {
        return it->second(args);
    }
    return "Unknown admin command: " + cmd;
}

void AdminHandler::set_reload_callback(std::function<void()> cb) {
    reload_callback_ = std::move(cb);
}

void AdminHandler::run_admin_loop(void* zmq_context, const std::string& endpoint) {
    running_ = true;
    void* admin_socket = zmq_socket(zmq_context, ZMQ_REP);
    if (!admin_socket) {
        log_error("AdminHandler: Failed to create ZeroMQ admin socket.");
        return;
    }
    if (zmq_bind(admin_socket, endpoint.c_str()) != 0) {
        log_error("AdminHandler: Failed to bind ZeroMQ admin socket to " + endpoint);
        zmq_close(admin_socket);
        return;
    }
    log_info("AdminHandler: ZeroMQ admin socket listening on " + endpoint);

    while (running_) {
        zmq_msg_t msg;
        zmq_msg_init(&msg);
        int rc = zmq_msg_recv(&msg, admin_socket, 0);
        if (rc < 0) {
            zmq_msg_close(&msg);
            continue;
        }
        std::string req_str(static_cast<char*>(zmq_msg_data(&msg)), zmq_msg_size(&msg));
        zmq_msg_close(&msg);

        log_info("AdminHandler: Received admin command: " + req_str);

        // Parse command and args (format: "cmd arg1 arg2 ...")
        std::istringstream iss(req_str);
        std::string cmd, arg;
        iss >> cmd;
        std::getline(iss, arg);
        if (!arg.empty() && arg[0] == ' ') arg = arg.substr(1);

        std::string response;
        if (cmd == "reload") {
            if (reload_callback_) {
                reload_callback_();
                response = "Config reload triggered.";
            } else {
                response = "No reload callback registered.";
            }
        } else {
            response = handle_command(cmd, arg);
        }

        zmq_msg_t resp;
        zmq_msg_init_size(&resp, response.size());
        std::memcpy(zmq_msg_data(&resp), response.data(), response.size());
        zmq_msg_send(&resp, admin_socket, 0);
        zmq_msg_close(&resp);
    }
    zmq_close(admin_socket);
}

} // namespace aiworld