# rAthena AI World

A comprehensive AI system for rAthena that enhances gameplay through intelligent agents, dynamic content generation, and adaptive game mechanics.

## Features

### AI Agents
- **Adaptive Balance AI**: Dynamic class and skill balancing
- **Dynamic Reward AI**: Smart reward distribution and dopamine management
- **Competitive Ranking AI**: Intelligent matchmaking and ranking systems
- **Player Engagement AI**: Personalized engagement optimization
- **Adaptive Economy AI**: Dynamic market and economy management
- **Smart Party & Guild AI**: Intelligent group formation and management
- **Personalized AI Coach**: Individual player skill development
- **Emotional Investment AI**: Character attachment and storytelling
- **Dynamic AI Monster**: Adaptive monster behavior and MVPs
- **Social Influence AI**: Community building and social dynamics
- **Adaptive Quest AI**: Dynamic quest generation and adaptation

### Core Systems
- Multi-model AI support (Azure OpenAI, OpenAI GPT-4, DeepSeek)
- P2P Hosting System
  * Distributed map hosting via P2P network
  * Coordinator server for centralized management
  * Dynamic map distribution
  * Secure database synchronization
  * Real-time monitoring system
  * FluxCP-integrated admin panel
  * WARP client patches for P2P support
  * Advanced security measures
  * Automated failover to VPS
  * Performance optimization tools
- Advanced memory management with short-term and long-term memory
- Real-time monitoring and performance optimization
- Comprehensive configuration system
- Extensive testing and diagnostic tools

## Requirements

### System Requirements
- Linux/Unix-based OS (Ubuntu 20.04+ recommended)
- 8GB RAM minimum (16GB+ recommended)
- 4 CPU cores minimum (8+ recommended)
- 20GB free disk space
- Internet connection for AI API access
- Port 5121 and 5122 open for P2P networking

### Software Dependencies
- CMake 3.12+
- GCC 8+ or Clang 7+
- Git
- libcurl4-openssl-dev
- libmysqlclient-dev
- libssl-dev
- rapidjson-dev
- libpcre3-dev
- zlib1g-dev

### AI API Requirements
At least one of:
- Azure OpenAI API key (recommended)
- OpenAI API key
- DeepSeek API key

## Quick Start

1. **Clone the Repository**
   ```bash
   git clone https://github.com/yourusername/rathena-AI-world.git
   cd rathena-AI-world
   ```

2. **Configure Systems**
   ```bash
   # Configure AI system
   cp conf/ai_agents.conf.example conf/ai_agents.conf
   
   # Configure P2P system
   cp conf/p2p/p2p.conf.example conf/p2p/p2p.conf
   cp conf/p2p/host_node.conf.example conf/p2p/host_node.conf
   
   # Edit configuration files with your settings
   ```

3. **Build and Install**
   ```bash
   # Install P2P dependencies
   sudo apt-get install -y \
       libssl-dev \
       libmysqlclient-dev \
       libpcre3-dev \
       zlib1g-dev

   # Using build script (recommended)
   chmod +x build.sh
   ./build.sh --with-p2p

   # Or using make
   make p2p=yes
   make install
   ```

4. **Setup P2P Services**
   ```bash
   # Setup P2P coordinator service
   sudo cp tools/services/p2p-coordinator.service /etc/systemd/system/
   sudo systemctl daemon-reload
   sudo systemctl enable p2p-coordinator
   sudo systemctl start p2p-coordinator

   # Setup monitoring service
   sudo cp tools/services/p2p-monitor.service /etc/systemd/system/
   sudo systemctl enable p2p-monitor
   sudo systemctl start p2p-monitor
   ```

5. **Verify Installation**
   ```bash
   # Check AI system status
   tools/ai_status.sh

   # Check P2P system status
   systemctl status p2p-coordinator
   
   # Monitor AI system
   tools/ai_monitor.sh

   # Monitor P2P network
   http://your-server/p2p/dashboard
   ```

