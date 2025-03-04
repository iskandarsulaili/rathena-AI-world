# AI System CMake configuration

# Add subdirectories
add_subdirectory(context)
add_subdirectory(model)
add_subdirectory(memory)

# Set source files
set(AI_SOURCES
    AISystemManager.cpp
)

# Set header files
set(AI_HEADERS
    AISystemManager.hpp
)

# Create AI system library
add_library(ai_system STATIC
    ${AI_SOURCES}
    ${AI_HEADERS}
)

# Set include directories
target_include_directories(ai_system
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

# Set up langchain include directory
set(LANGCHAIN_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/context/langchain)
file(MAKE_DIRECTORY ${LANGCHAIN_INCLUDE_DIR})

# Link dependencies
target_link_libraries(ai_system
    PUBLIC
        ai_context
        ai_model
        ai_memory
        nlohmann_json::nlohmann_json
    PRIVATE
        OpenSSL::SSL
        OpenSSL::Crypto
        CURL::libcurl
)

# Add compile definitions
target_compile_definitions(ai_system
    PRIVATE
        $<$<CONFIG:Debug>:_DEBUG>
        $<$<CONFIG:Release>:NDEBUG>
        LANGCHAIN_INCLUDE_DIR="${LANGCHAIN_INCLUDE_DIR}"
)

# Set compile options
target_compile_options(ai_system
    PRIVATE
        $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Werror>
        $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Werror>
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
)

# Set properties
set_target_properties(ai_system PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    PUBLIC_HEADER "${AI_HEADERS}"
)

# Install rules
install(TARGETS ai_system
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    PUBLIC_HEADER DESTINATION include/ai
)

# Create and install pkg-config file
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/ai_system.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/ai_system.pc
    @ONLY
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ai_system.pc
    DESTINATION lib/pkgconfig
)

# Testing configuration
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

# Documentation
if(BUILD_DOCUMENTATION)
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        set(DOXYGEN_INPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
        set(DOXYGEN_OUTPUT_DIR ${CMAKE_BINARY_DIR}/docs/ai)
        set(DOXYGEN_INDEX_FILE ${DOXYGEN_OUTPUT_DIR}/html/index.html)
        set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
        set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

        configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)

        add_custom_command(OUTPUT ${DOXYGEN_INDEX_FILE}
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}
            MAIN_DEPENDENCY ${DOXYFILE_OUT} ${DOXYFILE_IN}
            COMMENT "Generating AI system documentation with Doxygen"
            VERBATIM
        )
        add_custom_target(ai_docs DEPENDS ${DOXYGEN_INDEX_FILE})
    endif()
endif()

# Summary
message(STATUS "Configured AI system with:")
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