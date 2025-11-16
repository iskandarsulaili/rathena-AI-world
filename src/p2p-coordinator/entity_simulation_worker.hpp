// entity_simulation_worker.hpp
// Entity simulation, authoritative state, and gameplay logic for P2P Coordinator

#pragma once

#include <string>
#include <unordered_map>
#include <mutex>
#include <memory>

struct EntityState {
    std::string entity_id;
    int32_t x, y, z;
    int16_t velocity_x, velocity_y, velocity_z;
    // Add more fields as needed (HP, buffs, etc.)
};

class EntitySimulationWorker {
public:
    EntitySimulationWorker();
    ~EntitySimulationWorker();

    // Add/remove entity
    void add_entity(const EntityState& state);
    void remove_entity(const std::string& entity_id);

    // Get entity state
    std::shared_ptr<EntityState> get_entity(const std::string& entity_id);

    // Simulation tick
    void tick();

    // Entity count
    size_t entity_count() const;

private:
    mutable std::mutex sim_mutex_;
    std::unordered_map<std::string, std::shared_ptr<EntityState>> entities_;
};