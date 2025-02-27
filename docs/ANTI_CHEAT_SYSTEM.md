# Anti-Cheat System

This document provides a detailed overview of the anti-cheat system implemented in the rAthena AI World client.

## 1. System Architecture

The anti-cheat system uses a layered approach to detect and prevent cheating:

```
+---------------------+
| Server-Side Checks  |
+---------------------+
          ↑
          ↓
+---------------------+
| Client-Side Checks  |
+---------------------+
          ↑
          ↓
+---------------------+
| Kernel-Level Hooks  |
+---------------------+
          ↑
          ↓
+---------------------+
| Game Process        |
+---------------------+
```

### 1.1 Protection Layers

1. **Kernel-Level Hooks**: Monitors system calls and memory access
2. **Client-Side Checks**: Performs integrity checks and behavior analysis
3. **Server-Side Checks**: Validates client actions and detects anomalies

### 1.2 Component Integration

The anti-cheat system is integrated with the client through:

- **Memory Protection**: Prevents unauthorized memory access
- **API Hooking**: Intercepts and validates API calls
- **Process Monitoring**: Detects suspicious processes
- **Network Validation**: Validates network traffic

## 2. Detection Methods

### 2.1 Memory Scanning

The anti-cheat system scans memory for:

1. **Known Signatures**: Patterns matching known cheat tools
2. **Code Injection**: Unauthorized code in the process memory
3. **Memory Modification**: Changes to protected memory regions
4. **Hook Detection**: Unauthorized API hooks

Memory scanning is performed:
- At game startup
- Periodically during gameplay
- When suspicious activity is detected

### 2.2 Behavior Analysis

The anti-cheat system analyzes player behavior for:

1. **Movement Patterns**: Detecting impossible or unnatural movement
2. **Action Timing**: Identifying inhuman reaction times
3. **Input Patterns**: Detecting automated inputs
4. **Resource Usage**: Identifying abnormal resource gathering rates
5. **Combat Patterns**: Detecting inhuman combat performance

Behavior analysis uses:
- Statistical models
- Machine learning algorithms
- Pattern recognition
- Anomaly detection

### 2.3 Code Integrity

The anti-cheat system verifies code integrity through:

1. **Checksum Verification**: Validates file checksums
2. **Code Signing**: Verifies digital signatures
3. **Runtime Verification**: Checks code integrity during execution
4. **Import Table Validation**: Verifies DLL imports
5. **Self-Checking Code**: Code that validates its own integrity

### 2.4 External Process Detection

The anti-cheat system detects suspicious external processes:

1. **Process Enumeration**: Identifies running processes
2. **Window Detection**: Finds hidden windows
3. **Driver Detection**: Identifies suspicious drivers
4. **DLL Injection Detection**: Detects injected DLLs
5. **Handle Monitoring**: Tracks process handles

## 3. Prevention Techniques

### 3.1 Obfuscation

The anti-cheat system uses obfuscation to protect itself:

1. **Code Obfuscation**: Obscures code functionality
2. **String Encryption**: Encrypts string literals
3. **Control Flow Obfuscation**: Complicates control flow
4. **Anti-Debugging**: Detects and prevents debugging
5. **Self-Modifying Code**: Code that modifies itself during execution

### 3.2 Encryption

The anti-cheat system uses encryption to protect:

1. **Memory**: Encrypts sensitive memory regions
2. **Network Traffic**: Encrypts client-server communication
3. **Configuration**: Encrypts configuration settings
4. **Game Data**: Encrypts critical game data
5. **Anti-Cheat Data**: Encrypts detection signatures

### 3.3 Virtualization

The anti-cheat system uses virtualization to:

1. **Hide Code**: Executes code in a virtual machine
2. **Protect Data**: Stores sensitive data in virtualized memory
3. **Detect Tampering**: Identifies virtualization attacks
4. **Isolate Execution**: Runs critical code in isolated environments

## 4. Implementation Details

### 4.1 Memory Protection

Memory protection is implemented through:

```cpp
// Example memory protection implementation
class MemoryProtector {
private:
    struct ProtectedRegion {
        void* address;
        size_t size;
        uint32_t checksum;
    };
    
    std::vector<ProtectedRegion> regions;
    
public:
    void ProtectRegion(void* address, size_t size) {
        ProtectedRegion region;
        region.address = address;
        region.size = size;
        region.checksum = CalculateChecksum(address, size);
        regions.push_back(region);
        
        // Apply memory protection
        VirtualProtect(address, size, PAGE_READONLY, NULL);
    }
    
    bool ValidateRegions() {
        for (const auto& region : regions) {
            uint32_t currentChecksum = CalculateChecksum(region.address, region.size);
            if (currentChecksum != region.checksum) {
                // Memory tampering detected
                return false;
            }
        }
        return true;
    }
    
private:
    uint32_t CalculateChecksum(void* address, size_t size) {
        // Calculate checksum of memory region
        // ...
    }
};
```

