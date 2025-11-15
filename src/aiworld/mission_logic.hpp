#pragma once
#include <string>
#include <vector>
#include <optional>
#include <nlohmann/json.hpp>

namespace aiworld {

// Mission data structure
struct Mission {
    std::string mission_id;
    std::string assignee_id;
    nlohmann::json mission_data;
    std::string status;
    std::string created_at;
    std::string updated_at;
};

class MissionLogic {
public:
    // Create a new mission
    std::string create_mission(const std::string& assignee_id, const nlohmann::json& mission_data);

    // Get a mission by ID
    std::optional<Mission> get_mission(const std::string& mission_id);

    // Update mission status
    bool update_mission_status(const std::string& mission_id, const std::string& status);

    // List all missions for an assignee
    std::vector<Mission> list_missions(const std::string& assignee_id);

    // Delete a mission
    bool delete_mission(const std::string& mission_id);

    // Load/save missions from/to PostgreSQL
    void load_from_db();
    void save_to_db();

    MissionLogic();
    ~MissionLogic();

private:
    std::vector<Mission> missions_;
    // Add DB connection or manager reference as needed
};

} // namespace aiworld