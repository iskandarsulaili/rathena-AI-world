# Network Configuration Management

This document describes how to manage and validate the P2P network configuration settings.

## Configuration Files

The main network configuration file is located at:
```
conf/p2p/network_quality.conf
```

This file contains all settings related to:
- Network quality thresholds
- Performance scoring weights
- Testing parameters
- Geographic zones
- Recovery and failover settings

## Validation Tools

### Command-Line Tools

1. **validate_network_config**
   - Binary executable for validating configuration files
   - Built automatically during compilation
   - Performs detailed validation of all settings

2. **check_network_config.sh**
   - User-friendly shell script wrapper
   - Provides colored output and automatic fixes
   - Maintains configuration backups

### Usage Examples

```bash
# Basic configuration check
./tools/check_network_config.sh

# Validate with detailed output
./tools/check_network_config.sh -v

# Attempt to fix configuration issues
./tools/check_network_config.sh -f

# Check specific configuration file
./tools/check_network_config.sh -v my_custom_config.conf
```

## Configuration Guidelines

### Network Quality Thresholds

```ini
[thresholds]
# Latency thresholds (milliseconds)
latency_warning = 100    # Warning level
latency_critical = 200   # Critical level
latency_max = 300        # Maximum acceptable

# Bandwidth requirements (Mbps)
bandwidth_min = 5        # Minimum required
bandwidth_optimal = 10   # Optimal level
bandwidth_warning = 3    # Warning threshold
```

### Scoring Weights

```ini
[scoring]
# Must sum to 1.0
latency_weight = 0.4    # Latency importance
bandwidth_weight = 0.3  # Bandwidth importance
stability_weight = 0.3  # Stability importance
```

### Testing Parameters

```ini
[testing]
ping_interval = 30        # Seconds between ping tests
full_test_interval = 300  # Seconds between full tests
ping_samples = 10         # Number of samples per test
```

## Best Practices

1. **Regular Validation**
   - Run configuration validation before deployment
   - Validate after any configuration changes
   - Set up automated validation in CI/CD pipeline

2. **Performance Tuning**
   ```ini
   # Start with conservative thresholds
   latency_warning = 150
   bandwidth_min = 5
   
   # Adjust based on monitoring data
   latency_warning = 100
   bandwidth_min = 10
   ```

3. **Geographic Configuration**
   ```ini
   # Define zones based on network quality
   [geographic]
   quality_zones = [
       "premium",    # <50ms, >20Mbps
       "standard",   # <100ms, >10Mbps
       "basic"       # <200ms, >5Mbps
   ]
   ```

4. **Backup and Recovery**
   - Keep backup of working configurations
   - Use version control for configuration files
   - Document all configuration changes

## Common Issues and Solutions

### 1. Scoring Weight Imbalance
```ini
# Problem:
latency_weight = 0.5
bandwidth_weight = 0.4
stability_weight = 0.4  # Total > 1.0

# Solution:
latency_weight = 0.4
bandwidth_weight = 0.3
stability_weight = 0.3  # Total = 1.0
```

### 2. Threshold Conflicts
```ini
# Problem:
latency_warning = 200
latency_critical = 150  # Warning > Critical

# Solution:
latency_warning = 100
latency_critical = 200  # Warning < Critical
```

### 3. Resource Overutilization
```ini
# Problem:
ping_interval = 1    # Too frequent
test_samples = 1000  # Too many samples

# Solution:
ping_interval = 30   # Every 30 seconds
test_samples = 10    # Reasonable sample size
```

## Runtime Configuration

The NetworkConfig class provides type-safe access to configuration values:

```cpp
// Get configuration instance
auto& config = NetworkConfig::getInstance();

// Access thresholds
auto thresholds = config.getThresholds();
if (latency > thresholds.latency_warning) {
    // Handle warning condition
}

// Get zone requirements
auto zone = config.getZoneRequirements("premium");
if (bandwidth >= zone.min_bandwidth) {
    // Handle premium zone logic
}
```

## Monitoring Integration

Configuration values are exposed to the monitoring system:

```sql
-- Query current thresholds
SELECT 
    config_key, 
    config_value 
FROM p2p_system_config 
WHERE config_group = 'network_quality';

-- Monitor threshold violations
SELECT 
    host_id,
    COUNT(*) as violations
FROM p2p_host_metrics
WHERE network_latency > :latency_critical
GROUP BY host_id;
```

## Automated Testing

The validation tool is integrated into the test suite:

```bash
# Run all network configuration tests
make test-network-config

# Run specific test cases
./tests/run_tests.sh --group network-config
```

## Configuration Updates

1. Make changes to network_quality.conf
2. Run validation: `./tools/check_network_config.sh -v`
3. If valid, apply changes: `make reload-config`
4. Monitor system for any issues
5. Rollback if needed: `make rollback-config`

Remember to always validate configuration changes before applying them to a production environment.