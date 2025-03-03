# P2P Hosting Implementation for rAthena

## System Architecture Overview

### Components
1. Central Coordinator Server (CCS)
   - Manages P2P host selection and assignment
   - Maintains host health metrics
   - Handles failover coordination
   - Proxies database operations

2. P2P Host Node
   - Player client with high-performance capabilities
   - Runs lightweight map server instance
   - Handles local map data and events
   - Syncs with CCS for data persistence

3. VPS Backup Servers
   - Runs full rAthena map server instance
   - Acts as fallback when no P2P hosts available
   - Provides redundancy for critical maps

4. Modified Client (WARP Integration)
   - Enhanced with P2P hosting capabilities
   - Includes host eligibility checking
   - Handles secure map server operations
   - Maintains connection management

### Communication Flow
```
[Client] <-> [P2P Host/VPS] <-> [CCS] <-> [Database]
                    ^
                    |
     [Performance Metrics & Health Checks]
```

## P2P Host Selection & Management

### Host Eligibility Criteria
- Minimum System Requirements:
  - CPU: 4+ cores
  - RAM: 8GB+
  - Network: <100ms latency, stable connection
  - Upload bandwidth: 10Mbps+
  - Uptime: >30 minutes

### Selection Algorithm
```python
def select_host(map_id, available_hosts):
    eligible_hosts = filter_by_requirements(available_hosts)
    if not eligible_hosts:
        return fallback_to_vps()
        
    ranked_hosts = rank_by_metrics(eligible_hosts)
    # Prioritize by:
    # 1. Performance score
    # 2. Geographic proximity
    # 3. Current load
    return ranked_hosts[0]
```

### Host Health Monitoring
- Real-time metrics collection:
  - CPU usage
  - Memory availability
  - Network latency
  - Connected players
  - Error rates

### Failover Protocol
1. Health check failure detected
2. CCS initiates host transition
3. Notify affected clients
4. Redirect to new host/VPS
5. Sync state to new host
6. Verify transition completion

## Client-Side Modifications (WARP Integration)

### WARP Patches Required
1. Enable P2P hosting capabilities:
```c
// Add to ragexe
struct P2P_HOST_CONFIG {
    bool enabled;
    uint32 performance_score;
    uint16 max_players;
    char* host_token;
};

void initialize_p2p_hosting() {
    if (check_system_requirements()) {
        p2p_config.enabled = true;
        p2p_config.performance_score = calculate_performance_score();
        // Register with CCS
        register_as_host();
    }
}
```

2. Map server hosting module:
```c
class P2PMapServer {
    private:
        map_session_data* sessions;
        struct map_data* map;
        
    public:
        bool initialize_map(int map_id);
        void process_player_events();
        void sync_with_ccs();
        void handle_player_connection(session_data* sd);
};
```

### Security Measures
- Secure communication channels using TLS
- Host verification through CCS
- Regular integrity checks
- Rate limiting and abuse prevention

## Map Server Distribution System

### Map Classification
1. Critical Maps (Always VPS-hosted):
   - Login areas
   - Main cities
   - PvP zones
   - Guild castles

2. P2P-Eligible Maps:
   - Field maps
   - Dungeons
   - Instance maps
   - Event areas

### Distribution Logic
```python
def assign_map_server(map_id, player):
    if is_critical_map(map_id):
        return get_vps_server()
        
    host = select_p2p_host(map_id)
    if host:
        return host
    return get_fallback_vps()
```

### State Synchronization
1. Initial state transfer
2. Delta updates
3. Periodic full sync
4. Conflict resolution

## Secure Database Synchronization

### Architecture
```
[P2P Host] -> [CCS Proxy] -> [Database]
     ^            |
     |            v
[Encrypted Channel] [Access Control]
```

### Security Measures
1. Proxy Layer:
   - No direct database access from P2P hosts
   - Token-based authentication
   - Request validation
   - Rate limiting

2. Data Protection:
```python
class DatabaseProxy:
    def sync_data(self, host_id, data):
        if not verify_host_token(host_id):
            return ERROR_UNAUTHORIZED
            
        sanitized_data = sanitize_input(data)
        encrypted_data = encrypt_payload(sanitized_data)
        
        return db_controller.update(encrypted_data)
```

3. Real-time Sync Protocol:
```javascript
{
    "type": "db_sync",
    "host_id": "p2p_host_uuid",
    "timestamp": 1638262400,
    "data": {
        "map_state": {...},
        "player_updates": [...],
        "event_logs": [...]
    },
    "checksum": "sha256_hash"
}
```

## Implementation Steps

1. Core Infrastructure:
   - Set up CCS infrastructure
   - Implement host selection system
   - Create database proxy layer

2. Client Modifications:
   - Patch ragexe with WARP
   - Add P2P hosting capabilities
   - Implement security measures

3. Map Server Integration:
   - Modify map server for P2P hosting
   - Implement state synchronization
   - Add failover handling

4. Testing & Deployment:
   - Performance testing
   - Security audits
   - Gradual rollout

## Fallback Mechanisms

### VPS Failover
1. Continuous VPS availability monitoring
2. Automatic traffic rerouting
3. State recovery procedures

### Network Partitioning
1. Detection mechanisms
2. Temporary state handling
3. Reconciliation procedures

## Security Considerations

1. Host Verification
   - Digital signatures
   - Certificate-based authentication
   - Regular re-verification

2. Data Protection
   - End-to-end encryption
   - Secure key distribution
   - Access control lists

3. Anti-Cheat Measures
   - Server-side validation
   - Behavior monitoring
   - Integrity checking

## Performance Optimizations

1. Load Balancing
   - Dynamic host assignment
   - Geographic distribution
   - Load prediction

2. State Management
   - Incremental updates
   - Compression
   - Caching strategies

3. Network Optimization
   - Protocol efficiency
   - Bandwidth management
   - Latency minimization