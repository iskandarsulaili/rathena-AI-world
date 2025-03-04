# rAthena AI System Development Guide

This guide provides detailed information for developers who want to understand, modify, or contribute to the rAthena AI system.

## Code Organization

```
rathena-AI-world/
├── src/ai/                    # AI system source code
│   ├── AISystemManager.*      # Core system manager
│   ├── memory/               # Memory management system
│   ├── model/                # AI model interfaces
│   └── agents/               # Individual AI agents
├── include/ai/               # Public headers
├── conf/                     # Configuration files
├── tools/                    # Utility scripts
├── tests/                    # Test suite
└── doc/                      # Documentation
```

## Development Setup

### 1. Development Environment
```bash
# Required development tools
sudo apt install build-essential cmake git clang-format clang-tidy
sudo apt install libcurl4-openssl-dev libssl-dev rapidjson-dev

# Optional tools
sudo apt install doxygen graphviz ccache gdb valgrind
```

### 2. Code Style
We follow a modified Google C++ style guide:

```cpp
// Example class declaration
class MyClass {
public:
    explicit MyClass(int value);
    void doSomething();

private:
    int m_value;
};

// Function implementation
void MyClass::doSomething() {
    if (m_value > 0) {
        // Do something
    }
}
```

Use the provided `.clang-format` configuration:
```bash
# Format code
make format

# Check formatting
make format-check
```

## Development Workflow

### 1. Creating a New Agent

```cpp
// src/ai/agents/MyNewAgent.hpp
class MyNewAgent : public BaseAgent {
public:
    MyNewAgent(AIModelManager& model_manager, MemoryManager& memory_manager);
    
    // Required interface implementations
    bool initialize() override;
    void update(time_t current_time) override;
    void shutdown() override;
    
private:
    // Agent-specific members
};
```

### 2. Memory Management

```cpp
// Example of proper memory usage
void MyNewAgent::processData(const DataType& data) {
    // Store in short-term memory
    auto memory_id = memory_manager.storeMemory(
        getAgentId(),
        "process_data",
        data.serialize(),
        MemoryType::SHORT_TERM,
        MemoryPriority::MEDIUM
    );
    
    // Use memory reference
    auto memory = memory_manager.getMemory(memory_id);
    if (memory) {
        // Process memory
    }
}
```

### 3. AI Model Integration

```cpp
// Example of AI model usage
AIResponse MyNewAgent::getAIResponse(const std::string& prompt) {
    AIRequest request;
    request.prompt = prompt;
    request.agent_id = getAgentId();
    request.priority = 1;
    
    return model_manager.sendRequest(request);
}
```

### 4. Configuration Integration

```cpp
// Example of configuration integration
bool MyNewAgent::loadConfig() {
    Config& cfg = *config_manager;
    
    // Load settings with defaults
    m_enabled = cfg.get_bool("my_agent/enabled", false);
    m_update_interval = cfg.get_int("my_agent/update_interval", 300);
    m_threshold = cfg.get_float("my_agent/threshold", 0.5f);
    
    return true;
}
```

## Testing

### 1. Unit Tests

```cpp
// tests/ai/agents/MyNewAgentTest.cpp
TEST_CASE("MyNewAgent basic functionality") {
    AIModelManager model_manager;
    MemoryManager memory_manager;
    MyNewAgent agent(model_manager, memory_manager);
    
    REQUIRE(agent.initialize());
    
    SECTION("Test specific functionality") {
        // Test implementation
    }
}
```

### 2. Integration Tests

```cpp
// tests/integration/MyNewAgentIntegration.cpp
TEST_CASE("MyNewAgent system integration") {
    AISystemManager& system = AISystemManager::getInstance();
    auto* agent = system.getAgent<MyNewAgent>();
    
    REQUIRE(agent != nullptr);
    
    // Test system interaction
}
```

## Performance Considerations

1. **Memory Management**
   - Use smart pointers for ownership
   - Minimize memory allocations in hot paths
   - Implement proper cleanup in destructors

2. **AI Model Usage**
   - Cache frequently used responses
   - Batch similar requests when possible
   - Implement request prioritization

3. **Thread Safety**
   - Use proper synchronization mechanisms
   - Avoid shared state when possible
   - Document thread safety requirements

## Debug Tools

### 1. Memory Debugging

```bash
# Run with memory checker
valgrind --leak-check=full ./map-server

# Memory profiling
valgrind --tool=massif ./map-server
```

### 2. Performance Profiling

```bash
# CPU profiling
perf record ./map-server
perf report

# Generate flame graph
perf script | stackcollapse-perf.pl | flamegraph.pl > profile.svg
```

### 3. Logging

```cpp
// Logging levels
ShowDebug("Debug message: %s\n", message);
ShowInfo("Info message: %s\n", message);
ShowWarning("Warning message: %s\n", message);
ShowError("Error message: %s\n", message);
ShowFatalError("Fatal error: %s\n", message);
```

## Documentation

### 1. Code Documentation

```cpp
/**
 * @brief Brief description
 * 
 * Detailed description of the function,
 * its behavior, and usage.
 * 
 * @param param1 Description of first parameter
 * @param param2 Description of second parameter
 * @return Description of return value
 * @throws Description of possible exceptions
 */
ReturnType functionName(ParamType1 param1, ParamType2 param2);
```

### 2. Generate Documentation

```bash
# Generate documentation
make docs

# View documentation
xdg-open build/docs/html/index.html
```

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make changes following the style guide
4. Add tests for new functionality
5. Update documentation
6. Submit pull request

### Commit Messages

```
type(scope): Short description

Longer description of changes, rationale, and any breaking changes.

Refs #123
```

Types: feat, fix, docs, style, refactor, test, chore

## Support

- Development channel: #rathena-ai-dev on Discord
- Code review meetings: Every Wednesday 20:00 UTC
- Documentation: https://github.com/yourusername/rathena-AI-world/wiki
- Issue tracker: https://github.com/yourusername/rathena-AI-world/issues

## Appendix

### A. Common Patterns

```cpp
// Singleton pattern (thread-safe)
class Singleton {
public:
    static Singleton& getInstance() {
        static Singleton instance;
        return instance;
    }
    
private:
    Singleton() = default;
    // Delete copy/move
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
};
```

### B. Error Handling

```cpp
// Error handling pattern
class Error : public std::runtime_error {
public:
    explicit Error(const std::string& message) 
        : std::runtime_error(message) {}
};

try {
    // Risky operation
} catch (const Error& e) {
    ShowError("Operation failed: %s\n", e.what());
}
```

### C. Performance Optimization

```cpp
// Performance optimization examples
class Optimized {
private:
    // Preallocate buffers
    std::vector<char> m_buffer;
    
    // Use string views for non-owning strings
    std::string_view m_view;
    
    // Cache frequently accessed data
    std::unordered_map<std::string, AIResponse> m_cache;
};