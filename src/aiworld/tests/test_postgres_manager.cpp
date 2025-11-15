#include <gtest/gtest.h>
#include "../postgres_manager.hpp"
#include <cstdlib>

using namespace aiworld;

TEST(PostgresManager, EnvVarFailFast) {
    // Backup and unset env vars
    const char* vars[] = {"PGHOST", "PGPORT", "PGDATABASE", "PGUSER", "PGPASSWORD"};
    std::vector<std::string> old_vals;
    for (const char* var : vars) {
        const char* val = std::getenv(var);
        old_vals.push_back(val ? val : "");
        unsetenv(var);
    }
    // Should throw
    EXPECT_THROW({
        PostgresManager::create_from_env();
    }, std::runtime_error);

    // Restore env vars
    for (size_t i = 0; i < 5; ++i) {
        if (!old_vals[i].empty()) setenv(vars[i], old_vals[i].c_str(), 1);
    }
}

TEST(PostgresManager, RedactConninfo) {
    std::string conninfo = "host=localhost port=5432 dbname=test user=test password=secret";
    std::string redacted = PostgresManager::redact_conninfo(conninfo);
    EXPECT_NE(conninfo, redacted);
    EXPECT_NE(redacted.find("password=******"), std::string::npos);
}

TEST(PostgresManager, ConnectionLifecycle) {
    // These env vars must be set for this test to pass
    if (!std::getenv("PGHOST") || !std::getenv("PGPORT") || !std::getenv("PGDATABASE") ||
        !std::getenv("PGUSER") || !std::getenv("PGPASSWORD")) {
        GTEST_SKIP() << "PostgreSQL env vars not set for integration test";
    }
    auto mgr = PostgresManager::create_from_env();
    EXPECT_TRUE(mgr->is_connected());
    auto& conn = mgr->connection();
    EXPECT_TRUE(conn.is_open());
    EXPECT_NO_THROW(mgr->reconnect());
}