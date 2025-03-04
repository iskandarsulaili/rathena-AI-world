#!/bin/bash

# AI System Monitor Tool
# Provides real-time monitoring of AI system performance and resource usage

# Color codes
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

# Configuration
REFRESH_RATE=5
LOG_PATH="../log/map_ai.log"
CONF_PATH="../conf/ai_agents.conf"
MAP_SERVER_PID=""
MONITOR_ACTIVE=true

# Function to handle script termination
cleanup() {
    MONITOR_ACTIVE=false
    echo -e "\n${BLUE}Monitoring stopped.${NC}"
    tput cnorm # Restore cursor
    exit 0
}

trap cleanup SIGINT SIGTERM

# Function to get memory usage
get_memory_usage() {
    if [ ! -z "$MAP_SERVER_PID" ]; then
        local mem=$(ps -o rss= -p "$MAP_SERVER_PID" | awk '{print $1/1024}')
        printf "%.2f MB" $mem
    else
        echo "N/A"
    fi
}

# Function to get CPU usage
get_cpu_usage() {
    if [ ! -z "$MAP_SERVER_PID" ]; then
        local cpu=$(ps -p "$MAP_SERVER_PID" -o %cpu= | awk '{print $1}')
        printf "%.1f%%" $cpu
    else
        echo "N/A"
    fi
}

# Function to get API request metrics
get_api_metrics() {
    local period=$1 # Time period in seconds
    local current_time=$(date +%s)
    local start_time=$((current_time - period))
    
    # Count API requests in the time period
    local requests=$(grep -c "API request" "$LOG_PATH")
    local errors=$(grep -c "API error" "$LOG_PATH")
    local latency=$(grep "API response time:" "$LOG_PATH" | tail -n 10 | awk '{sum += $4} END {print sum/NR}')
    
    echo "$requests|$errors|$latency"
}

# Function to get agent activity
get_agent_activity() {
    local agent=$1
    local last_active=$(grep "$agent" "$LOG_PATH" | tail -n 1 | cut -d' ' -f1,2)
    local status=$(grep -q "^[[:space:]]*${agent}.*enabled:[[:space:]]*true" "$CONF_PATH" && echo "Enabled" || echo "Disabled")
    echo "$status|$last_active"
}

# Function to draw progress bar
draw_progress_bar() {
    local percent=$1
    local width=20
    local filled=$((width * percent / 100))
    local empty=$((width - filled))
    
    printf "["
    printf "%${filled}s" | tr ' ' '='
    printf "%${empty}s" | tr ' ' ' '
    printf "] %3d%%" $percent
}

# Function to display header
display_header() {
    clear
    echo -e "${BLUE}============================================${NC}"
    echo -e "${BLUE}       rAthena AI System Monitor v1.0       ${NC}"
    echo -e "${BLUE}============================================${NC}"
    echo "Press Ctrl+C to exit"
    echo ""
}

# Function to display system metrics
display_system_metrics() {
    echo -e "${BLUE}System Metrics:${NC}"
    echo -e "Memory Usage: $(get_memory_usage)"
    echo -e "CPU Usage: $(get_cpu_usage)"
    echo ""
}

# Function to display API metrics
display_api_metrics() {
    local metrics=$(get_api_metrics 300) # Last 5 minutes
    local requests=$(echo $metrics | cut -d'|' -f1)
    local errors=$(echo $metrics | cut -d'|' -f2)
    local latency=$(echo $metrics | cut -d'|' -f3)
    
    echo -e "${BLUE}API Metrics (Last 5 minutes):${NC}"
    echo -e "Requests: $requests"
    echo -e "Errors: $errors"
    printf "Average Latency: %.2fms\n" $latency
    echo ""
}

# Function to display agent status
display_agent_status() {
    echo -e "${BLUE}AI Agent Status:${NC}"
    
    local agents=("adaptive_balance" "dynamic_reward" "competitive_ranking" 
                 "player_engagement" "economy" "party_guild" "ai_coach"
                 "emotional_investment" "dynamic_monster" "social_influence"
                 "adaptive_quest")
    
    for agent in "${agents[@]}"; do
        local activity=$(get_agent_activity "$agent")
        local status=$(echo $activity | cut -d'|' -f1)
        local last_active=$(echo $activity | cut -d'|' -f2)
        
        if [ "$status" == "Enabled" ]; then
            echo -e "$agent: ${GREEN}●${NC} Active (Last: $last_active)"
        else
            echo -e "$agent: ${RED}○${NC} Inactive"
        fi
    done
    echo ""
}

# Function to display resource usage
display_resource_usage() {
    echo -e "${BLUE}Resource Usage:${NC}"
    
    local mem_percent=$(get_memory_usage | cut -d' ' -f1)
    local cpu_percent=$(get_cpu_usage | cut -d'%' -f1)
    
    echo -n "Memory: "
    draw_progress_bar ${mem_percent%.*}
    echo ""
    
    echo -n "CPU: "
    draw_progress_bar ${cpu_percent%.*}
    echo ""
    echo ""
}

# Function to display recent errors
display_recent_errors() {
    echo -e "${BLUE}Recent Errors:${NC}"
    local errors=$(grep -i "error\|failed\|critical" "$LOG_PATH" | tail -n 3)
    if [ ! -z "$errors" ]; then
        echo -e "${RED}$errors${NC}"
    else
        echo -e "${GREEN}No recent errors${NC}"
    fi
    echo ""
}

# Main monitoring loop
main() {
    # Find map server PID
    MAP_SERVER_PID=$(pgrep -f "map-server.*ai_system")
    if [ -z "$MAP_SERVER_PID" ]; then
        echo -e "${RED}Error: AI System is not running${NC}"
        exit 1
    fi
    
    # Hide cursor
    tput civis
    
    # Monitor loop
    while $MONITOR_ACTIVE; do
        display_header
        display_system_metrics
        display_api_metrics
        display_resource_usage
        display_agent_status
        display_recent_errors
        
        sleep $REFRESH_RATE
    done
}

# Start monitoring
main