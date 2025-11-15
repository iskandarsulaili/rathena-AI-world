#pragma once
#include <string>
#include <vector>
#include <optional>
#include <nlohmann/json.hpp>

namespace aiworld {

// Entity data structure (player or NPC)
struct Entity {
    std::string entity_id;
    std::string entity_type; // "player" or "npc"
    nlohmann::json state;
    std::string created_at;
    std::string updated_at;
};

class EntityLogic {
public:
    // Register or update an entity
    void upsert_entity(const std::string& entity_id, const std::string& entity_type, const nlohmann::json& state);

    // Get an entity by ID
    std::optional<Entity> get_entity(const std::string& entity_id);

    // List all entities of a type
    std::vector<Entity> list_entities(const std::string& entity_type);

    // Delete an entity
    bool delete_entity(const std::string& entity_id);

    // Load/save entities from/to PostgreSQL
    void load_from_db();
    void save_to_db();

    EntityLogic();
    ~EntityLogic();

private:
    std::vector<Entity> entities_;
    // Add DB connection or manager reference as needed
};

} // namespace aiworld