# Context and LangChain integration build configuration

# Set source files
set(CONTEXT_SOURCES
    ContextManager.cpp
    LangChainMemory.cpp
    LangChainChains.cpp
)

# Set header files
set(CONTEXT_HEADERS
    ContextManager.hpp
    LangChainMemory.hpp
    LangChainChains.hpp
    langchain/base.hpp
    langchain/memory.hpp
    langchain/schema.hpp
)

# Create context library
add_library(ai_context STATIC
    ${CONTEXT_SOURCES}
    ${CONTEXT_HEADERS}
)

# Set include directories
target_include_directories(ai_context
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/src
    PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}
)

# Find required packages
find_package(nlohmann_json REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(CURL REQUIRED)

# Link dependencies
target_link_libraries(ai_context
    PUBLIC
        ai_common
        nlohmann_json::nlohmann_json
    PRIVATE
        OpenSSL::SSL
        OpenSSL::Crypto
        CURL::libcurl
)

# Set compile features
target_compile_features(ai_context PRIVATE cxx_std_17)

# Set compile options
target_compile_options(ai_context
    PRIVATE
        $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Werror>
        $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Werror>
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
)

# Set compile definitions
target_compile_definitions(ai_context
    PRIVATE
        $<$<CONFIG:Debug>:_DEBUG>
        $<$<CONFIG:Release>:NDEBUG>
)

# Install rules
install(TARGETS ai_context
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(FILES ${CONTEXT_HEADERS}
    DESTINATION include/ai/context
)

# Testing configuration
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

# Documentation
if(BUILD_DOCUMENTATION)
    set(DOXYGEN_INPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
    set(DOXYGEN_OUTPUT_DIR ${CMAKE_BINARY_DIR}/docs/ai/context)
    set(DOXYGEN_INDEX_FILE ${DOXYGEN_OUTPUT_DIR}/html/index.html)
    set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
    set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    # Replace variables inside Doxyfile
    configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)

    # Add documentation target
    add_custom_command(OUTPUT ${DOXYGEN_INDEX_FILE}
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}
        MAIN_DEPENDENCY ${DOXYFILE_OUT} ${DOXYFILE_IN}
        COMMENT "Generating context API documentation with Doxygen"
        VERBATIM
    )
    add_custom_target(context_docs DEPENDS ${DOXYGEN_INDEX_FILE})
endif()

# Set properties
set_target_properties(ai_context PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    PUBLIC_HEADER "${CONTEXT_HEADERS}"
)

# Export targets
export(TARGETS ai_context FILE ai_context-config.cmake)

# Summary
message(STATUS "Configured AI Context system with:")
message(STATUS "  - LangChain integration enabled")
message(STATUS "  - Using nlohmann_json ${nlohmann_json_VERSION}")
message(STATUS "  - Using OpenSSL ${OPENSSL_VERSION}")
message(STATUS "  - Using CURL ${CURL_VERSION}")
if(BUILD_TESTING)
    message(STATUS "  - Testing enabled")
endif()
if(BUILD_DOCUMENTATION)
    message(STATUS "  - Documentation enabled")
endif()