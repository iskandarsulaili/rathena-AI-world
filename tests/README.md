# rAthena AI System Tests

This directory contains the test suite for the rAthena AI System. The tests are organized by component and use Catch2 as the testing framework.

## Directory Structure

```
tests/
├── ai/                     # AI system core tests
│   ├── agents/            # Individual agent tests
│   ├── memory/            # Memory system tests
│   └── model/             # AI model tests
├── data/                  # Test data files
│   ├── prompts.json      # Test prompts
│   ├── responses.json    # Expected responses
│   └── memories.json     # Memory test data
├── CMakeLists.txt        # Test build configuration
├── test_config.hpp.in    # Test configuration template
└── README.md            # This file
```

## Quick Start

### 1. Build Tests
```bash
# From project root
mkdir build && cd build
cmake .. -DBUILD_TESTING=ON
make -j$(nproc)
```

### 2. Run Tests
```bash
# Run all tests
ctest --output-on-failure

# Run specific test categories
./ai_system_tests "[ai]"        # AI system tests
./ai_system_tests "[memory]"    # Memory system tests
./ai_system_tests "[agents]"    # Agent tests
./ai_system_tests "[model]"     # AI model tests
```

### 3. Run with Coverage
```bash
# Build with coverage
cmake .. -DBUILD_TESTING=ON -DENABLE_COVERAGE=ON
make coverage

# View coverage report
xdg-open coverage/index.html
```

## Writing Tests

### 1. Test File Organization
- Put tests in appropriate subdirectories
- Use meaningful file names (e.g., `AdaptiveBalanceAgentTests.cpp`)
- Include relevant test fixtures and utilities

### 2. Test Case Structure
```cpp
TEST_CASE("Component description", "[category]") {
    // Setup
    AISystemTest test;
    test.SetUp();
    
    SECTION("Specific functionality") {
        // Test implementation
        REQUIRE(condition);
    }
    
    test.TearDown();
}
```

### 3. Using Test Data
```cpp
// Load test data
auto data = loadTestData("prompts.json");

// Use in tests
SECTION("Test with data") {
    auto result = processTestCase(data["test_case"]);
    REQUIRE(result.success);
}
```

### 4. Test Categories
Use appropriate tags for test organization:
- `[core]` - Core system functionality
- `[agents]` - AI agent tests
- `[memory]` - Memory system tests
- `[model]` - AI model tests
- `[performance]` - Performance benchmarks
- `[integration]` - Integration tests

## Best Practices

1. **Test Independence**
   - Each test should be independent
   - Clean up all resources in TearDown
   - Don't rely on test execution order

2. **Test Data**
   - Use external test data files
   - Keep test data versioned
   - Document data format changes

3. **Mock Objects**
   - Use provided mock classes
   - Document mock behavior
   - Keep mocks simple

4. **Performance Tests**
   ```cpp
   BENCHMARK("Operation name") {
       return performOperation();
   }
   ```

5. **Error Cases**
   - Test both success and failure paths
   - Verify error messages and codes
   - Test edge cases

## Adding New Tests

1. Create test file in appropriate directory
2. Add file to `CMakeLists.txt`
3. Implement tests using Catch2 framework
4. Add necessary test data
5. Document special requirements

## Test Configuration

### Environment Variables
- `TEST_AI_API_KEY` - API key for testing
- `TEST_AI_API_ENDPOINT` - API endpoint
- `TEST_DATABASE_URL` - Test database URL
- `TEST_LOG_LEVEL` - Logging level for tests

### Configuration File
Edit `test_config.hpp.in` for test-specific settings:
```cpp
#define TEST_TIMEOUT_SHORT 1000
#define TEST_TIMEOUT_MEDIUM 5000
#define TEST_TIMEOUT_LONG 30000
```

## Memory Testing

Use provided tools for memory testing:
```bash
# Run with Valgrind
make memcheck

# Run with Address Sanitizer
cmake .. -DUSE_SANITIZER=Address
make
ctest
```

## Performance Testing

1. Use benchmarking features:
```cpp
BENCHMARK("Description") {
    return operation();
}
```

2. Run performance tests:
```bash
./ai_system_tests [.][benchmark]
```

3. Analyze results:
```bash
# Generate performance report
make benchmark-report
```

## Troubleshooting

### Common Issues

1. **Tests Failing to Build**
   - Check CMake configuration
   - Verify dependencies
   - Check include paths

2. **Tests Timing Out**
   - Adjust timeouts in configuration
   - Check resource usage
   - Verify test assumptions

3. **Memory Leaks**
   - Use Valgrind
   - Check cleanup in TearDown
   - Verify resource management

### Getting Help

1. Check test logs in `build/Testing/Temporary/`
2. Review test documentation
3. Contact development team

## Contributing

1. Follow test style guide
2. Add documentation
3. Include test data
4. Verify coverage
5. Run full test suite

## Resources

- [Catch2 Documentation](https://github.com/catchorg/Catch2/tree/devel/docs)
- [Testing Guide](../doc/testing.md)
- [Code Coverage Guide](../doc/coverage.md)