### 4.2 API Hooking Detection

API hooking detection is implemented through:

```cpp
// Example API hook detection
class APIHookDetector {
public:
    bool DetectHooks() {
        // Check for hooks in critical functions
        if (IsHooked(&CreateFileW) ||
            IsHooked(&ReadFile) ||
            IsHooked(&WriteFile) ||
            IsHooked(&VirtualProtect) ||
            IsHooked(&VirtualAlloc)) {
            // API hooks detected
            return true;
        }
        return false;
    }
    
private:
    bool IsHooked(void* functionAddress) {
        uint8_t* bytes = (uint8_t*)functionAddress;
        
        // Check for common hook patterns
        // JMP instruction (0xE9)
        if (bytes[0] == 0xE9) {
            return true;
        }
        
        // CALL instruction (0xE8)
        if (bytes[0] == 0xE8) {
            return true;
        }
        
        // MOV/JMP pattern
        if (bytes[0] == 0xFF && (bytes[1] == 0x25 || bytes[1] == 0x15)) {
            return true;
        }
        
        return false;
    }
};
```

### 4.3 Process Monitoring

Process monitoring is implemented through:

```cpp
// Example process monitoring
class ProcessMonitor {
public:
    std::vector<std::string> DetectSuspiciousProcesses() {
        std::vector<std::string> suspiciousProcesses;
        
        // Enumerate all processes
        DWORD processes[1024], bytesReturned;
        if (EnumProcesses(processes, sizeof(processes), &bytesReturned)) {
            DWORD numProcesses = bytesReturned / sizeof(DWORD);
            
            for (DWORD i = 0; i < numProcesses; i++) {
                if (processes[i] != 0) {
                    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processes[i]);
                    if (hProcess) {
                        char processName[MAX_PATH] = "<unknown>";
                        GetModuleBaseNameA(hProcess, NULL, processName, sizeof(processName));
                        
                        // Check against blacklist
                        if (IsBlacklisted(processName)) {
                            suspiciousProcesses.push_back(processName);
                        }
                        
                        CloseHandle(hProcess);
                    }
                }
            }
        }
        
        return suspiciousProcesses;
    }
    
private:
    bool IsBlacklisted(const std::string& processName) {
        // Check against blacklist of known cheat tools
        static const std::vector<std::string> blacklist = {
            "cheatengine.exe",
            "ollydbg.exe",
            "x64dbg.exe",
            "ida64.exe",
            "wireshark.exe",
            "fiddler.exe",
            // Add more as needed
        };
        
        for (const auto& blacklistedProcess : blacklist) {
            if (_stricmp(processName.c_str(), blacklistedProcess.c_str()) == 0) {
                return true;
            }
        }
        
        return false;
    }
};
```

### 4.4 Behavior Analysis

Behavior analysis is implemented through:

```cpp
// Example behavior analysis
class BehaviorAnalyzer {
private:
    struct MovementData {
        float x, y, z;
        uint64_t timestamp;
    };
    
    std::deque<MovementData> movementHistory;
    
public:
    void RecordMovement(float x, float y, float z) {
        MovementData data;
        data.x = x;
        data.y = y;
        data.z = z;
        data.timestamp = GetCurrentTimestamp();
        
        movementHistory.push_back(data);
        
        // Keep history limited to prevent memory growth
        if (movementHistory.size() > 100) {
            movementHistory.pop_front();
        }
    }
    
    bool DetectSpeedHack() {
        if (movementHistory.size() < 2) {
            return false;
        }
        
        const float MAX_SPEED = 20.0f; // Maximum allowed speed
        
        for (size_t i = 1; i < movementHistory.size(); i++) {
            const auto& prev = movementHistory[i-1];
            const auto& curr = movementHistory[i];
            
            // Calculate distance
            float dx = curr.x - prev.x;
            float dy = curr.y - prev.y;
            float dz = curr.z - prev.z;
            float distance = sqrt(dx*dx + dy*dy + dz*dz);
            
            // Calculate time difference in seconds
            float timeDiff = (curr.timestamp - prev.timestamp) / 1000.0f;
            
            // Calculate speed
            float speed = distance / timeDiff;
            
            if (speed > MAX_SPEED) {
                // Speed hack detected
                return true;
            }
        }
        
        return false;
    }
    
private:
    uint64_t GetCurrentTimestamp() {
        // Get current time in milliseconds
        // ...
    }
};
```

