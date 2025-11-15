#include <gtest/gtest.h>
#include "../config_loader.hpp"
#include <atomic>
#include <thread>
#include <chrono>

using namespace aiworld;

TEST(ConfigLoader, ReloadCallback) {
    ConfigLoader loader;
    std::atomic<bool> called{false};
    loader.set_reload_callback([&]() { called = true; });
    loader.trigger_reload();
    EXPECT_TRUE(called);
}

TEST(ConfigLoader, SighupHandler) {
    ConfigLoader loader;
    std::atomic<bool> called{false};
    loader.set_reload_callback([&]() { called = true; });
    loader.install_sighup_handler();
#ifdef SIGHUP
    // Send SIGHUP to this process
    std::raise(SIGHUP);
    // Wait a bit for the signal to be handled
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_TRUE(called);
#else
    GTEST_SKIP() << "SIGHUP not supported on this platform";
#endif
}

TEST(ConfigLoader, SupportsHotReload) {
#ifdef SIGHUP
    EXPECT_TRUE(ConfigLoader::supports_hot_reload());
#else
    EXPECT_FALSE(ConfigLoader::supports_hot_reload());
#endif
}