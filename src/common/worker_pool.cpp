// worker_pool.cpp
// Implementation of WorkerPool for rAthena core servers

#include "worker_pool.hpp"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <nlohmann/json.hpp>

WorkerPool::WorkerPool(size_t num_workers) {
    nlohmann::json log_start = {
        {"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())},
        {"level", "INFO"},
        {"service", "worker_pool"},
        {"event", "startup"},
        {"worker_count", num_workers}
    };
    std::cout << log_start.dump() << std::endl;
    for (size_t i = 0; i < num_workers; ++i) {
        auto worker = std::make_unique<WorkerThread>();
        worker->thread = std::thread(&WorkerPool::worker_loop, this, i);
        workers_.push_back(std::move(worker));
    }
}

WorkerPool::~WorkerPool() {
    stop();
    nlohmann::json log_shutdown = {
        {"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())},
        {"level", "INFO"},
        {"service", "worker_pool"},
        {"event", "shutdown"}
    };
    std::cout << log_shutdown.dump() << std::endl;
}

void WorkerPool::start() {
    for (auto& worker : workers_) {
        worker->running = true;
    }
    nlohmann::json log_start = {
        {"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())},
        {"level", "INFO"},
        {"service", "worker_pool"},
        {"event", "workers_started"},
        {"worker_count", workers_.size()}
    };
    std::cout << log_start.dump() << std::endl;
}

void WorkerPool::stop() {
    for (auto& worker : workers_) {
        worker->running = false;
        worker->cv.notify_all();
    }
    for (auto& worker : workers_) {
        if (worker->thread.joinable())
            worker->thread.join();
    }
    nlohmann::json log_stop = {
        {"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())},
        {"level", "INFO"},
        {"service", "worker_pool"},
        {"event", "workers_stopped"},
        {"worker_count", workers_.size()}
    };
    std::cout << log_stop.dump() << std::endl;
}

void WorkerPool::enqueue_task(const WorkerTask& task) {
    // Simple round-robin for now (can be improved with load metrics)
    static std::atomic<size_t> rr{0};
    size_t idx = rr++ % workers_.size();
    {
        std::lock_guard<std::mutex> lock(workers_[idx]->queue_mutex);
        workers_[idx]->task_queue.push(task);
    }
    workers_[idx]->cv.notify_one();
    nlohmann::json log_task = {
        {"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())},
        {"level", "DEBUG"},
        {"service", "worker_pool"},
        {"event", "task_enqueued"},
        {"worker_index", idx}
    };
    std::cout << log_task.dump() << std::endl;
}

void WorkerPool::assign_entity(const std::string& entity_id, float x, float y, float z) {
    std::lock_guard<std::mutex> lock(entity_map_mutex);
    size_t idx = select_worker_for_entity(x, y, z);
    entity_to_worker_[entity_id] = idx;
    nlohmann::json log_assign = {
        {"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())},
        {"level", "DEBUG"},
        {"service", "worker_pool"},
        {"event", "entity_assigned"},
        {"entity_id", entity_id},
        {"worker_index", idx}
    };
    std::cout << log_assign.dump() << std::endl;
}

void WorkerPool::remove_entity(const std::string& entity_id) {
    std::lock_guard<std::mutex> lock(entity_map_mutex);
    entity_to_worker_.erase(entity_id);
}

size_t WorkerPool::get_worker_index(const std::string& entity_id) const {
    std::lock_guard<std::mutex> lock(entity_map_mutex);
    auto it = entity_to_worker_.find(entity_id);
    if (it != entity_to_worker_.end())
        return it->second;
    return 0;
}

size_t WorkerPool::worker_count() const {
    return workers_.size();
}

std::vector<size_t> WorkerPool::get_queue_sizes() const {
    std::vector<size_t> sizes;
    for (const auto& worker : workers_) {
        std::lock_guard<std::mutex> lock(worker->queue_mutex);
        sizes.push_back(worker->task_queue.size());
    }
    return sizes;
}

void WorkerPool::register_simulation_worker(size_t index, std::shared_ptr<IEntitySimulationWorker> worker) {
    if (index < workers_.size()) {
        workers_[index]->simulation_worker = worker;
    }
}

size_t WorkerPool::select_worker_for_entity(float x, float y, float z) {
    // Simple spatial hash: use x/y/z to pick a worker (can be replaced with octree/grid)
    size_t hash = static_cast<size_t>(std::abs(x) + std::abs(y) + std::abs(z));
    return hash % workers_.size();
}

void WorkerPool::worker_loop(size_t index) {
    auto& worker = workers_[index];
    while (worker->running) {
        WorkerTask task;
        {
            std::unique_lock<std::mutex> lock(worker->queue_mutex);
            if (worker->task_queue.empty()) {
                worker->cv.wait(lock, [&] { return !worker->running || !worker->task_queue.empty(); });
            }
            if (!worker->running) break;
            if (!worker->task_queue.empty()) {
                task = worker->task_queue.front();
                worker->task_queue.pop();
            }
        }
        if (task) {
            try {
                task();
            } catch (const std::exception& e) {
                std::cerr << "[WorkerPool] Exception in worker " << index << ": " << e.what() << std::endl;
            }
        }
        // ECS/actor tick: call simulation_worker->tick() if registered
        if (worker->simulation_worker) {
            try {
                worker->simulation_worker->tick();
            } catch (const std::exception& e) {
                std::cerr << "[WorkerPool] Exception in simulation_worker " << index << ": " << e.what() << std::endl;
            }
        }
    }
}