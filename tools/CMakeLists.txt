# Network Configuration Validator
add_executable(validate_network_config
    validate_network_config.cpp
)

target_link_libraries(validate_network_config
    PRIVATE
    p2p_common
)

target_include_directories(validate_network_config
    PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

# Install validation tool
install(TARGETS validate_network_config
    RUNTIME DESTINATION bin/tools
)

# Add test that runs the validator on the default config
add_test(NAME network_config_validation
    COMMAND validate_network_config ${CMAKE_SOURCE_DIR}/conf/p2p/network_quality.conf
)

# Create a symbolic link in the tools directory
add_custom_command(TARGET validate_network_config POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E create_symlink
        $<TARGET_FILE:validate_network_config>
        ${CMAKE_SOURCE_DIR}/tools/validate_network_config
    COMMENT "Creating symbolic link for network configuration validator"
)