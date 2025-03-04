#!/bin/bash

# Build script for rAthena AI System
# This script handles the compilation and installation of the AI system

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Default settings
BUILD_TYPE="Release"
INSTALL_PREFIX="/usr/local"
BUILD_DIR="build"
AI_MODEL="azure_openai"
PARALLEL_JOBS=$(nproc)

# Function to print usage
print_usage() {
    echo "Usage: $0 [options]"
    echo "Options:"
    echo "  --debug               Build in debug mode"
    echo "  --prefix=<path>      Installation prefix (default: /usr/local)"
    echo "  --build-dir=<path>   Build directory (default: build)"
    echo "  --ai-model=<model>   Primary AI model (azure_openai|openai_gpt4|deepseek_v3)"
    echo "  --jobs=<n>           Number of parallel jobs (default: number of CPU cores)"
    echo "  --help               Print this help message"
}

# Parse command line arguments
while [ "$1" != "" ]; do
    case $1 in
        --debug )        BUILD_TYPE="Debug"
                        ;;
        --prefix=* )     INSTALL_PREFIX="${1#*=}"
                        ;;
        --build-dir=* )  BUILD_DIR="${1#*=}"
                        ;;
        --ai-model=* )   AI_MODEL="${1#*=}"
                        ;;
        --jobs=* )       PARALLEL_JOBS="${1#*=}"
                        ;;
        --help )        print_usage
                        exit
                        ;;
        * )             print_usage
                        exit 1
    esac
    shift
done

# Function to check dependencies
check_dependencies() {
    echo -e "${YELLOW}Checking dependencies...${NC}"
    
    # Check for required programs
    REQUIRED_PROGRAMS="cmake make g++ git curl pkg-config"
    MISSING_PROGRAMS=""
    
    for prog in $REQUIRED_PROGRAMS; do
        if ! command -v $prog >/dev/null 2>&1; then
            MISSING_PROGRAMS="$MISSING_PROGRAMS $prog"
        fi
    done
    
    if [ ! -z "$MISSING_PROGRAMS" ]; then
        echo -e "${RED}Error: Required programs not found:${NC}$MISSING_PROGRAMS"
        echo "Please install these programs and try again."
        exit 1
    fi
    
    # Check for required libraries
    REQUIRED_PACKAGES="libcurl4-openssl-dev libssl-dev rapidjson-dev"
    MISSING_PACKAGES=""
    
    for pkg in $REQUIRED_PACKAGES; do
        if ! pkg-config --exists $pkg 2>/dev/null; then
            MISSING_PACKAGES="$MISSING_PACKAGES $pkg"
        fi
    done
    
    if [ ! -z "$MISSING_PACKAGES" ]; then
        echo -e "${RED}Error: Required packages not found:${NC}$MISSING_PACKAGES"
        echo "Please install these packages and try again."
        echo "On Ubuntu/Debian: sudo apt install$MISSING_PACKAGES"
        exit 1
    fi
    
    echo -e "${GREEN}All dependencies satisfied.${NC}"
}

# Function to create and configure build directory
configure_build() {
    echo -e "${YELLOW}Configuring build...${NC}"
    
    # Create build directory
    mkdir -p $BUILD_DIR
    cd $BUILD_DIR
    
    # Configure CMake
    cmake .. \
        -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
        -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX \
        -DWITH_AI_SYSTEM=ON \
        -DWITH_AI_AZURE=$([ "$AI_MODEL" = "azure_openai" ] && echo "ON" || echo "OFF") \
        -DWITH_AI_OPENAI=$([ "$AI_MODEL" = "openai_gpt4" ] && echo "ON" || echo "OFF") \
        -DWITH_AI_DEEPSEEK=$([ "$AI_MODEL" = "deepseek_v3" ] && echo "ON" || echo "OFF") \
        -DBUILD_TESTING=ON
    
    if [ $? -ne 0 ]; then
        echo -e "${RED}Error: CMake configuration failed${NC}"
        exit 1
    fi
    
    cd ..
    echo -e "${GREEN}Build configured successfully.${NC}"
}

# Function to build the project
build_project() {
    echo -e "${YELLOW}Building project...${NC}"
    
    cd $BUILD_DIR
    make -j$PARALLEL_JOBS
    
    if [ $? -ne 0 ]; then
        echo -e "${RED}Error: Build failed${NC}"
        exit 1
    fi
    
    cd ..
    echo -e "${GREEN}Build completed successfully.${NC}"
}

# Function to run tests
run_tests() {
    echo -e "${YELLOW}Running tests...${NC}"
    
    cd $BUILD_DIR
    ctest --output-on-failure
    
    if [ $? -ne 0 ]; then
        echo -e "${RED}Error: Tests failed${NC}"
        exit 1
    fi
    
    cd ..
    echo -e "${GREEN}All tests passed.${NC}"
}

# Function to install the project
install_project() {
    echo -e "${YELLOW}Installing...${NC}"
    
    cd $BUILD_DIR
    make install
    
    if [ $? -ne 0 ]; then
        echo -e "${RED}Error: Installation failed${NC}"
        exit 1
    fi
    
    cd ..
    echo -e "${GREEN}Installation completed successfully.${NC}"
}

# Main execution
echo "rAthena AI System Build Script"
echo "============================="
echo "Build type: $BUILD_TYPE"
echo "Install prefix: $INSTALL_PREFIX"
echo "Build directory: $BUILD_DIR"
echo "Primary AI model: $AI_MODEL"
echo "Parallel jobs: $PARALLEL_JOBS"
echo "============================="

# Execute build steps
check_dependencies
configure_build
build_project
run_tests
install_project

echo -e "${GREEN}Build and installation completed successfully!${NC}"
echo "Please configure your AI model API keys in conf/ai_agents.conf"
echo "For more information, see the documentation in doc/ai_system.md"

exit 0