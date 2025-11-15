# CMakeLists.txt for rAthena AIWorld Plugin/Module and Standalone Server (ZeroMQ IPC + PostgreSQL + Tests)

cmake_minimum_required(VERSION 3.20)
project(aiworld_plugin LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find ZeroMQ
find_package(PkgConfig REQUIRED)
pkg_check_modules(ZeroMQ REQUIRED libzmq)

# Find libpqxx (PostgreSQL C++ client)
pkg_check_modules(PQXX REQUIRED libpqxx)

# Enable testing and coverage
enable_testing()
option(CODE_COVERAGE "Enable coverage reporting" ON)
if(CODE_COVERAGE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -O0")
endif()

# Plugin sources
set(AIWORLD_SRC
    aiworld_plugin.cpp
    aiworld_ipc.cpp
    aiworld_ipc.hpp
    aiworld_messages.hpp
    aiworld_plugin.hpp
    aiworld_utils.cpp
    aiworld_utils.hpp
    aiworld_server.cpp
)

# Standalone server main
set(AIWORLD_SERVER_SRC
    aiworld_server.cpp
    aiworld_utils.cpp
    aiworld_ipc.cpp
    aiworld_messages.hpp
    aiworld_utils.hpp
    aiworld_ipc.hpp
    aiworld_types.hpp
)

# Main server binary
add_executable(aiworld_server ${AIWORLD_SERVER_SRC})
target_include_directories(aiworld_server
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${ZeroMQ_INCLUDE_DIRS}
        ${PQXX_INCLUDE_DIRS}
)
target_link_libraries(aiworld_server
    ${ZeroMQ_LIBRARIES}
    ${PQXX_LIBRARIES}
)
target_compile_definitions(aiworld_server PRIVATE AIWORLD_SERVER_BUILD)

# Plugin shared library
add_library(aiworld_plugin SHARED ${AIWORLD_SRC})
target_include_directories(aiworld_plugin
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${ZeroMQ_INCLUDE_DIRS}
        ${PQXX_INCLUDE_DIRS}
        ../common
        ../map
        ../char
        ../config
)
target_link_libraries(aiworld_plugin
    ${ZeroMQ_LIBRARIES}
    ${PQXX_LIBRARIES}
)
target_compile_definitions(aiworld_plugin PRIVATE AIWORLD_PLUGIN_BUILD)

# Tests
add_subdirectory(tests)
