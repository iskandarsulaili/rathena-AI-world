# P2P Technical Implementation

This document provides a detailed technical overview of the P2P networking implementation in the rAthena AI World client.

## 1. Architecture Overview

The P2P system uses a hybrid architecture that combines traditional client-server communication with direct peer-to-peer connections:

```
                  +----------------+
                  |                |
                  |  Game Server   |
                  |                |
                  +--------+-------+
                           |
                           | (Critical Data)
                           |
         +--------+--------+--------+--------+
         |        |        |        |        |
+--------v--+ +---v------+ +---v----+ +------v---+
|           | |          | |        | |          |
| Client A  <--> Client B <-> Client C <-> Client D |
|           | |          | |        | |          |
+-----------+ +----------+ +--------+ +----------+
      (P2P Connections for Non-Critical Data)
```

### 1.1 Connection Types

- **Client-Server**: Used for authentication, critical game state, transactions, and security
- **Peer-to-Peer**: Used for position updates, visual effects, chat, and other non-critical data

### 1.2 Network Topology

The P2P network uses a mesh topology with optimized connections:

- **Proximity-Based**: Clients connect primarily to peers in the same game area
- **Dynamic Meshing**: Connections are established and terminated based on proximity
- **Connection Limits**: Each client maintains a maximum of 10 peer connections by default
- **Server Coordination**: The server provides connection information to facilitate peer discovery

## 2. Protocol Specification

### 2.1 P2P Packet Structure

All P2P packets follow this structure:

```
+----------------+----------------+----------------+----------------+
| Packet Header  | Packet Type    | Payload Length | Payload        |
| (8 bytes)      | (2 bytes)      | (4 bytes)      | (variable)     |
+----------------+----------------+----------------+----------------+
```

#### 2.1.1 Packet Header

The packet header contains:
- Magic number (4 bytes): 0x50325032 ("P2P2")
- Protocol version (2 bytes)
- Flags (2 bytes)

#### 2.1.2 Packet Types

| Type ID | Name | Description |
|---------|------|-------------|
| 0x0001 | HANDSHAKE_REQUEST | Initial connection request |
| 0x0002 | HANDSHAKE_RESPONSE | Connection acceptance |
| 0x0003 | POSITION_UPDATE | Player position data |
| 0x0004 | VISUAL_EFFECT | Visual effect data |
| 0x0005 | CHAT_MESSAGE | Local chat message |
| 0x0006 | AI_INTERACTION | AI interaction data |
| 0x0007 | ENVIRONMENTAL | Environmental data |
| 0x0008 | PING | Connection test |
| 0x0009 | PONG | Connection test response |
| 0x000A | DISCONNECT | Graceful disconnection |

### 2.2 Encryption

All P2P packets are encrypted using:

1. **Handshake**: Diffie-Hellman key exchange for session key establishment
2. **Symmetric Encryption**: AES-256-GCM for packet payload encryption
3. **Authentication**: HMAC-SHA256 for packet authentication

### 2.3 Compression

Packet payloads are compressed using:

1. **Small Payloads** (<100 bytes): No compression
2. **Medium Payloads** (100-1000 bytes): LZ4 compression
3. **Large Payloads** (>1000 bytes): Zstandard compression

## 3. Implementation Details

### 3.1 Core Components

The P2P system is implemented in the following components:

#### 3.1.1 ai_network.dll

- **P2PManager**: Central management of P2P connections
- **P2PConnection**: Individual peer connection management
- **P2PDiscovery**: Peer discovery and NAT traversal
- **P2PPacketHandler**: Packet processing and routing
- **P2PEncryption**: Encryption and security

#### 3.1.2 Integration Points

The P2P system integrates with the client through:

- **NetworkHook**: Intercepts and redirects network traffic
- **GameStateHook**: Accesses and updates game state
- **RenderHook**: Processes visual effects from peers

### 3.2 NAT Traversal

NAT traversal is implemented using multiple techniques:

1. **STUN**: Server for identifying public IP/port
2. **TURN**: Relay server for difficult NAT configurations
3. **ICE**: Framework for connection establishment
4. **UPnP/NAT-PMP**: Automatic port forwarding when available
5. **Hole Punching**: Direct connection technique for symmetric NATs

### 3.3 Bandwidth Management

Bandwidth is managed through:

1. **Prioritization**: Critical data gets bandwidth priority
2. **Rate Limiting**: Configurable per-connection limits
3. **Adaptive Quality**: Reduces data detail under bandwidth constraints
4. **Batching**: Combines multiple small packets
5. **Delta Compression**: Only sends changes from previous state

## 4. Data Synchronization

### 4.1 Position Synchronization

Player positions are synchronized using:

1. **Dead Reckoning**: Predicts movement between updates
2. **Position Interpolation**: Smooths movement between updates
3. **Jitter Buffer**: Compensates for network inconsistency
4. **Authoritative Correction**: Server can override positions

### 4.2 Visual Effect Synchronization

Visual effects are synchronized using:

