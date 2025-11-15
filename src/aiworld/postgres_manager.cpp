#include "postgres_manager.hpp"
#include "aiworld_utils.hpp"
#include <cstdlib>
#include <stdexcept>
#include <sstream>

namespace aiworld {

namespace {
std::string get_env(const char* var) {
    const char* val = std::getenv(var);
    if (!val) throw std::runtime_error(std::string("Missing required environment variable: ") + var);
    return std::string(val);
}
}

std::string PostgresManager::build_conninfo_from_env() {
    std::ostringstream oss;
    oss << "host=" << get_env("PGHOST")
        << " port=" << get_env("PGPORT")
        << " dbname=" << get_env("PGDATABASE")
        << " user=" << get_env("PGUSER")
        << " password=" << get_env("PGPASSWORD");
    return oss.str();
}

std::unique_ptr<PostgresManager> PostgresManager::create_from_env() {
    std::string conninfo = build_conninfo_from_env();
    log_info("PostgresManager: Connecting with: " + redact_conninfo(conninfo));
    return std::make_unique<PostgresManager>(conninfo);
}

std::string PostgresManager::redact_conninfo(const std::string& conninfo) {
    auto pos = conninfo.find("password=");
    if (pos == std::string::npos) return conninfo;
    auto end = conninfo.find(' ', pos);
    std::string redacted = conninfo;
    redacted.replace(pos + 9, (end == std::string::npos ? redacted.size() : end) - (pos + 9), "******");
    return redacted;
}

PostgresManager::PostgresManager(const std::string& conninfo)
    : conninfo_(conninfo), conn_(nullptr) {
    reconnect();
}

void PostgresManager::reconnect() {
    std::lock_guard<std::mutex> lock(conn_mutex_);
    conn_.reset();
    try {
        conn_ = std::make_unique<pqxx::connection>(conninfo_);
        if (!conn_->is_open()) {
            throw std::runtime_error("PostgresManager: Connection failed (is_open() == false)");
        }
        log_info("PostgresManager: Connected to PostgreSQL: " + redact_conninfo(conninfo_));
    } catch (const std::exception& e) {
        log_error(std::string("PostgresManager: Connection error: ") + e.what());
        throw;
    }
}

bool PostgresManager::is_connected() const {
    std::lock_guard<std::mutex> lock(conn_mutex_);
    return conn_ && conn_->is_open();
}

pqxx::connection& PostgresManager::connection() {
    std::lock_guard<std::mutex> lock(conn_mutex_);
    if (!conn_ || !conn_->is_open()) {
        throw std::runtime_error("PostgresManager: Not connected");
    }
    return *conn_;
}

std::string PostgresManager::current_conninfo() const {
    return redact_conninfo(conninfo_);
}

} // namespace aiworld