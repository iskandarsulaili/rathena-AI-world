// worker_pool.cpp
// Implementation of WorkerPool for entity simulation and authoritative state

#include "worker_pool.hpp"
#include <nlohmann/json.hpp>
#include <iostream>
#include <cmath>

WorkerPool::WorkerPool(size_t num_workers)
    : running_(false)
{
    workers_.reserve(num_workers);
    for (size_t i = 0; i < num_workers; ++i) {
        workers_.emplace_back(std::make_shared<EntitySimulationWorker>());
    }
}

WorkerPool::~WorkerPool() {
    stop();
}

void WorkerPool::start() {
    running_ = true;
    for (size_t i = 0; i < workers_.size(); ++i) {
        threads_.emplace_back(&WorkerPool::worker_loop, this, i);
    }
    nlohmann::json log = {
        {"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())},
        {"level", "INFO"},
        {"event", "worker_pool_started"},
        {"worker_count", workers_.size()}
    };
    std::cout << log.dump() << std::endl;
}

void WorkerPool::stop() {
    running_ = false;
    for (auto& t : threads_) {
        if (t.joinable()) t.join();
    }
    threads_.clear();
    nlohmann::json log = {
        {"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())},
        {"level", "INFO"},
        {"event", "worker_pool_stopped"}
    };
    std::cout << log.dump() << std::endl;
}

void WorkerPool::assign_entity(const std::string& entity_id, float x, float y, float z) {
    std::lock_guard<std::mutex> lock(pool_mutex_);
    // Simple spatial hash: assign by (x + y + z) mod num_workers
    size_t idx = static_cast<size_t>(std::abs(static_cast<int>(x + y + z))) % workers_.size();
    entity_to_worker_[entity_id] = idx;
    EntityState state;
    state.entity_id = entity_id;
    state.x = static_cast<int32_t>(x);
    state.y = static_cast<int32_t>(y);
    state.z = static_cast<int32_t>(z);
    workers_[idx]->add_entity(state);
    nlohmann::json log = {
        {"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())},
        {"level", "INFO"},
        {"event", "entity_assigned"},
        {"entity_id", entity_id},
        {"worker_idx", idx}
    };
    std::cout << log.dump() << std::endl;
}

void WorkerPool::remove_entity(const std::string& entity_id) {
    std::lock_guard<std::mutex> lock(pool_mutex_);
    auto it = entity_to_worker_.find(entity_id);
    if (it != entity_to_worker_.end()) {
        size_t idx = it->second;
        workers_[idx]->remove_entity(entity_id);
        entity_to_worker_.erase(it);
        nlohmann::json log = {
            {"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())},
            {"level", "INFO"},
            {"event", "entity_removed"},
            {"entity_id", entity_id},
            {"worker_idx", idx}
        };
        std::cout << log.dump() << std::endl;
    }
}

std::shared_ptr<EntitySimulationWorker> WorkerPool::get_simulation_worker_for_entity(const std::string& entity_id) {
    std::lock_guard<std::mutex> lock(pool_mutex_);
    auto it = entity_to_worker_.find(entity_id);
    if (it != entity_to_worker_.end()) {
        return workers_[it->second];
    }
    return nullptr;
}

std::vector<size_t> WorkerPool::get_queue_sizes() const {
    std::lock_guard<std::mutex> lock(pool_mutex_);
    std::vector<size_t> sizes;
    for (const auto& worker : workers_) {
        sizes.push_back(worker->entity_count());
    }
    return sizes;
}

void WorkerPool::worker_loop(size_t idx) {
    while (running_) {
        workers_[idx]->tick();
        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // 60Hz tick
    }
}