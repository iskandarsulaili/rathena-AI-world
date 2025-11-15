// entity_simulation_worker.cpp
// Implementation of EntitySimulationWorker for ECS/actor-style simulation

#include "entity_simulation_worker.hpp"
#include <iostream>

EntitySimulationWorker::EntitySimulationWorker() {}

EntitySimulationWorker::~EntitySimulationWorker() {}

void EntitySimulationWorker::tick() {
    std::lock_guard<std::mutex> lock(entities_mutex);
    for (auto& pair : entities_) {
        auto& state = pair.second;
        // TODO: Implement ECS/actor tick logic for each entity
        // Example: state->update();
        if (state) {
            // Placeholder: print entity id
            std::cout << "[EntitySimulationWorker] Tick entity: " << pair.first << std::endl;
        }
    }
}

void EntitySimulationWorker::add_entity(const std::string& entity_id, std::shared_ptr<EntityState> state) {
    std::lock_guard<std::mutex> lock(entities_mutex);
    entities_[entity_id] = state;
}

void EntitySimulationWorker::remove_entity(const std::string& entity_id) {
    std::lock_guard<std::mutex> lock(entities_mutex);
    entities_.erase(entity_id);
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