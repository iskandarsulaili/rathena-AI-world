// p2p_coordinator.cpp
// Main entry point and server implementation for the P2P Coordinator

#include "p2p_coordinator.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <csignal>
#include <atomic>
#include <thread>
#include <chrono>
#include <nlohmann/json.hpp> // For structured logging

static std::atomic<bool> running(true);

void signal_handler(int signal) {
    running = false;
}

// Implementation of P2PCoordinatorServer
P2PCoordinatorServer::P2PCoordinatorServer(size_t num_workers, bool p2p_enabled)
    : worker_pool_(std::make_unique<WorkerPool>(num_workers)),
      running_(false),
      p2p_enabled_(p2p_enabled)
{
    // Create and register simulation workers
    simulation_workers_.reserve(num_workers);
    for (size_t i = 0; i < num_workers; ++i) {
        auto sim_worker = std::make_shared<EntitySimulationWorker>();
        simulation_workers_.push_back(sim_worker);
        worker_pool_->register_simulation_worker(i, sim_worker);
    }
}

P2PCoordinatorServer::~P2PCoordinatorServer() {
    stop();
}

void P2PCoordinatorServer::start() {
    running_ = true;
    worker_pool_->start();

    // Structured startup log
    nlohmann::json log_start = {
        {"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())},
        {"level", "INFO"},
        {"service", "p2p_coordinator"},
        {"event", "startup"},
        {"p2p_enabled", p2p_enabled_},
        {"worker_count", simulation_workers_.size()}
    };
    std::cout << log_start.dump() << std::endl;

    // Main tick loop
    while (running_) {
        tick();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void P2PCoordinatorServer::stop() {
    if (running_) {
        running_ = false;
        worker_pool_->stop();

        // Structured shutdown log
        nlohmann::json log_stop = {
            {"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())},
            {"level", "INFO"},
            {"service", "p2p_coordinator"},
            {"event", "shutdown"}
        };
        std::cout << log_stop.dump() << std::endl;
    }
}

void P2PCoordinatorServer::tick() {
    // Example: log worker queue sizes and entity counts
    nlohmann::json log_tick = {
        {"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())},
        {"level", "DEBUG"},
        {"service", "p2p_coordinator"},
        {"event", "tick"},
        {"worker_queues", get_worker_queue_sizes()}
    };
    std::vector<size_t> entity_counts;
    for (const auto& sim_worker : simulation_workers_) {
        entity_counts.push_back(sim_worker->entity_count());
    }
    log_tick["entity_counts"] = entity_counts;
    std::cout << log_tick.dump() << std::endl;
}

void P2PCoordinatorServer::add_entity(const std::string& entity_id, float x, float y, float z) {
    worker_pool_->assign_entity(entity_id, x, y, z);
    // Future: add entity state to simulation worker
}

void P2PCoordinatorServer::remove_entity(const std::string& entity_id) {
    worker_pool_->remove_entity(entity_id);
    // Future: remove entity state from simulation worker
}

std::vector<size_t> P2PCoordinatorServer::get_worker_queue_sizes() const {
    return worker_pool_->get_queue_sizes();
}

// Main entry point
void print_usage() {
    std::cout << "Usage: p2p_coordinator [--config path] [--disable-p2p]" << std::endl;
}

int main(int argc, char* argv[]) {
    std::string config_path = "config/p2p_coordinator.conf";
    bool p2p_enabled = true;
    bool show_status = false;

    // Parse command-line args
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--config" && i + 1 < argc) {
            config_path = argv[++i];
        } else if (arg == "--disable-p2p") {
            p2p_enabled = false;
        } else if (arg == "--status") {
            show_status = true;
        } else if (arg == "--help" || arg == "-h") {
            print_usage();
            return 0;
        }
    }

    // Load config (optional, can extend)
    std::ifstream config_file(config_path);
    if (!config_file) {
        nlohmann::json log_warn = {
            {"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())},
            {"level", "WARN"},
            {"event", "config_file_missing"},
            {"config_path", config_path}
        };
        std::cerr << log_warn.dump() << std::endl;
    }

    // Determine number of workers (default: hardware concurrency)
    size_t num_workers = std::thread::hardware_concurrency();
    if (num_workers == 0) num_workers = 4; // Fallback

    // Diagnostics/status output
    if (show_status) {
        nlohmann::json status = {
            {"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())},
            {"service", "p2p_coordinator"},
            {"config_path", config_path},
            {"p2p_enabled", p2p_enabled},
            {"worker_count", num_workers}
        };
        std::cout << status.dump(2) << std::endl;
        return 0;
    }

    // Register signal handlers for graceful shutdown
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    try {
        // Instantiate and start the server
        P2PCoordinatorServer server(num_workers, p2p_enabled);
        server.start();
    } catch (const std::exception& ex) {
        nlohmann::json log_err = {
            {"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())},
            {"level", "ERROR"},
            {"event", "fatal_exception"},
            {"message", ex.what()}
        };
        std::cerr << log_err.dump() << std::endl;
        return 1;
    } catch (...) {
        nlohmann::json log_err = {
            {"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())},
            {"level", "ERROR"},
            {"event", "fatal_exception"},
            {"message", "Unknown exception"}
        };
        std::cerr << log_err.dump() << std::endl;
        return 1;
    }

    return 0;
}