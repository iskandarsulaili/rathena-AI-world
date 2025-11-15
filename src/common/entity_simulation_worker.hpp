#pragma once
// entity_simulation_worker.hpp
// ECS/actor-style simulation worker for rAthena core servers

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <mutex>
#include <atomic>

// Forward declaration for entity state
struct EntityState;

// Interface for simulation worker (per-thread, per-core)
class IEntitySimulationWorker {
public:
    virtual ~IEntitySimulationWorker() = default;

    // Called every simulation tick (e.g., 60Hz)
    virtual void tick() = 0;

    // Add entity to simulation
    virtual void add_entity(const std::string& entity_id, std::shared_ptr<EntityState> state) = 0;

    // Remove entity from simulation
    virtual void remove_entity(const std::string& entity_id) = 0;

    // Get entity state
    virtual std::shared_ptr<EntityState> get_entity(const std::string& entity_id) = 0;

    // Get number of entities
    virtual size_t entity_count() const = 0;
};

// Example ECS/actor implementation
class EntitySimulationWorker : public IEntitySimulationWorker {
public:
    EntitySimulationWorker();
    ~EntitySimulationWorker();

    void tick() override;
    void add_entity(const std::string& entity_id, std::shared_ptr<EntityState> state) override;
    void remove_entity(const std::string& entity_id) override;
    std::shared_ptr<EntityState> get_entity(const std::string& entity_id) override;
    size_t entity_count() const override;

private:
    std::unordered_map<std::string, std::shared_ptr<EntityState>> entities_;
    mutable std::mutex entities_mutex;
};

#endif // ENTITY_SIMULATION_WORKER_HPP