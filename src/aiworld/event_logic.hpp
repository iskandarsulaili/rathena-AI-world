#pragma once
#include <string>
#include <vector>
#include <optional>
#include <nlohmann/json.hpp>

namespace aiworld {

// Event data structure
struct Event {
    std::string event_id;
    std::string event_type;
    nlohmann::json event_data;
    std::string created_at;
};

class EventLogic {
public:
    // Create a new event
    std::string create_event(const std::string& event_type, const nlohmann::json& event_data);

    // Get an event by ID
    std::optional<Event> get_event(const std::string& event_id);

    // List all events of a type
    std::vector<Event> list_events(const std::string& event_type);

    // Delete an event
    bool delete_event(const std::string& event_id);

    // Load/save events from/to PostgreSQL
    void load_from_db();
    void save_to_db();

    EventLogic();
    ~EventLogic();

private:
    std::vector<Event> events_;
    // Add DB connection or manager reference as needed
};

} // namespace aiworld