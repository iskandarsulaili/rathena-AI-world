// worker_pool.hpp
// Worker pool for entity simulation, authoritative state, and state sync/reconciliation

#pragma once

#include <vector>
#include <string>
#include <memory>
#include <mutex>
#include <thread>
#include <atomic>
#include "entity_simulation_worker.hpp"

class WorkerPool {
public:
    WorkerPool(size_t num_workers);
    ~WorkerPool();

    // Start/stop all simulation workers
    void start();
    void stop();

    // Assign/remove entity to/from a worker (spatial partitioning)
    void assign_entity(const std::string& entity_id, float x, float y, float z);
    void remove_entity(const std::string& entity_id);

    // Get simulation worker for an entity
    std::shared_ptr<EntitySimulationWorker> get_simulation_worker_for_entity(const std::string& entity_id);

    // Get queue sizes for diagnostics
    std::vector<size_t> get_queue_sizes() const;

private:
    void worker_loop(size_t idx);

    std::vector<std::shared_ptr<EntitySimulationWorker>> workers_;
    std::vector<std::thread> threads_;
    std::atomic<bool> running_;
    mutable std::mutex pool_mutex_;
    std::unordered_map<std::string, size_t> entity_to_worker_;
};