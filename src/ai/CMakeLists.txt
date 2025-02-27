# AI Module CMake configuration

set(AI_HEADERS
    common/ai_agent_base.hpp
    common/ai_config.hpp
    common/ai_request.hpp
    common/ai_response.hpp
    common/ai_types.hpp
    common/ai_provider.hpp
    agents/ai_beggar_agent.hpp
    agents/ai_legends_agent.hpp
)

set(AI_SOURCES
    common/ai_agent_base.cpp
    common/ai_config.cpp
    agents/ai_beggar_agent.cpp
    ai_module.cpp
)

# Include directories
include_directories(
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/3rdparty
    ${PROJECT_SOURCE_DIR}/src/map
    ${PROJECT_SOURCE_DIR}/src/common
)

# Create AI module library
add_library(ai_module STATIC
    ${AI_HEADERS}
    ${AI_SOURCES}
)

# Link against rAthena libraries
target_link_libraries(ai_module
    common
    map
)

# Add compiler definitions
target_compile_definitions(ai_module
    PRIVATE
    RATHENA_BUILD
    AI_MODULE_INTERNAL
)

# Set include directories for clients of this library
target_include_directories(ai_module
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/common
    ${CMAKE_CURRENT_SOURCE_DIR}/agents
)

# Set C++ standard
set_target_properties(ai_module PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# Installation rules
install(TARGETS ai_module
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(FILES ${AI_HEADERS}
    DESTINATION include/ai
)

# Add subdirectories for tests if enabled
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()