# Network Quality Testing Guide

## Testing Tools and Methods

### 1. Basic Quality Tests
```bash
# Latency Test (Using ICMP)
ping -c 20 [host_address]       # Basic latency test
mtr -n [host_address]           # Detailed route analysis

# Bandwidth Test (Using iperf3)
iperf3 -c [host_address] -t 30  # TCP bandwidth test
iperf3 -u -c [host_address]     # UDP jitter test
```

### 2. Implementation in Host Client

```cpp
class NetworkQualityTest {
public:
    struct TestResults {
        float latency_ms;
        float jitter_ms;
        float upload_mbps;
        float download_mbps;
        float packet_loss;
    };

    TestResults run_full_test() {
        TestResults results;
        
        // Run tests in parallel
        std::future<float> latency = std::async(std::launch::async,
            &NetworkQualityTest::measure_latency, this);
        std::future<float> bandwidth = std::async(std::launch::async,
            &NetworkQualityTest::measure_bandwidth, this);
            
        results.latency_ms = latency.get();
        results.upload_mbps = bandwidth.get();
        
        return results;
    }

private:
    float measure_latency(size_t samples = 10) {
        std::vector<float> measurements;
        for (size_t i = 0; i < samples; ++i) {
            auto start = std::chrono::high_resolution_clock::now();
            // Send ping packet
            // Wait for response
            auto end = std::chrono::high_resolution_clock::now();
            float latency = std::chrono::duration<float, std::milli>(
                end - start).count();
            measurements.push_back(latency);
        }
        return calculate_average(measurements);
    }
    
    float measure_bandwidth() {
        // Implementation details in host_network_test.cpp
        return test_upload_speed();
    }
};
```

## Quality Validation Script

```python
#!/usr/bin/env python3
import subprocess
import json
import statistics

def validate_host_network(host_address, min_requirements):
    results = {
        'latency': test_latency(host_address),
        'bandwidth': test_bandwidth(host_address),
        'stability': test_stability(host_address),
        'passed': False
    }
    
    results['passed'] = (
        results['latency'] <= min_requirements['max_latency'] and
        results['bandwidth'] >= min_requirements['min_bandwidth'] and
        results['stability'] >= min_requirements['min_stability']
    )
    
    return results

def test_latency(host, samples=20):
    cmd = ['ping', '-c', str(samples), host]
    output = subprocess.check_output(cmd).decode()
    
    # Parse ping statistics
    times = []
    for line in output.splitlines():
        if 'time=' in line:
            time = float(line.split('time=')[1].split()[0])
            times.append(time)
            
    return {
        'avg': statistics.mean(times),
        'stddev': statistics.stdev(times),
        'min': min(times),
        'max': max(times)
    }

# Usage example
if __name__ == '__main__':
    min_requirements = {
        'max_latency': 100,
        'min_bandwidth': 10,
        'min_stability': 0.95
    }
    
    results = validate_host_network('host.example.com', min_requirements)
    print(json.dumps(results, indent=2))
```

## Integration Tests

```cpp
// test/network/quality_test.cpp
#include <gtest/gtest.h>
#include "network/quality.hpp"

TEST(NetworkQuality, LatencyMeasurement) {
    NetworkQualityTest tester;
    auto results = tester.measure_latency();
    
    EXPECT_GT(results, 0);
    EXPECT_LT(results, 1000);  // Expect < 1s latency
}

TEST(NetworkQuality, BandwidthMeasurement) {
    NetworkQualityTest tester;
    auto results = tester.measure_bandwidth();
    
    EXPECT_GT(results.upload_mbps, 1.0);
    EXPECT_GT(results.download_mbps, 1.0);
}

TEST(NetworkQuality, StabilityTest) {
    NetworkQualityTest tester;
    auto results = tester.test_stability(std::chrono::minutes(1));
    
    EXPECT_GT(results.stability_score, 0.9);
    EXPECT_LT(results.packet_loss, 0.02);
}
```

## Continuous Monitoring

### 1. Setup Prometheus Metrics

```cpp
// In host client
void register_network_metrics() {
    prometheus::Registry registry;
    
    auto& latency_gauge = prometheus::BuildGauge()
        .Name("network_latency_ms")
        .Help("Current network latency in milliseconds")
        .Register(registry);
        
    auto& bandwidth_gauge = prometheus::BuildGauge()
        .Name("network_bandwidth_mbps")
        .Help("Current bandwidth usage in Mbps")
        .Register(registry);
        
    // Update metrics periodically
    std::thread([&]() {
        while (running) {
            auto results = run_network_tests();
            latency_gauge.Set(results.latency_ms);
            bandwidth_gauge.Set(results.bandwidth_mbps);
            std::this_thread::sleep_for(std::chrono::seconds(30));
        }
    }).detach();
}
```

### 2. Grafana Dashboard Configuration

```json
{
  "dashboard": {
    "id": null,
    "title": "P2P Host Network Quality",
    "panels": [
      {
        "title": "Network Latency",
        "type": "graph",
        "datasource": "Prometheus",
        "targets": [
          {
            "expr": "network_latency_ms",
            "legendFormat": "{{host}}"
          }
        ],
        "thresholds": [
          {
            "value": 100,
            "colorMode": "warning"
          },
          {
            "value": 200,
            "colorMode": "critical"
          }
        ]
      },
      {
        "title": "Bandwidth Usage",
        "type": "graph",
        "targets": [
          {
            "expr": "network_bandwidth_mbps",
            "legendFormat": "{{host}}"
          }
        ]
      }
    ]
  }
}
```

## Alert Rules

```yaml
# prometheus/rules/network_quality.yml
groups:
  - name: network_quality
    rules:
      - alert: HighLatency
        expr: network_latency_ms > 200
        for: 5m
        labels:
          severity: warning
        annotations:
          description: "Host {{ $labels.host }} experiencing high latency"
          
      - alert: LowBandwidth
        expr: network_bandwidth_mbps < 5
        for: 5m
        labels:
          severity: warning
        annotations:
          description: "Host {{ $labels.host }} has insufficient bandwidth"
          
      - alert: NetworkInstability
        expr: rate(network_errors_total[5m]) > 0.1
        for: 5m
        labels:
          severity: critical
        annotations:
          description: "Host {{ $labels.host }} showing network instability"
```

## Performance Benchmarks

Minimum acceptable values for production deployment:

| Metric | Good | Acceptable | Poor |
|--------|------|------------|------|
| Latency | <50ms | 50-100ms | >100ms |
| Jitter | <10ms | 10-20ms | >20ms |
| Upload | >20Mbps | 10-20Mbps | <10Mbps |
| Download | >50Mbps | 20-50Mbps | <20Mbps |
| Packet Loss | <0.1% | 0.1-1% | >1% |
| Stability Score | >0.99 | 0.95-0.99 | <0.95 |