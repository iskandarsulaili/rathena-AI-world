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

### Software Dependencies
- CMake 3.12+
- GCC 8+ or Clang 7+
- Git
- libcurl4-openssl-dev
- libssl-dev
- rapidjson-dev

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

2. **Configure AI System**
   ```bash
   cp conf/ai_agents.conf.example conf/ai_agents.conf
   # Edit conf/ai_agents.conf with your API keys and preferences
   ```

3. **Build and Install**
   ```bash
   # Using build script (recommended)
   chmod +x build.sh
   ./build.sh

   # Or using make
   make
   make install
   ```

4. **Verify Installation**
   ```bash
   # Check AI system status
   tools/ai_status.sh

   # Monitor AI system
   tools/ai_monitor.sh
   ```

## Configuration

### 1. Basic Configuration
Edit `conf/ai_agents.conf`:
```ini
ai_system: {
    enabled: true
    primary_model: "azure_openai"  // azure_openai, openai_gpt4, or deepseek_v3
}
```

### 2. Enable/Disable Agents
```ini
adaptive_balance: { enabled: true }
dynamic_reward: { enabled: true }
// ... configure other agents
```

### 3. AI Model Configuration
```ini
azure_openai: {
    api_key: "your-api-key"
    endpoint: "your-endpoint"
}
```

## Usage

### Start the AI System
```bash
# Start with default settings
./map-server --ai-system

# Start with specific configuration
./map-server --ai-config=/path/to/config
```

### Monitor and Manage
```bash
# View system status
tools/ai_status.sh

# Monitor performance
tools/ai_monitor.sh

# Test specific agent
tools/test_agent.sh --agent=adaptive_balance
```

### In-Game Commands
```
@ai_status             - Show AI system status
@ai_agent <command>    - Control AI agents
@ai_config <setting>   - Adjust configuration
@ai_metrics           - View performance metrics
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
- All contributors and testers

## Project Status

Current Version: 1.0.0
Status: Active Development
Release Date: March 2025
