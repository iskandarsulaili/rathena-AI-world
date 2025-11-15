# CMakeLists.txt for AIWorld server tests (GoogleTest + coverage)

enable_testing()

# Find GoogleTest
find_package(GTest REQUIRED)
include(GoogleTest)

set(TEST_SRC
    test_postgres_manager.cpp
    test_config_loader.cpp
    test_admin_handler.cpp
    test_mission_logic.cpp
    test_event_logic.cpp
    test_entity_logic.cpp
)

add_executable(aiworld_tests ${TEST_SRC})
target_include_directories(aiworld_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        ${GTEST_INCLUDE_DIRS}
)
target_link_libraries(aiworld_tests
    GTest::gtest
    GTest::gtest_main
    pthread
    pqxx
    zmq
)

gtest_discover_tests(aiworld_tests)

# Coverage target
if(CODE_COVERAGE)
    add_custom_target(coverage
        COMMAND lcov --capture --directory . --output-file coverage.info
        COMMAND lcov --remove coverage.info '/usr/*' --output-file coverage.info
        COMMAND genhtml coverage.info --output-directory out
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Generating code coverage report with lcov"
    )
endif()