#include "event_logic.hpp"
#include "postgres_manager.hpp"
#include "aiworld_utils.hpp"
#include <pqxx/pqxx>
#include <stdexcept>
#include <algorithm>

namespace aiworld {

namespace {
const char* CREATE_TABLE_SQL = R"(
CREATE TABLE IF NOT EXISTS events (
    event_id TEXT PRIMARY KEY,
    event_type TEXT NOT NULL,
    event_data JSONB NOT NULL,
    created_at TIMESTAMPTZ DEFAULT NOW()
);
)";
}

EventLogic::EventLogic() {
    try {
        auto mgr = PostgresManager::create_from_env();
        pqxx::work tx(mgr->connection());
        tx.exec0(CREATE_TABLE_SQL);
        tx.commit();
        log_info("EventLogic: events table ensured in PostgreSQL.");
    } catch (const std::exception& e) {
        log_error(std::string("EventLogic: Failed to ensure schema: ") + e.what());
        throw;
    }
}

EventLogic::~EventLogic() {}

std::string EventLogic::create_event(const std::string& event_type, const nlohmann::json& event_data) {
    auto mgr = PostgresManager::create_from_env();
    pqxx::work tx(mgr->connection());
    std::string event_id = "event_" + std::to_string(std::hash<std::string>{}(event_type + event_data.dump() + std::to_string(std::time(nullptr))));
    tx.exec0(
        "INSERT INTO events (event_id, event_type, event_data) VALUES ($1, $2, $3)",
        event_id, event_type, event_data.dump()
    );
    tx.commit();
    log_info("EventLogic: Created event " + event_id + " of type " + event_type);
    return event_id;
}

std::optional<Event> EventLogic::get_event(const std::string& event_id) {
    auto mgr = PostgresManager::create_from_env();
    pqxx::work tx(mgr->connection());
    auto r = tx.exec_params(
        "SELECT event_id, event_type, event_data, created_at FROM events WHERE event_id = $1",
        event_id
    );
    if (r.empty()) return std::nullopt;
    Event e;
    e.event_id = r[0][0].as<std::string>();
    e.event_type = r[0][1].as<std::string>();
    e.event_data = nlohmann::json::parse(r[0][2].as<std::string>());
    e.created_at = r[0][3].as<std::string>();
    return e;
}

std::vector<Event> EventLogic::list_events(const std::string& event_type) {
    auto mgr = PostgresManager::create_from_env();
    pqxx::work tx(mgr->connection());
    auto r = tx.exec_params(
        "SELECT event_id, event_type, event_data, created_at FROM events WHERE event_type = $1",
        event_type
    );
    std::vector<Event> result;
    for (const auto& row : r) {
        Event e;
        e.event_id = row[0].as<std::string>();
        e.event_type = row[1].as<std::string>();
        e.event_data = nlohmann::json::parse(row[2].as<std::string>());
        e.created_at = row[3].as<std::string>();
        result.push_back(e);
    }
    return result;
}

bool EventLogic::delete_event(const std::string& event_id) {
    auto mgr = PostgresManager::create_from_env();
    pqxx::work tx(mgr->connection());
    auto r = tx.exec_params(
        "DELETE FROM events WHERE event_id = $1",
        event_id
    );
    tx.commit();
    log_info("EventLogic: Deleted event " + event_id);
    return r.affected_rows() > 0;
}

void EventLogic::load_from_db() {
    // No-op: always loads from DB on each call
}

void EventLogic::save_to_db() {
    // No-op: always saves to DB on each call
}

} // namespace aiworld