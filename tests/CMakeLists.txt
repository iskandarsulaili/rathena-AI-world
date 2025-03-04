# Test configuration for rAthena AI System

# Enable testing
enable_testing()

# Find required packages
find_package(Threads REQUIRED)

# Fetch Catch2
include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v2.13.9  # Latest stable version
)
FetchContent_MakeAvailable(Catch2)

# Set test source files
set(TEST_SOURCES
    ai/AISystemTests.cpp
    ai/agents/AdaptiveBalanceAgentTests.cpp
    ai/agents/DynamicRewardAgentTests.cpp
    ai/agents/CompetitiveRankingAgentTests.cpp
    ai/memory/MemoryManagerTests.cpp
    ai/model/AIModelTests.cpp
)

# Create test executable
add_executable(ai_system_tests ${TEST_SOURCES})

# Set include directories
target_include_directories(ai_system_tests
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link dependencies
target_link_libraries(ai_system_tests
    PRIVATE
        ai_system
        Catch2::Catch2
        ${CMAKE_THREAD_LIBS_INIT}
)

# Set compile options
target_compile_features(ai_system_tests PRIVATE cxx_std_17)
target_compile_options(ai_system_tests 
    PRIVATE
        $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Werror>
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
)

# Define test macros
target_compile_definitions(ai_system_tests
    PRIVATE
        CATCH_CONFIG_ENABLE_BENCHMARKING
        TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data"
)

# Create test data directory
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/data)

# Copy test data files
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/data/
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/data)

# Register tests
include(${Catch2_SOURCE_DIR}/contrib/Catch.cmake)
catch_discover_tests(ai_system_tests
    PROPERTIES
        TIMEOUT 30  # Timeout in seconds
        LABELS "ai"
)

# Custom test targets
add_custom_target(test_ai
    COMMAND ai_system_tests "[ai]"
    DEPENDS ai_system_tests
)

add_custom_target(test_memory
    COMMAND ai_system_tests "[memory]"
    DEPENDS ai_system_tests
)

add_custom_target(test_agents
    COMMAND ai_system_tests "[agents]"
    DEPENDS ai_system_tests
)

add_custom_target(test_performance
    COMMAND ai_system_tests "[performance]"
    DEPENDS ai_system_tests
)

# Test configuration file
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/test_config.hpp.in
    ${CMAKE_CURRENT_BINARY_DIR}/test_config.hpp
)

# Additional include directory for generated files
target_include_directories(ai_system_tests
    PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}
)

# Generate test documentation if enabled
if(BUILD_DOCUMENTATION)
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        set(DOXYGEN_TEST_OUTPUT_DIR ${CMAKE_BINARY_DIR}/docs/tests)
        configure_file(
            ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
            ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
            @ONLY
        )
        add_custom_target(test_docs
            ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating test documentation"
        )
    endif()
endif()

# Create test coverage target if enabled
if(ENABLE_COVERAGE)
    find_program(GCOVR_PATH gcovr REQUIRED)
    add_custom_target(coverage
        COMMAND ${GCOVR_PATH}
            --root ${CMAKE_SOURCE_DIR}
            --exclude ".*tests.*"
            --exclude ".*third_party.*"
            --html --html-details
            -o ${CMAKE_BINARY_DIR}/coverage/index.html
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        DEPENDS ai_system_tests
        COMMENT "Generating code coverage report"
    )
endif()

# Setup test environment
set_tests_properties(ai_system_tests PROPERTIES
    ENVIRONMENT "AI_TEST_DATA_DIR=${CMAKE_CURRENT_SOURCE_DIR}/data"
)

# Create directory for test logs
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/logs/tests)

# Setup memory checker if available
find_program(VALGRIND_PATH valgrind)
if(VALGRIND_PATH)
    add_custom_target(memcheck
        COMMAND ${VALGRIND_PATH}
            --leak-check=full
            --show-leak-kinds=all
            --track-origins=yes
            --error-exitcode=1
            $<TARGET_FILE:ai_system_tests>
        DEPENDS ai_system_tests
        COMMENT "Running memory checks"
    )
endif()

# Create test results directory
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/test_results)

# Add benchmarking target
add_custom_target(benchmark
    COMMAND ai_system_tests "[benchmark]"
    DEPENDS ai_system_tests
)

message(STATUS "Test configuration completed")
message(STATUS "Available test targets:")
message(STATUS "  - test_ai: Run all AI system tests")
message(STATUS "  - test_memory: Run memory system tests")
message(STATUS "  - test_agents: Run AI agent tests")
message(STATUS "  - test_performance: Run performance tests")
if(DOXYGEN_FOUND)
    message(STATUS "  - test_docs: Generate test documentation")
endif()
if(ENABLE_COVERAGE)
    message(STATUS "  - coverage: Generate code coverage report")
endif()
if(VALGRIND_PATH)
    message(STATUS "  - memcheck: Run memory checks")
endif()
message(STATUS "  - benchmark: Run performance benchmarks")