## Configuration

### 1. AI System Configuration
Edit `conf/ai_agents.conf`:
```ini
ai_system: {
    enabled: true
    primary_model: "azure_openai"  // azure_openai, openai_gpt4, or deepseek_v3
}
```

### 2. Enable/Disable AI Agents
```ini
adaptive_balance: { enabled: true }
dynamic_reward: { enabled: true }
// ... configure other agents
```

### 3. P2P System Configuration
Edit `conf/p2p/p2p.conf`:
```yaml
coordinator:
  port: 5121
  db_proxy_port: 5122
  max_connections: 1000

security:
  encryption_enabled: true
  key_rotation_interval: 3600
```

### 4. P2P Host Configuration
Edit `conf/p2p/host_node.conf`:
```yaml
node:
  max_maps: 5
  max_players: 100
  update_interval: 50

resources:
  cpu_limit: 80
  memory_limit: 85
  network_limit: 100
```

### 5. AI Model Configuration
```ini
azure_openai: {
    api_key: "your-api-key"
    endpoint: "your-endpoint"
}
```

## Usage

### Starting the Systems
```bash
# Start with default settings
./map-server --ai-system

# Start with P2P support
./map-server --with-p2p

# Start with specific configuration
./map-server --ai-config=/path/to/config
```

### Monitor and Manage
```bash
# View AI system status
tools/ai_status.sh

# Monitor AI performance
tools/ai_monitor.sh

# Test specific AI agent
tools/test_agent.sh --agent=adaptive_balance

# P2P Management
p2p-control --list-hosts
p2p-control --map-status
p2p-control --performance
p2p-control --security-audit
```

### In-Game Commands
```
# AI Commands
@ai_status             - Show AI system status
@ai_agent <command>    - Control AI agents
@ai_config <setting>   - Adjust configuration
@ai_metrics           - View performance metrics

# P2P Commands
@p2p_status          - Show P2P network status
@p2p_host <command>  - Manage P2P hosting
@p2p_metrics        - View P2P performance
```

## Development

### Build Options
```bash
# Debug build
make debug

# Release build
make release

# Run tests
make test

# Generate documentation
make docs
```

### Adding New Agents
1. Create agent class in `src/ai/agents/`
2. Implement required interfaces
3. Register in `AISystemManager`
4. Add configuration to `ai_agents.conf`
5. Add tests in `tests/ai/agents/`

### Code Style
- Follow C++17 standards
- Use provided clang-format configuration
- Document all public interfaces
- Include unit tests for new features

## Documentation

- [Quick Start Guide](doc/QUICKSTART.md)
- [Configuration Guide](doc/configuration.md)
- [AI Agent Documentation](doc/agents/README.md)
- [P2P System Overview](doc/p2p_system_overview.md)
- [P2P Security Guide](doc/p2p_security.md)
- [P2P Hosting Implementation](doc/p2p_hosting_implementation.md)
- [P2P Monitoring Guide](doc/p2p_monitoring.md)
- [API Reference](doc/api/README.md)
- [Development Guide](doc/development.md)
- [Troubleshooting](doc/troubleshooting.md)

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests
5. Submit a pull request

See [CONTRIBUTING.md](CONTRIBUTING.md) for detailed guidelines.

## License

This project is licensed under the GNU General Public License v3.0 - see the [LICENSE](LICENSE) file for details.

## Support

- GitHub Issues: [Report a bug](https://github.com/yourusername/rathena-AI-world/issues)
- Discord: [Join our community](https://discord.gg/rathena-ai)
- Documentation: [Visit the wiki](https://github.com/yourusername/rathena-AI-world/wiki)
- Email: support@rathena-ai.org

## Acknowledgments

- rAthena Team and Community
- OpenAI and Azure OpenAI Teams
- DeepSeek Team
- WARP P2P Client Team
- All contributors and testers

## Project Status

Current Version: 1.0.0
Status: Active Development
Release Date: March 2025
