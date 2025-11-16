// protocol_server.hpp
// Unified protocol server for QUIC (msquic), gRPC (grpc-cpp), and NATS/JetStream (nats-cpp) event handling

#pragma once

#include <thread>
#include <vector>
#include <atomic>
#include <memory>
#include <functional>
#include <string>
#include <nlohmann/json.hpp>
#include "protocol_handler.hpp"
#include "metrics_exporter.hpp"

class ProtocolServer {
public:
    ProtocolServer(std::shared_ptr<ProtocolHandler> handler, std::shared_ptr<MetricsExporter> metrics);
    ~ProtocolServer();

    // Start all protocol listeners and event loops
    void start();

    // Stop all listeners and threads
    void stop();

    // Thread-safe event dispatch
    void dispatch_event(ProtocolType type, const std::string& payload);

private:
    void quic_receive_loop();
    void grpc_receive_loop();
    void nats_receive_loop();

    std::shared_ptr<ProtocolHandler> handler_;
    std::shared_ptr<MetricsExporter> metrics_;
    std::vector<std::thread> threads_;
    std::atomic<bool> running_;
};