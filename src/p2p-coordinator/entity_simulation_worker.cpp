// entity_simulation_worker.cpp
// Implementation of EntitySimulationWorker for authoritative state and gameplay logic

#include "entity_simulation_worker.hpp"
#include <nlohmann/json.hpp>
#include <iostream>

EntitySimulationWorker::EntitySimulationWorker() {}

EntitySimulationWorker::~EntitySimulationWorker() {}

void EntitySimulationWorker::add_entity(const EntityState& state) {
    std::lock_guard<std::mutex> lock(sim_mutex_);
    entities_[state.entity_id] = std::make_shared<EntityState>(state);
    nlohmann::json log = {
        {"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())},
        {"level", "INFO"},
        {"event", "entity_added"},
        {"entity_id", state.entity_id}
    };
    std::cout << log.dump() << std::endl;
}

void EntitySimulationWorker::remove_entity(const std::string& entity_id) {
    std::lock_guard<std::mutex> lock(sim_mutex_);
    entities_.erase(entity_id);
    nlohmann::json log = {
        {"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())},
        {"level", "INFO"},
        {"event", "entity_removed"},
        {"entity_id", entity_id}
    };
    std::cout << log.dump() << std::endl;
}

std::shared_ptr<EntityState> EntitySimulationWorker::get_entity(const std::string& entity_id) {
    std::lock_guard<std::mutex> lock(sim_mutex_);
    auto it = entities_.find(entity_id);
    if (it != entities_.end()) {
        return it->second;
    }
    return nullptr;
}

void EntitySimulationWorker::tick() {
    std::lock_guard<std::mutex> lock(sim_mutex_);
    // Example: update entity positions, process gameplay logic
    for (auto& kv : entities_) {
        // Simulate physics, AI, cooldowns, etc. (stub)
        // kv.second->x += kv.second->velocity_x;
        // kv.second->y += kv.second->velocity_y;
        // kv.second->z += kv.second->velocity_z;
    }
    nlohmann::json log = {
        {"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())},
        {"level", "DEBUG"},
        {"event", "simulation_tick"},
        {"entity_count", entities_.size()}
    };
    std::cout << log.dump() << std::endl;
}

size_t EntitySimulationWorker::entity_count() const {
    std::lock_guard<std::mutex> lock(sim_mutex_);
    return entities_.size();
}