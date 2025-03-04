# AI System CMake configuration

# Find required packages
find_package(CURL REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(RapidJSON REQUIRED)

# Set source files for AI system
set(AI_SOURCES
    AISystemManager.cpp
    memory/MemoryManager.cpp
    model/AIModelManager.cpp
    model/AIModel.cpp
    model/AzureOpenAIModel.cpp
    model/OpenAIGPT4Model.cpp
    model/DeepSeekModel.cpp
)

# Set header files for AI system
set(AI_HEADERS
    AISystemManager.hpp
    memory/MemoryManager.hpp
    model/AIModelManager.hpp
    model/AIModel.hpp
)

# Create AI system library
add_library(ai_system STATIC ${AI_SOURCES} ${AI_HEADERS})

# Set include directories
target_include_directories(ai_system
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/src
        ${CURL_INCLUDE_DIRS}
        ${OPENSSL_INCLUDE_DIR}
        ${RAPIDJSON_INCLUDE_DIRS}
)

# Set compile flags
target_compile_features(ai_system PUBLIC cxx_std_17)
target_compile_options(ai_system 
    PRIVATE
        $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Werror>
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
)

# Define required preprocessor macros
target_compile_definitions(ai_system
    PRIVATE
        HAVE_AI_SYSTEM
        $<$<CONFIG:Debug>:AI_DEBUG>
)

# Link dependencies
target_link_libraries(ai_system
    PRIVATE
        common
        ${CURL_LIBRARIES}
        ${OPENSSL_LIBRARIES}
)

# Define installation rules
install(TARGETS ai_system
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(FILES ${AI_HEADERS}
    DESTINATION include/ai
)

# Add subdirectories for agent implementations
add_subdirectory(balance)
add_subdirectory(reward)
add_subdirectory(ranking)
add_subdirectory(engagement)
add_subdirectory(economy)
add_subdirectory(social)
add_subdirectory(coach)
add_subdirectory(emotional)
add_subdirectory(monster)
add_subdirectory(quest)

# Testing configuration
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

# Documentation
if(BUILD_DOCUMENTATION)
    set(AI_DOC_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/../../doc/ai_system.md
        ${CMAKE_CURRENT_SOURCE_DIR}/../../doc/ai_agents.md
        ${CMAKE_CURRENT_SOURCE_DIR}/../../doc/ai_integration.md
    )
    
    add_custom_target(ai_docs
        SOURCES ${AI_DOC_FILES}
    )
endif()

# Dependency checks and configuration
include(CheckAISystemDependencies)

# Configure AI system
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/config/ai_config.hpp.in
    ${CMAKE_CURRENT_BINARY_DIR}/config/ai_config.hpp
)

# Add configuration header to includes
target_include_directories(ai_system
    PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}/config
)

# Create helper script for API key configuration
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/scripts/configure_ai_keys.sh.in
    ${CMAKE_CURRENT_BINARY_DIR}/scripts/configure_ai_keys.sh
    @ONLY
)

# Make script executable
if(UNIX)
    execute_process(
        COMMAND chmod +x ${CMAKE_CURRENT_BINARY_DIR}/scripts/configure_ai_keys.sh
    )
endif()

# Output configuration summary
message(STATUS "AI System Configuration:")
message(STATUS "  Enabled Agents:")
message(STATUS "    - Adaptive Balance AI: ${ENABLE_BALANCE_AI}")
message(STATUS "    - Dynamic Reward AI: ${ENABLE_REWARD_AI}")
message(STATUS "    - Competitive Ranking AI: ${ENABLE_RANKING_AI}")
message(STATUS "    - Player Engagement AI: ${ENABLE_ENGAGEMENT_AI}")
message(STATUS "    - Adaptive Economy AI: ${ENABLE_ECONOMY_AI}")
message(STATUS "    - Smart Party & Guild AI: ${ENABLE_SOCIAL_AI}")
message(STATUS "    - Personalized AI Coach: ${ENABLE_COACH_AI}")
message(STATUS "    - Emotional Investment AI: ${ENABLE_EMOTIONAL_AI}")
message(STATUS "    - Dynamic AI Monster: ${ENABLE_MONSTER_AI}")
message(STATUS "    - Adaptive Quest AI: ${ENABLE_QUEST_AI}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ Standard: C++17")
message(STATUS "  Using OpenSSL: ${OPENSSL_VERSION}")
message(STATUS "  Using CURL: ${CURL_VERSION_STRING}")
message(STATUS "  Using RapidJSON: ${RAPIDJSON_VERSION}")