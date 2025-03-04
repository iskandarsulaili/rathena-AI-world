#!/bin/bash

# AI System Status Tool
# Displays current status of the AI system and all agents

# Color codes
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

# Configuration
MAP_SERVER_PORT=5121
CONF_PATH="../conf/ai_agents.conf"
LOG_PATH="../log/map_ai.log"

# Function to print header
print_header() {
    echo -e "${BLUE}============================================${NC}"
    echo -e "${BLUE}          rAthena AI System Status          ${NC}"
    echo -e "${BLUE}============================================${NC}"
    echo ""
}

# Function to check if AI system is running
check_system_status() {
    if pgrep -f "map-server.*ai_system" > /dev/null; then
        echo -e "System Status: ${GREEN}Running${NC}"
        return 0
    else
        echo -e "System Status: ${RED}Not Running${NC}"
        return 1
    fi
}

# Function to get agent status
get_agent_status() {
    local agent=$1
    if grep -q "^[[:space:]]*${agent}.*enabled:[[:space:]]*true" "$CONF_PATH" 2>/dev/null; then
        if grep -q "${agent}.*initialized successfully" "$LOG_PATH" 2>/dev/null; then
            echo -e "${GREEN}Active${NC}"
        else
            echo -e "${YELLOW}Enabled but not running${NC}"
        fi
    else
        echo -e "${RED}Disabled${NC}"
    fi
}

# Function to get memory usage
get_memory_usage() {
    local pid=$(pgrep -f "map-server.*ai_system")
    if [ ! -z "$pid" ]; then
        local mem=$(ps -o rss= -p "$pid" | awk '{print $1/1024}')
        printf "%.2f MB" $mem
    else
        echo "N/A"
    fi
}

# Function to get AI system metrics
get_system_metrics() {
    echo -e "${BLUE}System Metrics:${NC}"
    echo -e "Memory Usage: $(get_memory_usage)"
    echo -e "API Requests (last hour): $(grep "API request" "$LOG_PATH" | wc -l)"
    echo -e "Active Connections: $(netstat -an | grep ":$MAP_SERVER_PORT" | grep ESTABLISHED | wc -l)"
}

# Function to check AI models
check_ai_models() {
    echo -e "${BLUE}AI Models:${NC}"
    
    # Check Azure OpenAI
    if grep -q "azure_openai.*api_key:[[:space:]]*\"[^\"]\+\"" "$CONF_PATH" 2>/dev/null; then
        echo -e "Azure OpenAI: ${GREEN}Configured${NC}"
    else
        echo -e "Azure OpenAI: ${RED}Not Configured${NC}"
    fi
    
    # Check OpenAI GPT-4
    if grep -q "openai_gpt4.*api_key:[[:space:]]*\"[^\"]\+\"" "$CONF_PATH" 2>/dev/null; then
        echo -e "OpenAI GPT-4: ${GREEN}Configured${NC}"
    else
        echo -e "OpenAI GPT-4: ${RED}Not Configured${NC}"
    fi
    
    # Check DeepSeek
    if grep -q "deepseek_v3.*api_key:[[:space:]]*\"[^\"]\+\"" "$CONF_PATH" 2>/dev/null; then
        echo -e "DeepSeek v3: ${GREEN}Configured${NC}"
    else
        echo -e "DeepSeek v3: ${RED}Not Configured${NC}"
    fi
}

# Function to display agent status
display_agent_status() {
    echo -e "${BLUE}AI Agents:${NC}"
    echo -e "Adaptive Balance AI: $(get_agent_status "adaptive_balance")"
    echo -e "Dynamic Reward AI: $(get_agent_status "dynamic_reward")"
    echo -e "Competitive Ranking AI: $(get_agent_status "competitive_ranking")"
    echo -e "Player Engagement AI: $(get_agent_status "player_engagement")"
    echo -e "Adaptive Economy AI: $(get_agent_status "economy")"
    echo -e "Smart Party & Guild AI: $(get_agent_status "party_guild")"
    echo -e "Personalized AI Coach: $(get_agent_status "ai_coach")"
    echo -e "Emotional Investment AI: $(get_agent_status "emotional_investment")"
    echo -e "Dynamic AI Monster: $(get_agent_status "dynamic_monster")"
    echo -e "Social Influence AI: $(get_agent_status "social_influence")"
    echo -e "Adaptive Quest AI: $(get_agent_status "adaptive_quest")"
}

# Function to check recent errors
check_recent_errors() {
    echo -e "${BLUE}Recent Errors (last hour):${NC}"
    local errors=$(grep -i "error\|failed\|critical" "$LOG_PATH" | tail -n 5)
    if [ ! -z "$errors" ]; then
        echo -e "${RED}$errors${NC}"
    else
        echo -e "${GREEN}No recent errors${NC}"
    fi
}

# Main execution
print_header

if check_system_status; then
    echo ""
    get_system_metrics
    echo ""
    check_ai_models
    echo ""
    display_agent_status
    echo ""
    check_recent_errors
else
    echo -e "${RED}AI System is not running. Please start the server first.${NC}"
fi

echo ""
echo -e "${BLUE}============================================${NC}"