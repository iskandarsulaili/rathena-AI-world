# Quick Start Guide: rAthena AI System Integration

This guide will help you quickly set up and integrate the AI system into your rAthena server.

## Requirements

### System Requirements
- Linux/Unix-based OS (Ubuntu 20.04+ recommended)
- 8GB RAM minimum (16GB+ recommended)
- 4 CPU cores minimum (8+ recommended)
- 20GB free disk space
- Internet connection for AI API access

### Software Requirements
- CMake 3.12+
- GCC 8+ or Clang 7+
- Git
- libcurl4-openssl-dev
- libssl-dev
- rapidjson-dev

### AI API Requirements
You'll need at least one of these API keys:
- Azure OpenAI API key (recommended)
- OpenAI API key
- DeepSeek API key

## Installation Steps

1. **Clone the Repository**
   ```bash
   git clone https://github.com/yourusername/rathena-AI-world.git
   cd rathena-AI-world
   ```

2. **Configure API Keys**
   ```bash
   # Copy example configuration
   cp conf/ai_agents.conf.example conf/ai_agents.conf
   
   # Edit configuration with your API keys
   nano conf/ai_agents.conf
   ```

3. **Build and Install**
   ```bash
   # Make build script executable
   chmod +x build.sh
   
   # Run build script with default settings
   ./build.sh
   
   # Or customize the build
   ./build.sh --prefix=/opt/rathena --ai-model=azure_openai --debug
   ```

4. **Verify Installation**
   ```bash
   # Check if AI system is running
   tail -f log/map_ai.log
   ```

## Quick Configuration

### 1. Enable AI Agents
In `conf/ai_agents.conf`:
```ini
// Enable/disable entire AI system
ai_system: {
    enabled: true
    primary_model: "azure_openai"  // azure_openai, openai_gpt4, or deepseek_v3
}

// Enable specific agents
adaptive_balance: {
    enabled: true
}
dynamic_reward: {
    enabled: true
}
competitive_ranking: {
    enabled: true
}
// ... enable other agents as needed
```

### 2. Configure Memory System
```ini
// Memory system settings
memory: {
    short_term_capacity: 1000
    long_term_capacity: 10000
    short_term_retention: 86400    // 1 day
    long_term_retention: 7776000   // 90 days
}
```

### 3. Performance Settings
```ini
// Performance tuning
performance: {
    max_concurrent_requests: 100
    request_timeout: 30
    batch_size: 32
    cache_duration: 300
}
```

## Testing the Installation

1. **Check AI System Status**
   ```bash
   ./tools/ai_status.sh
   ```

2. **Test Individual Agents**
   ```bash
   ./tools/test_agent.sh --agent=adaptive_balance
   ./tools/test_agent.sh --agent=dynamic_reward
   ```

3. **Monitor Performance**
   ```bash
   ./tools/ai_monitor.sh
   ```

## Common Operations

### Enable/Disable Agents at Runtime
```sql
// In-game command
@ai_agent enable adaptive_balance
@ai_agent disable dynamic_reward
```

### View AI System Status
```sql
@ai_status
```

### Adjust Agent Parameters
```sql
@ai_config set adaptive_balance win_rate_tolerance 5.0
@ai_config set dynamic_reward max_dopamine_hits_per_hour 5
```

### View Agent Metrics
```sql
@ai_metrics adaptive_balance
@ai_metrics dynamic_reward
```

## Troubleshooting

### 1. AI System Not Starting
- Check logs in `log/map_ai.log`
- Verify API keys in configuration
- Ensure all dependencies are installed
- Check system resources

### 2. High Memory Usage
- Reduce memory capacities in configuration
- Decrease batch sizes
- Enable aggressive cleanup

### 3. Slow Response Times
- Check network connectivity
- Reduce concurrent requests
- Enable response caching
- Consider upgrading API tier

### 4. Agent Errors
- Check agent-specific logs
- Verify agent configuration
- Test API connectivity
- Check memory system status

## Getting Help

1. **Documentation**
   - Full documentation in `doc/`
   - Agent-specific guides in `doc/agents/`
   - API reference in `doc/api/`

2. **Support Channels**
   - GitHub Issues
   - Discord Server
   - Forums
   - Email Support

3. **Contributing**
   - See `CONTRIBUTING.md`
   - Development guide in `doc/development.md`
   - Coding standards in `doc/coding_standards.md`

## Next Steps

1. Read the full documentation in `doc/ai_system.md`
2. Configure agents for your specific needs
3. Set up monitoring and alerts
4. Join the community for support and updates
5. Consider contributing improvements back to the project

## Additional Resources

- [Full Documentation](doc/ai_system.md)
- [API Reference](doc/api/README.md)
- [Configuration Guide](doc/configuration.md)
- [Performance Tuning](doc/performance.md)
- [Security Guide](doc/security.md)
- [Development Guide](doc/development.md)
- [Troubleshooting Guide](doc/troubleshooting.md)

## Support

For help and support:
- GitHub Issues: [rathena-AI-world Issues](https://github.com/yourusername/rathena-AI-world/issues)
- Discord: [Join Server](https://discord.gg/rathena-ai)
- Email: support@rathena-ai.org