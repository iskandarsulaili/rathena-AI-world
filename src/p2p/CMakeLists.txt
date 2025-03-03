# P2P Extension for rAthena
cmake_minimum_required(VERSION 3.12)

# Source files
set(P2P_SOURCES
    common/network.cpp
    common/crypt.cpp
    coordinator/CoordinatorServer.cpp
    coordinator/HostManager.cpp
    coordinator/MapDistributor.cpp
    coordinator/DatabaseProxy.cpp
    map/P2PMapServer.cpp
    map/MapInstance.cpp
)

# Header files
set(P2P_HEADERS
    common/types.hpp
    common/network.hpp
    common/crypt.hpp
    coordinator/CoordinatorServer.hpp
    coordinator/HostManager.hpp
    coordinator/MapDistributor.hpp
    coordinator/DatabaseProxy.hpp
    map/P2PMapServer.hpp
    map/MapInstance.hpp
)

# Create p2p library
add_library(p2p STATIC ${P2P_SOURCES} ${P2P_HEADERS})

# Include directories
target_include_directories(p2p
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/common
        ${CMAKE_CURRENT_SOURCE_DIR}/coordinator
        ${CMAKE_CURRENT_SOURCE_DIR}/map
)

# Link dependencies
target_link_libraries(p2p
    PRIVATE
        pthread
        ssl
        crypto
)

# Compiler flags
target_compile_features(p2p PUBLIC cxx_std_17)
target_compile_options(p2p
    PRIVATE
        -Wall
        -Wextra
        -Werror
        -pedantic
)

# Compile definitions
target_compile_definitions(p2p
    PRIVATE
        P2P_ENABLED
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(p2p
        PRIVATE
            P2P_DEBUG
    )
endif()

# Installation
install(TARGETS p2p
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(FILES ${P2P_HEADERS}
    DESTINATION include/rathena/p2p
)

# Testing
if(BUILD_TESTING)
    # P2P unit tests
    add_executable(p2p_tests
        tests/network_tests.cpp
        tests/map_instance_tests.cpp
        tests/coordinator_tests.cpp
    )
    
    target_link_libraries(p2p_tests
        PRIVATE
            p2p
            gtest
            gtest_main
    )
    
    add_test(NAME p2p_tests COMMAND p2p_tests)
endif()

# Documentation
find_package(Doxygen)
if(DOXYGEN_FOUND)
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

    add_custom_target(p2p_docs
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating P2P documentation with Doxygen"
        VERBATIM
    )
endif()

# Package configuration
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/p2p-config-version.cmake"
    VERSION 1.0.0
    COMPATIBILITY AnyNewerVersion
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/p2p-config-version.cmake"
    DESTINATION lib/cmake/p2p
)

# Export targets
install(EXPORT p2p-targets
    FILE p2p-config.cmake
    NAMESPACE rathena::
    DESTINATION lib/cmake/p2p
)

export(TARGETS p2p FILE p2p-config.cmake)