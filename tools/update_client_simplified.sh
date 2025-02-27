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

print_message "$BLUE" "Simplified AI Client Updater"
print_message "$BLUE" "=============================="
echo

# Check Python installation
print_message "$BLUE" "Checking Python installation..."
if ! command_exists python3; then
    print_message "$RED" "Error: Python 3 is not installed"
    print_message "$RED" "Please install Python 3.7 or later from your package manager or https://www.python.org/"
    exit 1
fi

print_message "$GREEN" "Python found"

# Check if required directories exist
if [ ! -d "dlls" ]; then
    print_message "$RED" "Error: dlls directory not found"
    print_message "$RED" "Please ensure the dlls directory exists and contains the required DLL files"
    exit 1
fi

if [ ! -d "configs" ]; then
    print_message "$RED" "Error: configs directory not found"
    print_message "$RED" "Please ensure the configs directory exists and contains the required configuration files"
    exit 1
fi

# Check for required DLL files
MISSING=0
for file in dlls/ai_client.dll dlls/ai_network.dll dlls/ai_resource.dll; do
    if [ ! -f "$file" ]; then
        print_message "$RED" "Missing required file: $file"
        MISSING=$((MISSING + 1))
    fi
done

# Check for required config files
for file in configs/ai_client.ini configs/ai_mapping.ini; do
    if [ ! -f "$file" ]; then
        print_message "$RED" "Missing required file: $file"
        MISSING=$((MISSING + 1))
    fi
done

if [ $MISSING -gt 0 ]; then
    print_message "$RED" "Error: Required files are missing. Please check the dlls and configs directories"
    exit 1
fi

# Ask for client directory
echo
print_message "$BLUE" "Please enter the path to your Ragnarok Online client directory:"
print_message "$BLUE" "Example: /home/user/Games/RagnarokOnline/Client"
echo
read -p "Client directory: " CLIENT_DIR

# Validate client directory
if [ ! -d "$CLIENT_DIR" ]; then
    print_message "$RED" "Error: The specified directory does not exist"
    exit 1
fi

if [ ! -f "$CLIENT_DIR/data.grf" ]; then
    print_message "$RED" "Error: This does not appear to be a valid Ragnarok Online client directory"
    print_message "$RED" "(data.grf not found)"
    exit 1
fi

# Run the update script
echo
print_message "$BLUE" "Starting client update process..."
python3 update_client_simplified.py "$CLIENT_DIR"
if [ $? -ne 0 ]; then
    print_message "$RED" "Error: Update process failed"
    print_message "$RED" "Please check the logs for more information"
    exit 1
fi

echo
print_message "$GREEN" "Update completed successfully!"
echo
print_message "$BLUE" "You can now launch the game to use the AI features."
print_message "$BLUE" "If you experience any issues, you can find backups in the backup directory"

# Make script executable
chmod +x update_client_simplified.py

exit 0