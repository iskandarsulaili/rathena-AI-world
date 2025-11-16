# P2P Coordinator Server

## Overview

The P2P Coordinator is a standalone, production-grade C++ server responsible for hybrid P2P mesh management, multi-core entity simulation, protocol handling, security/anti-cheat, and resource management for the rAthena-AI-world MMO architecture.

- **Multi-core/worker pool:** Scales with CPU cores for high concurrency.
- **Hybrid P2P mesh:** Manages peer discovery, interest-based topology, and direct peer connections.
- **Protocol handling:** Supports QUIC (datagrams/streams), gRPC, and NATS/JetStream for all MMO traffic flows.
- **Security/anti-cheat:** Implements signature validation, anomaly detection, peer reputation, and resource monitoring.
- **Diagnostics:** Structured JSON logging, CLI diagnostics, and robust error handling.

## Build Instructions

```sh
cd rathena-AI-world/src/p2p-coordinator
mkdir build && cd build
cmake ..
make
```

## Run Instructions

```sh
./p2p_coordinator [--config path] [--disable-p2p] [--status] [--help]
```

- `--config path` : Path to config file (default: `config/p2p_coordinator.conf`)
- `--disable-p2p` : Start with P2P features disabled (multi-core/worker pool always enabled)
- `--status`      : Output diagnostics and exit
- `--help`        : Show usage

## Integration

- **Servers:** Integrates with rAthena-AI-world servers (map, char, aiworld, etc.) via protocol endpoints.
- **Clients:** WARP-p2p-client/P2P-DLL should point `coordinator_url` in its config to this server.
- **Optional P2P:** P2P can be enabled/disabled via config or CLI; legacy features remain functional.

## Logging & Diagnostics

- All events, errors, and metrics are logged in structured JSON to stdout.
- Use `--status` for a quick health/config report.
- All critical errors are surfaced and logged.

## Sample Config

See `config/p2p_coordinator.conf` for all available options and comments.

## Deployment

- Add this server to your orchestration scripts (e.g., `start-services.sh`).
- Ensure the server is started before clients and other servers that depend on P2P mesh features.

## Contact

For architecture details, see [`P2P-multi-CPU.md`](../../P2P-multi-CPU.md).