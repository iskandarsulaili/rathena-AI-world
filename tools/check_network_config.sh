#!/bin/bash

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
DEFAULT_CONFIG="${SCRIPT_DIR}/../conf/p2p/network_quality.conf"
VALIDATOR="${SCRIPT_DIR}/validate_network_config"

function print_usage() {
    echo "Usage: $0 [OPTIONS] [config_file]"
    echo
    echo "Validates P2P network configuration settings"
    echo
    echo "Options:"
    echo "  -h, --help     Show this help message"
    echo "  -v, --verbose  Show detailed validation results"
    echo "  -f, --fix      Try to fix common configuration issues"
    echo
    echo "Examples:"
    echo "  $0                      # Check default config"
    echo "  $0 -v my_config.conf    # Validate specific config with details"
    echo "  $0 -f                   # Auto-fix issues in default config"
}

function validate_config() {
    local config_file=$1
    local verbose=$2
    local fix=$3

    echo -e "${YELLOW}Validating network configuration: ${config_file}${NC}"
    echo

    if [ ! -f "$config_file" ]; then
        echo -e "${RED}Error: Configuration file not found: ${config_file}${NC}"
        exit 1
    }

    if [ "$verbose" = true ]; then
        $VALIDATOR "$config_file" -v
    else
        $VALIDATOR "$config_file"
    fi

    local exit_code=$?

    if [ $exit_code -eq 0 ]; then
        echo -e "\n${GREEN}✔ Configuration validation passed${NC}"
        return 0
    else
        echo -e "\n${RED}✘ Configuration validation failed${NC}"
        
        if [ "$fix" = true ]; then
            echo
            echo -e "${YELLOW}Attempting to fix configuration issues...${NC}"
            attempt_fix "$config_file"
        else
            echo
            echo "Run with -f option to attempt automatic fixes"
        fi
        
        return 1
    fi
}

function attempt_fix() {
    local config_file=$1
    local backup_file="${config_file}.bak"

    # Create backup
    cp "$config_file" "$backup_file"
    echo "Created backup: $backup_file"

    # Common fixes
    echo "Applying common fixes..."
    
    # Fix scoring weights to sum to 1.0
    fix_scoring_weights "$config_file"
    
    # Ensure minimum thresholds
    fix_thresholds "$config_file"
    
    # Validate again
    echo
    echo -e "${YELLOW}Validating fixed configuration...${NC}"
    if $VALIDATOR "$config_file" >/dev/null 2>&1; then
        echo -e "${GREEN}✔ Configuration fixed successfully${NC}"
        return 0
    else
        echo -e "${RED}✘ Automatic fix unsuccessful${NC}"
        echo "Restoring backup..."
        mv "$backup_file" "$config_file"
        return 1
    fi
}

function fix_scoring_weights() {
    local config_file=$1
    
    # Extract current weights
    local latency_weight=$(grep "latency_weight" "$config_file" | awk -F'=' '{print $2}' | tr -d ' ')
    local bandwidth_weight=$(grep "bandwidth_weight" "$config_file" | awk -F'=' '{print $2}' | tr -d ' ')
    local stability_weight=$(grep "stability_weight" "$config_file" | awk -F'=' '{print $2}' | tr -d ' ')
    
    # Calculate sum and adjust if needed
    local sum=$(echo "$latency_weight + $bandwidth_weight + $stability_weight" | bc)
    if [ $(echo "$sum != 1.0" | bc) -eq 1 ]; then
        echo "Adjusting scoring weights to sum to 1.0..."
        sed -i "s/latency_weight = .*/latency_weight = 0.4/" "$config_file"
        sed -i "s/bandwidth_weight = .*/bandwidth_weight = 0.3/" "$config_file"
        sed -i "s/stability_weight = .*/stability_weight = 0.3/" "$config_file"
    fi
}

function fix_thresholds() {
    local config_file=$1
    
    # Ensure minimum bandwidth requirements
    sed -i 's/bandwidth_min = \([0-9]*\)/bandwidth_min = 5/' "$config_file"
    
    # Ensure reasonable latency thresholds
    sed -i 's/latency_warning = \([0-9]*\)/latency_warning = 100/' "$config_file"
    sed -i 's/latency_critical = \([0-9]*\)/latency_critical = 200/' "$config_file"
}

# Parse command line arguments
VERBOSE=false
FIX=false
CONFIG_FILE="$DEFAULT_CONFIG"

while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            print_usage
            exit 0
            ;;
        -v|--verbose)
            VERBOSE=true
            shift
            ;;
        -f|--fix)
            FIX=true
            shift
            ;;
        *)
            CONFIG_FILE="$1"
            shift
            ;;
    esac
done

validate_config "$CONFIG_FILE" "$VERBOSE" "$FIX"