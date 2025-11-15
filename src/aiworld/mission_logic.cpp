#include "mission_logic.hpp"
#include "postgres_manager.hpp"
#include "aiworld_utils.hpp"
#include <pqxx/pqxx>
#include <stdexcept>
#include <algorithm>

namespace aiworld {

namespace {
const char* CREATE_TABLE_SQL = R"(
CREATE TABLE IF NOT EXISTS missions (
    mission_id TEXT PRIMARY KEY,
    assignee_id TEXT NOT NULL,
    mission_data JSONB NOT NULL,
    status TEXT NOT NULL,
    created_at TIMESTAMPTZ DEFAULT NOW(),
    updated_at TIMESTAMPTZ DEFAULT NOW()
);
)";
}

MissionLogic::MissionLogic() {
    // Ensure schema exists
    try {
        auto mgr = PostgresManager::create_from_env();
        pqxx::work tx(mgr->connection());
        tx.exec0(CREATE_TABLE_SQL);
        tx.commit();
        log_info("MissionLogic: missions table ensured in PostgreSQL.");
    } catch (const std::exception& e) {
        log_error(std::string("MissionLogic: Failed to ensure schema: ") + e.what());
        throw;
    }
}

MissionLogic::~MissionLogic() {}

std::string MissionLogic::create_mission(const std::string& assignee_id, const nlohmann::json& mission_data) {
    auto mgr = PostgresManager::create_from_env();
    pqxx::work tx(mgr->connection());
    std::string mission_id = "mission_" + std::to_string(std::hash<std::string>{}(assignee_id + mission_data.dump() + std::to_string(std::time(nullptr))));
    tx.exec0(
        "INSERT INTO missions (mission_id, assignee_id, mission_data, status) VALUES ($1, $2, $3, $4)",
        mission_id, assignee_id, mission_data.dump(), "pending"
    );
    tx.commit();
    log_info("MissionLogic: Created mission " + mission_id + " for " + assignee_id);
    return mission_id;
}

std::optional<Mission> MissionLogic::get_mission(const std::string& mission_id) {
    auto mgr = PostgresManager::create_from_env();
    pqxx::work tx(mgr->connection());
    auto r = tx.exec_params(
        "SELECT mission_id, assignee_id, mission_data, status, created_at, updated_at FROM missions WHERE mission_id = $1",
        mission_id
    );
    if (r.empty()) return std::nullopt;
    Mission m;
    m.mission_id = r[0][0].as<std::string>();
    m.assignee_id = r[0][1].as<std::string>();
    m.mission_data = nlohmann::json::parse(r[0][2].as<std::string>());
    m.status = r[0][3].as<std::string>();
    m.created_at = r[0][4].as<std::string>();
    m.updated_at = r[0][5].as<std::string>();
    return m;
}

bool MissionLogic::update_mission_status(const std::string& mission_id, const std::string& status) {
    auto mgr = PostgresManager::create_from_env();
    pqxx::work tx(mgr->connection());
    auto r = tx.exec_params(
        "UPDATE missions SET status = $1, updated_at = NOW() WHERE mission_id = $2",
        status, mission_id
    );
    tx.commit();
    log_info("MissionLogic: Updated mission " + mission_id + " to status " + status);
    return r.affected_rows() > 0;
}

std::vector<Mission> MissionLogic::list_missions(const std::string& assignee_id) {
    auto mgr = PostgresManager::create_from_env();
    pqxx::work tx(mgr->connection());
    auto r = tx.exec_params(
        "SELECT mission_id, assignee_id, mission_data, status, created_at, updated_at FROM missions WHERE assignee_id = $1",
        assignee_id
    );
    std::vector<Mission> result;
    for (const auto& row : r) {
        Mission m;
        m.mission_id = row[0].as<std::string>();
        m.assignee_id = row[1].as<std::string>();
        m.mission_data = nlohmann::json::parse(row[2].as<std::string>());
        m.status = row[3].as<std::string>();
        m.created_at = row[4].as<std::string>();
        m.updated_at = row[5].as<std::string>();
        result.push_back(m);
    }
    return result;
}

bool MissionLogic::delete_mission(const std::string& mission_id) {
    auto mgr = PostgresManager::create_from_env();
    pqxx::work tx(mgr->connection());
    auto r = tx.exec_params(
        "DELETE FROM missions WHERE mission_id = $1",
        mission_id
    );
    tx.commit();
    log_info("MissionLogic: Deleted mission " + mission_id);
    return r.affected_rows() > 0;
}

void MissionLogic::load_from_db() {
    // No-op: always loads from DB on each call
}

void MissionLogic::save_to_db() {
    // No-op: always saves to DB on each call
}

} // namespace aiworld