1. **Effect ID**: Identifies the effect type
2. **Parameters**: Effect-specific parameters
3. **Position**: World coordinates
4. **Timing**: Start time and duration
5. **Owner**: Entity that created the effect

### 4.3 AI Interaction Synchronization

AI interactions are synchronized using:

1. **Interaction ID**: Identifies the interaction type
2. **AI Entity ID**: Identifies the AI entity
3. **Player ID**: Identifies the player
4. **Parameters**: Interaction-specific parameters
5. **Result**: Outcome of the interaction

## 5. Security Measures

### 5.1 Anti-Tampering

The P2P system includes anti-tampering measures:

1. **Packet Validation**: Validates all incoming packets
2. **Anomaly Detection**: Identifies abnormal packet patterns
3. **Rate Limiting**: Prevents packet flooding
4. **Signature Verification**: Verifies packet signatures
5. **Replay Protection**: Prevents packet replay attacks

### 5.2 Cheating Prevention

The P2P system prevents common cheating techniques:

1. **Position Validation**: Detects impossible movements
2. **Speed Hacking Detection**: Identifies abnormal movement speeds
3. **Wall Hacking Prevention**: Validates line-of-sight for interactions
4. **Information Hiding**: Only sends data the peer should know
5. **Server Verification**: Critical actions verified by server

## 6. Performance Optimization

### 6.1 CPU Optimization

CPU usage is optimized through:

1. **Multi-threading**: Separate threads for network, processing, and encryption
2. **Batch Processing**: Processes multiple packets in a batch
3. **Lock-Free Algorithms**: Minimizes thread contention
4. **SIMD Instructions**: Uses CPU vector instructions when available
5. **Lazy Evaluation**: Only processes data when needed

### 6.2 Memory Optimization

Memory usage is optimized through:

1. **Object Pooling**: Reuses packet objects
2. **Zero-Copy Design**: Minimizes data copying
3. **Memory Mapping**: Efficient large data handling
4. **Compact Data Structures**: Minimizes memory footprint
5. **Garbage Collection**: Timely cleanup of unused resources

## 7. Configuration Options

The P2P system can be configured through the `ai_client.ini` file:

```ini
[P2P]
enabled=true                # Enable/disable P2P functionality
max_peers=10                # Maximum number of peer connections
bandwidth_limit=50          # Bandwidth limit in KB/s
encryption_level=high       # Encryption level (low, medium, high)
nat_traversal=true          # Enable NAT traversal
port_range=7000-7100        # Port range for P2P connections
discovery_interval=30       # Peer discovery interval in seconds
handshake_timeout=5         # Handshake timeout in seconds
inactive_timeout=60         # Inactive connection timeout in seconds
position_update_rate=5      # Position updates per second
effect_update_rate=10       # Effect updates per second
compression_threshold=100   # Minimum size for compression in bytes
log_level=info              # Logging level (debug, info, warn, error)
```

## 8. Debugging and Monitoring

### 8.1 Logging

The P2P system logs information to:

1. **File Log**: `Client/System/logs/p2p.log`
2. **Console**: When debug mode is enabled
3. **Server**: Critical issues are reported to the server

### 8.2 Statistics

The P2P system collects the following statistics:

1. **Connection Count**: Number of active connections
2. **Bandwidth Usage**: Incoming and outgoing bandwidth
3. **Packet Counts**: Sent, received, dropped packets
4. **Latency**: Average, minimum, maximum latency
5. **Error Rates**: Packet errors, retransmissions

### 8.3 Visualization

When debug mode is enabled, the P2P system can visualize:

1. **Connection Graph**: Visual representation of peer connections
2. **Bandwidth Graph**: Real-time bandwidth usage
3. **Packet Flow**: Visual representation of packet flow
4. **Error Indicators**: Visual indicators for connection issues

## 9. Integration with AI Features

### 9.1 AI Legend Data Sharing

AI Legend data is shared via P2P:

1. **Basic Information**: Name, appearance, location
2. **Interaction State**: Current interaction state
3. **Visual Effects**: Special effects and animations
4. **Ambient Dialogue**: Non-critical dialogue

### 9.2 AI-Influenced Environment

AI-influenced environmental data is shared via P2P:

1. **Weather Effects**: AI-influenced weather
2. **Time Effects**: AI-influenced time changes
3. **Ambient Effects**: Background effects and animations
4. **Environmental Sounds**: Ambient sound effects

## 10. Error Handling and Recovery

### 10.1 Connection Failures

The P2P system handles connection failures through:

1. **Automatic Reconnection**: Attempts to reestablish dropped connections
2. **Fallback to Server**: Routes critical data through server when P2P fails
3. **Connection Pooling**: Maintains backup connections
4. **Graceful Degradation**: Reduces functionality rather than failing completely

### 10.2 Data Corruption

The P2P system handles data corruption through:

1. **Checksums**: Validates packet integrity
2. **Error Correction**: Corrects minor errors when possible
3. **Packet Retransmission**: Requests retransmission of corrupted packets
4. **State Synchronization**: Periodically synchronizes full state to correct drift