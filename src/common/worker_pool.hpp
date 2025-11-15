#pragma once
// worker_pool.hpp
// Production-grade multi-threaded worker pool for rAthena core servers (map-server, char-server, login-server, aiworld, etc.)

#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <functional>
#include <memory>
#include <unordered_map>
#include <string>

// Forward declaration for ECS/actor integration
class IEntitySimulationWorker;

// Task type for simulation/event processing
using WorkerTask = std::function<void()>;

// Worker pool for per-core simulation and event processing
class WorkerPool {
public:
    WorkerPool(size_t num_workers);
    ~WorkerPool();

    // Start all workers
    void start();

    // Stop all workers and join threads
    void stop();

    // Enqueue a task to the pool (thread-safe)
    void enqueue_task(const WorkerTask& task);

    // Assign an entity to a worker (spatial partitioning, load balancing)
    void assign_entity(const std::string& entity_id, float x, float y, float z);

    // Remove an entity from the pool
    void remove_entity(const std::string& entity_id);

    // Get worker for an entity
    size_t get_worker_index(const std::string& entity_id) const;

    // Get number of workers
    size_t worker_count() const;

    // Get stats (for metrics/monitoring)
    std::vector<size_t> get_queue_sizes() const;

    // ECS/actor integration: register simulation worker
    void register_simulation_worker(size_t index, std::shared_ptr<IEntitySimulationWorker> worker);

private:
    struct WorkerThread {
        std::thread thread;
        std::queue<WorkerTask> task_queue;
        std::mutex queue_mutex;
        std::condition_variable cv;
        std::atomic<bool> running{true};
        std::shared_ptr<IEntitySimulationWorker> simulation_worker;
    };

    std::vector<std::unique_ptr<WorkerThread>> workers_;
    std::unordered_map<std::string, size_t> entity_to_worker_;
    std::mutex entity_map_mutex;

    // Spatial partitioning (octree/grid) and load balancing
    size_t select_worker_for_entity(float x, float y, float z);

    void worker_loop(size_t index);
};

#endif // WORKER_POOL_HPP