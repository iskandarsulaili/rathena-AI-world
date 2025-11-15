#pragma once
#include <pqxx/pqxx>
#include <string>
#include <memory>
#include <mutex>
#include <optional>

namespace aiworld {

class PostgresManager {
public:
    // Throws std::runtime_error if any required env var is missing
    static std::unique_ptr<PostgresManager> create_from_env();

    // Redacts password in logs
    static std::string redact_conninfo(const std::string& conninfo);

    // Returns true if connected
    bool is_connected() const;

    // Get a reference to the connection (thread-safe)
    pqxx::connection& connection();

    // Reconnect (e.g., after config reload)
    void reconnect();

    // Get current connection info (redacted)
    std::string current_conninfo() const;

    // For testability: allow injection of connection string
    explicit PostgresManager(const std::string& conninfo);

private:
    std::string conninfo_;
    std::unique_ptr<pqxx::connection> conn_;
    std::mutex conn_mutex_;
    void check_env_or_throw(const char* var);
    static std::string build_conninfo_from_env();
};

} // namespace aiworld