// entity_simulation_worker.cpp
// Implementation of EntitySimulationWorker for ECS/actor-style simulation

#include "entity_simulation_worker.hpp"
#include <iostream>
#include <nlohmann/json.hpp>

EntitySimulationWorker::EntitySimulationWorker() {
    nlohmann::json log_start = {
        {"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())},
        {"level", "INFO"},
        {"service", "entity_simulation_worker"},
        {"event", "startup"}
    };
    std::cout << log_start.dump() << std::endl;
}

EntitySimulationWorker::~EntitySimulationWorker() {
    nlohmann::json log_shutdown = {
        {"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())},
        {"level", "INFO"},
        {"service", "entity_simulation_worker"},
        {"event", "shutdown"}
    };
    std::cout << log_shutdown.dump() << std::endl;
}

void EntitySimulationWorker::tick() {
    std::lock_guard<std::mutex> lock(entities_mutex);
    for (auto& pair : entities_) {
        auto& state = pair.second;
        // TODO: Implement ECS/actor tick logic for each entity
        // Example: state->update();
        if (state) {
            nlohmann::json log_tick = {
                {"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())},
                {"level", "DEBUG"},
                {"service", "entity_simulation_worker"},
                {"event", "entity_tick"},
                {"entity_id", pair.first}
            };
            std::cout << log_tick.dump() << std::endl;
        }
    }
}

void EntitySimulationWorker::add_entity(const std::string& entity_id, std::shared_ptr<EntityState> state) {
    std::lock_guard<std::mutex> lock(entities_mutex);
    entities_[entity_id] = state;
    nlohmann::json log_add = {
        {"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())},
        {"level", "DEBUG"},
        {"service", "entity_simulation_worker"},
        {"event", "entity_added"},
        {"entity_id", entity_id}
    };
    std::cout << log_add.dump() << std::endl;
}

void EntitySimulationWorker::remove_entity(const std::string& entity_id) {
    std::lock_guard<std::mutex> lock(entities_mutex);
    size_t erased = entities_.erase(entity_id);
    nlohmann::json log_remove = {
        {"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())},
        {"level", erased ? "DEBUG" : "WARN"},
        {"service", "entity_simulation_worker"},
        {"event", "entity_removed"},
        {"entity_id", entity_id},
        {"removed", static_cast<bool>(erased)}
    };
    std::cout << log_remove.dump() << std::endl;
}

std::shared_ptr<EntityState> EntitySimulationWorker::get_entity(const std::string& entity_id) {
    std::lock_guard<std::mutex> lock(entities_mutex);
    auto it = entities_.find(entity_id);
    if (it != entities_.end())
        return it->second;
    return nullptr;
}

size_t EntitySimulationWorker::entity_count() const {
    std::lock_guard<std::mutex> lock(entities_mutex);
    return entities_.size();
}