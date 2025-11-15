#!/bin/bash

# ============================================
# AI-rAthena Quick Start Script
# ============================================
# Starts all services in screen sessions for development/testing
# No systemd required - useful for development

set -e

# Colors
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

print_step() {
    echo -e "${BLUE}[START]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

# Check if .env exists
if [ ! -f .env ]; then
    print_warning ".env file not found! Creating from template..."
    cp .env.example .env
    print_warning "Please edit .env and add your API keys before continuing!"
    echo "Press Enter to continue after editing .env, or Ctrl+C to exit..."
    read
fi

# Load environment variables
source .env

# Check for API keys
if [ -z "$OPENAI_API_KEY" ] && [ -z "$ANTHROPIC_API_KEY" ] && [ -z "$GOOGLE_API_KEY" ] && [ -z "$AZURE_OPENAI_API_KEY" ]; then
    print_warning "No LLM API key found in .env!"
    print_warning "Please set at least one: OPENAI_API_KEY, ANTHROPIC_API_KEY, GOOGLE_API_KEY, or AZURE_OPENAI_API_KEY"
    exit 1
fi

print_step "Starting AI-rAthena services..."

# ============================================
# 1. Check PostgreSQL
# ============================================
print_step "Checking PostgreSQL..."
if ! sudo systemctl is-active --quiet postgresql; then
    print_step "Starting PostgreSQL..."
    sudo systemctl start postgresql
fi
print_success "PostgreSQL is running"

# ============================================
# 2. Check Redis
# ============================================
print_step "Checking DragonflyDB..."
if sudo systemctl is-active --quiet dragonfly; then
    print_success "DragonflyDB is running"
else
    print_step "Starting DragonflyDB..."
    sudo systemctl start dragonfly
    if sudo systemctl is-active --quiet dragonfly; then
        print_success "DragonflyDB started"
    else
        print_warning "DragonflyDB could not be started. Please check the service status."
    fi
fi

# ============================================
# 5. Start AIWorld Server (ZeroMQ IPC)
# ============================================
print_step "Starting AIWorld Server..."
if [ -f src/aiworld/aiworld_server ]; then
    screen -S aiworld-server -X quit 2>/dev/null || true
    screen -dmS aiworld-server bash -c "./src/aiworld/aiworld_server"
    print_success "AIWorld Server started"
else
    print_warning "AIWorld Server binary not found at src/aiworld/aiworld_server"
fi

# ============================================
# 6. Start P2P Coordinator Service
# ============================================
print_step "Starting P2P Coordinator Service..."
if [ -d p2p-coordinator/venv ]; then
    screen -S p2p-coordinator -X quit 2>/dev/null || true
    screen -dmS p2p-coordinator bash -c "cd p2p-coordinator/coordinator-service && ../venv/bin/python main.py"
    print_success "P2P Coordinator Service started"
else
    print_warning "P2P Coordinator virtual environment not found. Please set up Python venv in p2p-coordinator."
fi

# ============================================
# 3. Start AI Service
# ============================================
print_step "Starting AI Service..."

# Kill existing screen session if it exists
screen -S ai-service -X quit 2>/dev/null || true

# Start AI service in screen
cd ai-autonomous-world/ai-service
screen -dmS ai-service bash -c "source venv/bin/activate && uvicorn main:app --host 0.0.0.0 --port 8000 --workers 4 --log-level info"
cd ../..

# Wait for AI service to start
sleep 3

# Check if AI service is running
if curl -f http://localhost:8000/health > /dev/null 2>&1; then
    print_success "AI Service started (http://localhost:8000)"
else
    print_warning "AI Service may still be starting..."
fi

# ============================================
# 4. Start rAthena Map Server
# ============================================
print_step "Starting rAthena Map Server..."

# Kill existing screen session if it exists
screen -S rathena-map -X quit 2>/dev/null || true

# Start map server in screen
screen -dmS rathena-map bash -c "./map-server"

print_success "rAthena Map Server started"

# ============================================
# Display Status
# ============================================
echo ""
echo "============================================"
print_success "All services started!"
echo "============================================"
echo ""
echo "Service URLs:"
echo "  AI Service API:  http://localhost:8000"
echo "  AI Service Docs: http://localhost:8000/docs"
echo "  rAthena Map:     localhost:5121"
echo "  rAthena Web:     http://localhost:8888"
echo ""
echo "Screen Sessions:"
echo "  AI Service:      screen -r ai-service"
echo "  rAthena Map:     screen -r rathena-map"
echo ""
echo "View Logs:"
echo "  screen -r ai-service    (Ctrl+A, D to detach)"
echo "  screen -r rathena-map   (Ctrl+A, D to detach)"
echo ""
echo "Stop Services:"
echo "  ./stop-services.sh"
echo ""
echo "Test Installation:"
echo "  curl http://localhost:8000/health"
echo "  python3 tests/e2e_test.py"
echo ""
echo "============================================"