## 5. Integration with AI Features

### 5.1 AI-Enhanced Detection

The anti-cheat system uses AI to enhance detection:

1. **Pattern Recognition**: Identifies complex cheat patterns
2. **Anomaly Detection**: Detects unusual behavior
3. **Adaptive Learning**: Improves detection over time
4. **False Positive Reduction**: Minimizes false positives

### 5.2 AI Legend Protection

The anti-cheat system protects AI Legend features:

1. **Interaction Validation**: Validates player interactions with AI Legends
2. **Response Protection**: Prevents tampering with AI responses
3. **Memory Protection**: Protects AI memory contexts
4. **Behavior Monitoring**: Monitors AI behavior for manipulation

## 6. Configuration Options

The anti-cheat system can be configured through the `ai_client.ini` file:

```ini
[AntiCheat]
enabled=true                # Enable/disable anti-cheat functionality
scan_interval=300           # Memory scan interval in seconds
report_level=medium         # Reporting level (low, medium, high)
auto_update=true            # Automatically update anti-cheat definitions
performance_impact=low      # Performance impact level (low, medium, high)
kernel_mode=false           # Enable kernel-mode protection
process_blacklist=true      # Enable process blacklisting
behavior_analysis=true      # Enable behavior analysis
memory_protection=true      # Enable memory protection
api_hook_detection=true     # Enable API hook detection
debug_protection=true       # Enable anti-debugging protection
```

## 7. Reporting and Enforcement

### 7.1 Report Types

The anti-cheat system generates the following report types:

1. **Warning**: Suspicious but not conclusive
2. **Detection**: Confirmed cheat detection
3. **Critical**: Severe cheat detection
4. **System**: Anti-cheat system status

### 7.2 Report Handling

Reports are handled through:

1. **Local Logging**: Saved to local log files
2. **Server Reporting**: Sent to the game server
3. **Immediate Action**: Triggers immediate response
4. **Delayed Action**: Triggers delayed response

### 7.3 Enforcement Actions

The anti-cheat system can trigger the following enforcement actions:

1. **Warning**: Display warning message
2. **Restriction**: Restrict certain game features
3. **Timeout**: Temporary ban
4. **Ban**: Permanent ban
5. **Hardware Ban**: Ban based on hardware identifiers

## 8. Performance Considerations

### 8.1 CPU Usage

The anti-cheat system optimizes CPU usage through:

1. **Adaptive Scanning**: Adjusts scan frequency based on system load
2. **Prioritized Checks**: Prioritizes critical checks
3. **Background Processing**: Performs intensive checks in background threads
4. **Idle-Time Processing**: Utilizes CPU idle time

### 8.2 Memory Usage

The anti-cheat system optimizes memory usage through:

1. **Minimal Footprint**: Keeps memory usage low
2. **Resource Sharing**: Shares resources between components
3. **On-Demand Loading**: Loads components only when needed
4. **Efficient Data Structures**: Uses memory-efficient data structures

## 9. Security Measures

### 9.1 Self-Protection

The anti-cheat system protects itself through:

1. **Self-Monitoring**: Monitors its own integrity
2. **Redundant Checks**: Implements multiple overlapping checks
3. **Stealth Operation**: Hides its presence and operation
4. **Unpredictable Behavior**: Varies its behavior to prevent analysis

### 9.2 Update Mechanism

The anti-cheat system updates itself through:

1. **Secure Downloads**: Downloads updates securely
2. **Signature Verification**: Verifies update signatures
3. **Incremental Updates**: Updates only changed components
4. **Fallback Mechanism**: Reverts to previous version on failure

## 10. Privacy Considerations

The anti-cheat system respects privacy through:

1. **Minimal Data Collection**: Collects only necessary data
2. **Data Anonymization**: Anonymizes collected data
3. **Transparent Operation**: Clearly communicates its operation
4. **User Control**: Provides options to control privacy impact

### 10.1 Data Collection

The anti-cheat system collects:

1. **System Information**: Hardware and software configuration
2. **Process List**: Running processes during gameplay
3. **Game Behavior**: In-game actions and patterns
4. **Detection Events**: Cheat detection events

### 10.2 Data Handling

Collected data is handled according to:

1. **Privacy Policy**: Adheres to the game's privacy policy
2. **Data Retention**: Retains data only as long as necessary
3. **Data Security**: Secures data during storage and transmission
4. **Data Access**: Restricts access to authorized personnel