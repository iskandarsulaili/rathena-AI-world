#include "entity_logic.hpp"
#include "postgres_manager.hpp"
#include "aiworld_utils.hpp"
#include <pqxx/pqxx>
#include <stdexcept>
#include <algorithm>

namespace aiworld {

namespace {
const char* CREATE_TABLE_SQL = R"(
CREATE TABLE IF NOT EXISTS entities (
    entity_id TEXT PRIMARY KEY,
    entity_type TEXT NOT NULL,
    state JSONB NOT NULL,
    created_at TIMESTAMPTZ DEFAULT NOW(),
    updated_at TIMESTAMPTZ DEFAULT NOW()
);
)";
}

EntityLogic::EntityLogic() {
    try {
        auto mgr = PostgresManager::create_from_env();
        pqxx::work tx(mgr->connection());
        tx.exec0(CREATE_TABLE_SQL);
        tx.commit();
        log_info("EntityLogic: entities table ensured in PostgreSQL.");
    } catch (const std::exception& e) {
        log_error(std::string("EntityLogic: Failed to ensure schema: ") + e.what());
        throw;
    }
}

EntityLogic::~EntityLogic() {}

void EntityLogic::upsert_entity(const std::string& entity_id, const std::string& entity_type, const nlohmann::json& state) {
    auto mgr = PostgresManager::create_from_env();
    pqxx::work tx(mgr->connection());
    tx.exec0(
        "INSERT INTO entities (entity_id, entity_type, state) VALUES ($1, $2, $3) "
        "ON CONFLICT (entity_id) DO UPDATE SET entity_type = $2, state = $3, updated_at = NOW()",
        entity_id, entity_type, state.dump()
    );
    tx.commit();
    log_info("EntityLogic: Upserted entity " + entity_id + " of type " + entity_type);
}

std::optional<Entity> EntityLogic::get_entity(const std::string& entity_id) {
    auto mgr = PostgresManager::create_from_env();
    pqxx::work tx(mgr->connection());
    auto r = tx.exec_params(
        "SELECT entity_id, entity_type, state, created_at, updated_at FROM entities WHERE entity_id = $1",
        entity_id
    );
    if (r.empty()) return std::nullopt;
    Entity e;
    e.entity_id = r[0][0].as<std::string>();
    e.entity_type = r[0][1].as<std::string>();
    e.state = nlohmann::json::parse(r[0][2].as<std::string>());
    e.created_at = r[0][3].as<std::string>();
    e.updated_at = r[0][4].as<std::string>();
    return e;
}

std::vector<Entity> EntityLogic::list_entities(const std::string& entity_type) {
    auto mgr = PostgresManager::create_from_env();
    pqxx::work tx(mgr->connection());
    auto r = tx.exec_params(
        "SELECT entity_id, entity_type, state, created_at, updated_at FROM entities WHERE entity_type = $1",
        entity_type
    );
    std::vector<Entity> result;
    for (const auto& row : r) {
        Entity e;
        e.entity_id = row[0].as<std::string>();
        e.entity_type = row[1].as<std::string>();
        e.state = nlohmann::json::parse(row[2].as<std::string>());
        e.created_at = row[3].as<std::string>();
        e.updated_at = row[4].as<std::string>();
        result.push_back(e);
    }
    return result;
}

bool EntityLogic::delete_entity(const std::string& entity_id) {
    auto mgr = PostgresManager::create_from_env();
    pqxx::work tx(mgr->connection());
    auto r = tx.exec_params(
        "DELETE FROM entities WHERE entity_id = $1",
        entity_id
    );
    tx.commit();
    log_info("EntityLogic: Deleted entity " + entity_id);
    return r.affected_rows() > 0;
}

void EntityLogic::load_from_db() {
    // No-op: always loads from DB on each call
}

void EntityLogic::save_to_db() {
    // No-op: always saves to DB on each call
}

} // namespace aiworld