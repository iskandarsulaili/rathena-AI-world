/**
 * aiworld_server.cpp
 * Standalone AIWorld server process for rAthena (ZeroMQ IPC)
 * Runs independently, like char, login, and map servers.
 * Handles all AI logic, mission orchestration, and entity management.
 */

#include "aiworld_ipc.hpp"
#include "aiworld_utils.hpp"
#include <nlohmann/json.hpp>
#include <iostream>
#include <thread>
#include <atomic>
#include <vector>
#include <zmq.h>

using namespace aiworld;

class AIWorldServer {
public:
    AIWorldServer(const std::string& endpoint)
        : zmq_context(nullptr), zmq_socket(nullptr), running(false), server_endpoint(endpoint) {}

    ~AIWorldServer() {
        stop();
    }

    bool start() {
        zmq_context = zmq_ctx_new();
        if (!zmq_context) {
            log_error("AIWorldServer: ZeroMQ context creation failed.");
            return false;
        }
        zmq_socket = ::zmq_socket(zmq_context, ZMQ_REP);
        if (!zmq_socket) {
            log_error("AIWorldServer: ZeroMQ socket creation failed.");
            return false;
        }
        if (zmq_bind(zmq_socket, server_endpoint.c_str()) != 0) {
            log_error("AIWorldServer: ZeroMQ bind failed: " + std::string(zmq_strerror(zmq_errno())));
            return false;
        }
        running = true;
        log_info("AIWorldServer started and listening on " + server_endpoint);
        server_thread = std::thread(&AIWorldServer::main_loop, this);
        return true;
    }

    void stop() {
        running = false;
        if (server_thread.joinable()) server_thread.join();
        if (zmq_socket) zmq_close(zmq_socket);
        if (zmq_context) zmq_ctx_term(zmq_context);
        log_info("AIWorldServer stopped.");
    }

private:
    void* zmq_context;
    void* zmq_socket;
    std::atomic<bool> running;
    std::thread server_thread;
    std::string server_endpoint;

