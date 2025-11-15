#include <gtest/gtest.h>
#include "../admin_handler.hpp"
#include <thread>
#include <chrono>
#include <zmq.h>

using namespace aiworld;

TEST(AdminHandler, RegisterAndHandleCommand) {
    AdminHandler handler;
    handler.register_command("echo", [](const std::string& arg) { return "ECHO: " + arg; });
    std::string resp = handler.handle_command("echo", "hello");
    EXPECT_EQ(resp, "ECHO: hello");
    EXPECT_EQ(handler.handle_command("unknown", ""), "Unknown admin command: unknown");
}

TEST(AdminHandler, ReloadCallback) {
    AdminHandler handler;
    std::atomic<bool> called{false};
    handler.set_reload_callback([&]() { called = true; });
    // Simulate reload command
    handler.register_command("reload", [&](const std::string&) {
        handler.set_reload_callback([&]() { called = true; });
        return "Config reload triggered.";
    });
    handler.handle_command("reload", "");
    EXPECT_TRUE(called);
}

TEST(AdminHandler, ZeroMQAdminLoop) {
    // This is a basic test for the admin loop; full integration test would require a real ZMQ client
    void* ctx = zmq_ctx_new();
    AdminHandler handler;
    std::thread t([&]() {
        handler.run_admin_loop(ctx, "tcp://127.0.0.1:5556");
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    void* sock = zmq_socket(ctx, ZMQ_REQ);
    zmq_connect(sock, "tcp://127.0.0.1:5556");
    std::string cmd = "echo test";
    zmq_send(sock, cmd.data(), cmd.size(), 0);
    char buf[256] = {0};
    int n = zmq_recv(sock, buf, sizeof(buf) - 1, 0);
    EXPECT_GT(n, 0);
    std::string resp(buf, n);
    EXPECT_EQ(resp, "Unknown admin command: echo");
    zmq_close(sock);
    handler.~AdminHandler();
    t.join();
    zmq_ctx_term(ctx);
}