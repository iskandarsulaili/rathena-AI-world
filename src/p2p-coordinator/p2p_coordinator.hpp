// p2p_coordinator.hpp
// Main header for P2P Coordinator server

#pragma once

#include <memory>
#include <vector>
#include <string>
#include <atomic>
#include "../common/worker_pool.hpp"
#include "../common/entity_simulation_worker.hpp"

class P2PCoordinatorServer {
public:
    P2PCoordinatorServer(size_t num_workers, bool p2p_enabled);
    ~P2PCoordinatorServer();

    // Start all workers and main loop
    void start();

    // Stop all workers and shutdown
    void stop();

    // Main tick loop (for diagnostics, metrics, etc)
    void tick();

    // Add/remove entities (for future mesh/protocol integration)
    void add_entity(const std::string& entity_id, float x, float y, float z);
    void remove_entity(const std::string& entity_id);

    // Get worker stats
    std::vector<size_t> get_worker_queue_sizes() const;

private:
    std::unique_ptr<WorkerPool> worker_pool_;
    std::vector<std::shared_ptr<EntitySimulationWorker>> simulation_workers_;
    std::atomic<bool> running_;
    bool p2p_enabled_;
};