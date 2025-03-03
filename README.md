# rAthena P2P Network

Enhanced P2P network system for rAthena, providing distributed hosting capabilities with robust security features.

## ✨ Features

### P2P Network Infrastructure
- Distributed map hosting
- Host node management
- Load balancing
- Resource optimization
- Network latency reduction
- Performance monitoring

### Network Security
- Host scoring and validation
- SQL injection prevention
- Memory safety improvements
- DDoS protection
- Thread safety verification
- WARP P2P Security integration
- Cross-platform security

### Monitoring System
- Real-time connection tracking
- Resource usage monitoring
- Host status tracking
- Performance metrics collection
- Security event logging

## 🚀 Getting Started

### Prerequisites
- CMake 3.15+
- C++17 compiler
- MySQL 5.7+
- libconfig
- nlohmann-json
- Redis (optional)
- PostgreSQL (optional)

### Building

#### Linux
```bash
# Build and test
./test_build.sh

# Build only
mkdir build && cd build
cmake -DBUILD_TESTING=ON ..
make
```

#### Windows
```batch
# Build and test
test_build.bat

# Build only
mkdir build && cd build
cmake -DBUILD_TESTING=ON -G "Visual Studio 16 2019" ..
cmake --build . --config Release
```

## 🔧 Configuration

### P2P Host Settings
```ini
p2p_hosting: {
    enabled: true
    min_score: 80
    sync_interval: 300
    ddos_protection: {
        enabled: true
        packet_rate: 1000
        connection_rate: 100
    }
}
```

### Security Settings
```ini
security: {
    sql_injection_prevention: true
    memory_checks: true
    packet_validation: true
    thread_safety: true
}
```

## 📚 Documentation
- [Database Structure](doc/database_structure.md)
- [P2P Hosting Implementation](doc/p2p_hosting_implementation.md)
- [P2P Security](doc/p2p_security.md)
- [System Overview](doc/p2p_system_overview.md)
- [Monitoring Setup](doc/monitoring_setup.md)

## 🛠️ Related Projects
- [FluxCP P2P Hosting](https://github.com/iskandarsulaili/FluxCP-AI-world-p2p-hosting) - Control panel with P2P hosting management
- [WARP P2P Client](https://github.com/Neo-Mind/WARP) - Client-side P2P implementation

## 🤝 Contributing
1. Fork the repository
2. Create feature branch (`git checkout -b feature/amazing-feature`)
3. Run tests (`./test_build.sh` or `test_build.bat`)
4. Commit changes (`git commit -m 'Add amazing feature'`)
5. Push to branch (`git push origin feature/amazing-feature`)
6. Open a Pull Request

## 📄 License
GNU General Public License v3.0 - see the [LICENSE](LICENSE) file for details.
