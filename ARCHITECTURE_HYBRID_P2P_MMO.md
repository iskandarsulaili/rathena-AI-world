# rAthena Hybrid P2P MMO - High Level Architecture

## Overview

This document provides a comprehensive, codebase-driven high-level architecture for the rAthena Hybrid P2P MMO system as currently implemented. It covers all major server and client integration points, protocol flows, core modules, and runtime controls, and is based strictly on the actual code present in the repository.

---

## System Layers

```mermaid
flowchart TD
    %% Client Layer
    C1[RO Client Executable]
    C2[P2P Network DLL<br/>(WARP-p2p-client/P2P-DLL)]
    C3[P2P Config (JSON)]
    C4[WARP Patcher]
    C1 -->|DLL Injection / Patch| C2
    C2 -->|WebRTC, QUIC, Fallback| C1
    C2 -->|Config, Enable/Disable, Fallback| C3

    %% Edge Region Layer
    E1[Regional Gateway<br/>(QUIC/WebSocket)]
    E2[Worker Pool<br/>(map-server, char-server, login-server)]
    E3[DragonflyDB]
    E4[P2P Coordinator Service]
    E2 -->|Entity State, Simulation, Event Queues| E2
    E2 -->|P2P/Legacy Protocols| E1
    E2 -->|Session/State| E3
    E2 -->|P2P Mesh/Coordination| E4

    %% Global Core Layer
    G1[Global Directory (etcd)]
    G2[State Bus (NATS JetStream)]
    G3[Persistent DB (PostgreSQL 17)]
    G4[Global Orchestrator (AI, Events)]
    G1 -->|Ownership, Bootstrap| E2
    G2 -->|Event Streaming| E2
    G3 -->|ACID Transactions| E2
    G4 -->|World Events, AI| G1

    %% Cross-layer Protocols
    C1 -->|QUIC/UDP, WebRTC| E1
    C2 -->|WebSocket/QUIC| E4
    E1 -->|QUIC Streams| E2
    E2 -->|gRPC, NATS, TCP| G1
    E2 -->|gRPC, NATS, TCP| G2
    E2 -->|gRPC, NATS, TCP| G3
    E2 -->|gRPC, NATS, TCP| G4
```

---

## Core Server Modules

- **common/worker_pool.hpp/cpp**: Multi-threaded worker pool for per-core simulation and event processing.
- **common/entity_simulation_worker.hpp/cpp**: ECS/actor-style simulation worker for entity state and tick logic.
- **common/entity_state.hpp**: Unified entity state for ECS/actor simulation.
- **common/arena_allocator.hpp/cpp**: Arena allocator for high-performance entity pools.
- **common/p2p_runtime_flags.hpp/cpp**: Runtime P2P enable/disable and fallback flags for all services.
- **common/p2p_runtime_flags_test.cpp**: Test/verification for P2P runtime flags.
- **common/p2p_runtime_flags_compat_test.cpp**: Test/verification for legacy, AI, and ML compatibility.
- **common/p2p_runtime_flags_atomic_commit_test.cpp**: Test/verification for atomic commit and thread safety.
- **common/arena_allocator_test.cpp**: Test/verification for arena allocator.
- **common/test_all.sh**: Run all core module tests.

---

## Protocol & Runtime Controls

- **P2P Enable/Disable**: Controlled per-service and per-zone/region at runtime via `p2p_runtime_flags`.
- **Fallback**: Automatic fallback to server-only mode on error or when P2P is disabled.
- **Protocol Bridging**: Message translation and routing for both legacy and P2P-enabled clients.
- **Event/Message Queues**: Thread-safe, per-worker, per-entity event and message queues.
- **Atomicity**: All state changes, protocol switches, and fallback operations are atomic and thread-safe.

---

## End-to-End Flow

1. **Client Launch**: RO client is patched with WARP, loads P2P DLL, and reads config.
2. **P2P Negotiation**: DLL attempts to establish P2P mesh via WebRTC/QUIC, with fallback to server if needed.
3. **Server Simulation**: rAthena worker pool and ECS/actor modules process entity state, simulation, and events.
4. **Protocol Routing**: Server routes packets and events to P2P or legacy clients as appropriate, using runtime flags.
5. **Fallback & Compatibility**: If P2P fails or is disabled, all gameplay, AI, and ML features continue via legacy server mode.
6. **Monitoring & Verification**: All modules are covered by atomic commit, compatibility, and fallback tests.

---

## Key Guarantees

- **No server-side DLL dependency**: All P2P logic on the client is protocol-driven; server only needs to support protocol compatibility.
- **Full legacy compatibility**: All gameplay, AI, and ML features work with or without P2P.
- **Atomic, testable, and production-grade**: All modules are atomic, thread-safe, and covered by tests.

---

## Next Steps

- Continue protocol monitoring and update server logic as new client features are released.
- Extend ECS/actor and arena allocator logic for new entity types and simulation needs.
- Maintain and expand test/verification coverage as the system evolves.

---