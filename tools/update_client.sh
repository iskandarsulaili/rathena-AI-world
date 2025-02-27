#!/bin/bash

# ANSI color codes
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored messages
print_message() {
    color=$1
    message=$2
    printf "${color}%s${NC}\n" "$message"
}

# Function to check if a command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Check Python installation
print_message "$BLUE" "Checking Python installation..."
if ! command_exists python3; then
    print_message "$RED" "Error: Python 3 is not installed"
    print_message "$RED" "Please install Python 3.7 or later from your package manager or https://www.python.org/"
    exit 1
fi

print_message "$GREEN" "Python found"

# Check required Python packages
print_message "$BLUE" "Checking required packages..."
if ! python3 -c "import json" 2>/dev/null; then
    print_message "$YELLOW" "Installing required packages..."
    pip3 install json
fi

# Check if script is run from tools directory
if [ ! -f "client_update_config.json" ]; then
    print_message "$RED" "Error: client_update_config.json not found"
    print_message "$RED" "Please ensure you are running this script from the tools directory"
    exit 1
fi

# Create backup directory
if [ ! -d "backup" ]; then
    print_message "$BLUE" "Creating backup directory..."
    mkdir -p backup
fi

# Check for required files
print_message "$BLUE" "Checking required files..."
MISSING=0
for file in ../Client/data.grf ../Client/rdata.grf ../Client/CDClient.dll; do
    if [ ! -f "$file" ]; then
        print_message "$RED" "Missing required file: $file"
        MISSING=$((MISSING + 1))
    fi
done

if [ $MISSING -gt 0 ]; then
    print_message "$RED" "Error: Required files are missing. Please check the Client directory"
    exit 1
fi

# Run the update script
print_message "$BLUE" "Starting client update process..."
python3 update_client.py client_update_config.json
if [ $? -ne 0 ]; then
    print_message "$RED" "Error: Update process failed"
    print_message "$RED" "Please check the logs for more information"
    exit 1
fi

print_message "$GREEN" "Update completed successfully!"
echo
print_message "$BLUE" "Reminder: Please verify the game client works correctly after update"
print_message "$BLUE" "If you experience any issues, you can find backups in the backup directory"

# Make script executable
chmod +x update_client.py

exit 0