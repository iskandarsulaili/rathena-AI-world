#pragma once
// entity_state.hpp
// Unified entity state for ECS/actor simulation in rAthena core servers

#include <string>
#include <vector>
#include <memory>
#include <cstdint>
#include <unordered_map>

// Basic 3D vector
struct Vector3 {
    float x, y, z;
};

// Entity state (can be extended for player, NPC, mob, etc.)
struct EntityState {
    std::string entity_id;
    std::string entity_type; // "player", "npc", "mob", etc.
    Vector3 position;
    Vector3 velocity;
    uint64_t last_update_tick;
    std::unordered_map<std::string, int64_t> int_attrs;
    std::unordered_map<std::string, float> float_attrs;
    std::unordered_map<std::string, std::string> string_attrs;
    // Add more ECS-style components as needed

    // Update logic (can be overridden)
    virtual void update() {
        // Example: simple position update
        position.x += velocity.x;
        position.y += velocity.y;
        position.z += velocity.z;
        last_update_tick++;
    }

    virtual ~EntityState() = default;
};

using EntityStatePtr = std::shared_ptr<EntityState>;