    void main_loop() {
        while (running) {
            zmq_msg_t zmq_msg;
            zmq_msg_init(&zmq_msg);
            int rc = zmq_msg_recv(&zmq_msg, zmq_socket, 0);
            if (rc < 0) {
                zmq_msg_close(&zmq_msg);
                continue;
            }
            std::string msg_str(static_cast<char*>(zmq_msg_data(&zmq_msg)), zmq_msg_size(&zmq_msg));
            zmq_msg_close(&zmq_msg);

            log_info("AIWorldServer received message: " + msg_str);

            // Parse message and dispatch
            nlohmann::json response_json;
            try {
                nlohmann::json req = nlohmann::json::parse(msg_str);
                int msg_type = req.value("message_type", 0);
                std::string corr_id = req.value("correlation_id", "");
                nlohmann::json payload = req.value("payload", nlohmann::json::object());

                // Core logic routers
                static EntityLogic entity_logic;
                static MissionLogic mission_logic;
                static EventLogic event_logic;

                response_json["message_type"] = msg_type;
                response_json["correlation_id"] = corr_id;

                switch (static_cast<IPCMessageType>(msg_type)) {
                    case IPCMessageType::ENTITY_STATE_SYNC: {
                        // Upsert or get entity state
                        std::string entity_id = payload.value("entity_id", "");
                        std::string entity_type = payload.value("entity_type", "");
                        nlohmann::json state = payload.value("state", nlohmann::json::object());
                        if (!entity_id.empty() && !entity_type.empty()) {
                            entity_logic.upsert_entity(entity_id, entity_type, state);
                            response_json["payload"] = {{"status", "entity upserted"}, {"entity_id", entity_id}};
                        } else if (!entity_id.empty()) {
                            auto ent = entity_logic.get_entity(entity_id);
                            if (ent) {
                                response_json["payload"] = {{"status", "entity found"}, {"entity", {
                                    {"entity_id", ent->entity_id},
                                    {"entity_type", ent->entity_type},
                                    {"state", ent->state},
                                    {"created_at", ent->created_at},
                                    {"updated_at", ent->updated_at}
                                }}};
                            } else {
                                response_json["payload"] = {{"status", "not found"}, {"entity_id", entity_id}};
                            }
                        } else {
                            response_json["payload"] = {{"error", "Missing entity_id or entity_type"}};
                        }
                        break;
                    }
                    case IPCMessageType::MISSION_ASSIGNMENT: {
                        // Create or get mission
                        std::string assignee_id = payload.value("assignee_id", "");
                        nlohmann::json mission_data = payload.value("mission_data", nlohmann::json::object());
                        if (!assignee_id.empty() && !mission_data.empty()) {
                            std::string mission_id = mission_logic.create_mission(assignee_id, mission_data);
                            response_json["payload"] = {{"status", "mission created"}, {"mission_id", mission_id}};
                        } else if (payload.contains("mission_id")) {
                            std::string mission_id = payload.value("mission_id", "");
                            auto m = mission_logic.get_mission(mission_id);
                            if (m) {
                                response_json["payload"] = {{"status", "mission found"}, {"mission", {
                                    {"mission_id", m->mission_id},
                                    {"assignee_id", m->assignee_id},
                                    {"mission_data", m->mission_data},
                                    {"status", m->status},
                                    {"created_at", m->created_at},
                                    {"updated_at", m->updated_at}
                                }}};
                            } else {
                                response_json["payload"] = {{"status", "not found"}, {"mission_id", mission_id}};
                            }
                        } else {
                            response_json["payload"] = {{"error", "Missing assignee_id or mission_data"}};
                        }
                        break;
                    }
                    case IPCMessageType::EVENT_NOTIFICATION: {
                        // Create or get event
                        std::string event_type = payload.value("event_type", "");
                        nlohmann::json event_data = payload.value("event_data", nlohmann::json::object());
                        if (!event_type.empty() && !event_data.empty()) {
                            std::string event_id = event_logic.create_event(event_type, event_data);
                            response_json["payload"] = {{"status", "event created"}, {"event_id", event_id}};
                        } else if (payload.contains("event_id")) {
                            std::string event_id = payload.value("event_id", "");
                            auto e = event_logic.get_event(event_id);
                            if (e) {
                                response_json["payload"] = {{"status", "event found"}, {"event", {
                                    {"event_id", e->event_id},
                                    {"event_type", e->event_type},
                                    {"event_data", e->event_data},
                                    {"created_at", e->created_at}
                                }}};
                            } else {
                                response_json["payload"] = {{"status", "not found"}, {"event_id", event_id}};
                            }
                        } else {
                            response_json["payload"] = {{"error", "Missing event_type or event_data"}};
                        }
                        break;
                    }
                    case IPCMessageType::AI_ACTION_REQUEST: {
                        // Placeholder for AI action logic (extend as needed)
                        response_json["payload"] = {{"status", "AI action request received"}, {"details", payload}};
                        break;
                    }
                    case IPCMessageType::HEARTBEAT: {
                        response_json["payload"] = {{"status", "alive"}};
                        break;
                    }
                    default: {
                        response_json["payload"] = {{"error", "Unknown or unimplemented message_type"}, {"type", msg_type}};
                        break;
                    }
                }
            } catch (const std::exception& e) {
                response_json["message_type"] = 99;
                response_json["payload"] = {{"error", e.what()}};
            }

            std::string resp_str = response_json.dump();
            zmq_msg_t resp_msg;
            zmq_msg_init_size(&resp_msg, resp_str.size());
            std::memcpy(zmq_msg_data(&resp_msg), resp_str.data(), resp_str.size());
            zmq_msg_send(&resp_msg, zmq_socket, 0);
            zmq_msg_close(&resp_msg);
        }
    }
};

int main(int argc, char* argv[]) {
    std::string endpoint = "tcp://*:5555";
    if (argc > 1) {
        endpoint = argv[1];
    }
    AIWorldServer server(endpoint);
    if (!server.start()) {
        log_error("Failed to start AIWorldServer.");
        return 1;
    }
    // Wait for server to finish (Ctrl+C to exit)
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return 0;
}