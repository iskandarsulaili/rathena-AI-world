# rAthena AI World

## Overview
Advanced AI-driven gameplay enhancement and P2P network security system for rAthena, providing intelligent NPCs, dynamic world evolution, and robust protection.

## ✨ Features

### AI Agents

#### Mayor AI Agent
- Weekly server statistics analysis
- Dynamic event creation based on player behavior
- Player retention and acquisition strategies
- Comprehensive reporting on server health and trends
- Tailored events for different player segments
- Event creation system with various types and difficulties
- Integration with other AI systems for coordinated experiences

#### AI Integration
- Support for Azure OpenAI, OpenAI, and DeepSeek V3 providers
- LangChain integration for context memory management
- Dynamic configuration systems for each AI character
- Fallback mechanisms for offline operation

### P2P and Network Security
- Thread-safe packet monitoring
- Resource usage tracking
- Performance metrics collection
- Host statistics monitoring
- P2P hosting system with host scoring
- Host eligibility validation
- Real-time gaming experience optimization
- Data synchronization with Redis/PostgreSQL support
- SQL injection prevention
- Memory safety improvements
- Basic DDoS protection
- Cross-platform security implementation
- WARP P2P Security System integration

### Monitoring System
- Real-time connection tracking
- Resource usage monitoring
- Performance metrics collection
- Security event logging
- Host status tracking
- Advanced P2P network analytics

### Development Tools
- Cross-platform build system
- Comprehensive test suite
- Memory leak detection
- Thread safety verification
- Performance testing
- Build verification scripts
- Client update utilities

## 🚀 Getting Started

### Prerequisites
- CMake 3.15+
- C++17 compliant compiler
- MySQL 5.7+
- libconfig
- nlohmann-json
- Redis (optional, for P2P data sync)
- PostgreSQL (optional, for P2P data sync)
- Azure OpenAI, OpenAI, or DeepSeek API key (for AI features)

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

### AI Providers Configuration
```ini
ai_providers: {
    azure_openai: {
        enabled: true
        api_key: "your-api-key"
        endpoint: "your-endpoint"
        deployment: "gpt-4o"
        api_version: "2023-05-15"
    }
    openai: {
        enabled: true
        api_key: "your-api-key"
        model: "gpt-4o"
    }
    deepseek: {
        enabled: true
        api_key: "your-api-key"
        model: "deepseek-v3"
    }
}
```

### Mayor Configuration
```ini
ai_mayor_enabled: true
ai_mayor_provider: "azure_openai"
ai_mayor_model: "gpt-4o"
ai_mayor_analysis_frequency: "weekly"
```

### Network Monitor Settings
```ini
network_monitor: {
    enabled: true
    update_interval: 60
    cleanup_interval: 300
}
```

### Security Settings
```ini
security: {
    ddos_protection: {
        enabled: true
        packet_rate: 1000
        connection_rate: 100
    }
}
```

### P2P Data Sync Configuration
```ini
p2p_data_sync: {
    enabled: true
    redis_enabled: false
    postgresql_enabled: false
    sync_interval: 300
}
```

## 📊 Monitoring

### AI System Monitoring
- Character interactions
- Player engagement metrics
- Event participation
- AI response performance
- Memory usage statistics

### Performance Metrics
- Packet rates
- Bandwidth usage
- Connection counts
- Resource utilization
- Security events

### Host Statistics
- CPU usage
- Memory usage
- Network usage
- Connected players
- System status

## 🔒 Security Features

### Protection
- SQL injection prevention
- Memory safety
- Thread safety
- Resource management
- Input validation

### Monitoring
- Security event logging
- Resource tracking
- Connection monitoring
- Performance tracking
- Alert generation

## 🧪 Testing

### Running Tests
```bash
# Full test suite
cd build/src/test
./network_tests

# Specific tests
./network_tests --gtest_filter=NetworkMonitorTest.*
./network_tests --gtest_filter=SyncTest.*
./network_tests --gtest_filter=P2PConfigParserTest.*
```

### Memory Testing
```bash
# Linux (Valgrind)
valgrind --leak-check=full ./network_tests

# Windows (Dr. Memory)
drmemory.exe -light -- network_tests.exe
```

## 🔜 Upcoming Features

### Security
- TLS implementation
- Advanced DDoS protection
- Enhanced P2P host validation
- Certificate management
- Packet encryption

### Monitoring
- Real-time dashboard using [FluxCP-AI-world-p2p-hosting](https://github.com/iskandarsulaili/FluxCP-AI-world-p2p-hosting)

### Performance
- Advanced caching
- Load balancing
- Dynamic scaling
- Resource optimization

## 🤝 Contributing

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/amazing-feature`)
3. Run tests (`./test_build.sh` or `test_build.bat`)
4. Commit changes (`git commit -m 'Add amazing feature'`)
5. Push to branch (`git push origin feature/amazing-feature`)
6. Open a Pull Request

## 📄 License

This project is licensed under the GNU General Public License v3.0 - see the [LICENSE](LICENSE) file for details.
