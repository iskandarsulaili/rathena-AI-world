# Network Quality Monitoring Setup

## Prometheus Configuration

### 1. Network Metrics Configuration
```yaml
# prometheus/p2p_network.yml
scrape_configs:
  - job_name: 'p2p_network'
    scrape_interval: 15s
    static_configs:
      - targets: ['localhost:9100']
    metrics_path: '/metrics/network'
    
    relabel_configs:
      - source_labels: [__address__]
        target_label: instance
      - source_labels: [__param_target]
        target_label: host

custom_metrics:
  network_quality:
    - name: network_latency_milliseconds
      type: gauge
      help: "Network latency in milliseconds"
    
    - name: network_bandwidth_mbps
      type: gauge
      help: "Network bandwidth in Mbps"
      
    - name: network_jitter_milliseconds
      type: gauge
      help: "Network jitter in milliseconds"
      
    - name: packet_loss_ratio
      type: gauge
      help: "Packet loss ratio (0.0-1.0)"
```

### 2. Alert Rules
```yaml
# prometheus/rules/network_alerts.yml
groups:
  - name: network_quality
    rules:
      - alert: CriticalLatency
        expr: avg_over_time(network_latency_milliseconds[5m]) > 200
        for: 2m
        labels:
          severity: critical
        annotations:
          summary: "Critical Latency on {{ $labels.instance }}"
          description: "Network latency is {{ $value }}ms (threshold: 200ms)"

      - alert: BandwidthDegradation
        expr: network_bandwidth_mbps < 5
        for: 5m
        labels:
          severity: warning
        annotations:
          summary: "Low Bandwidth on {{ $labels.instance }}"
          description: "Bandwidth dropped to {{ $value }}Mbps"

      - alert: HighPacketLoss
        expr: rate(packet_loss_ratio[5m]) > 0.02
        for: 2m
        labels:
          severity: warning
        annotations:
          summary: "High Packet Loss on {{ $labels.instance }}"
          description: "Packet loss rate is {{ $value }}%"
```

## Grafana Dashboards

### 1. Network Overview Dashboard
```json
{
  "dashboard": {
    "title": "P2P Network Quality Overview",
    "panels": [
      {
        "title": "Network Latency Distribution",
        "type": "heatmap",
        "datasource": "Prometheus",
        "targets": [
          {
            "expr": "rate(network_latency_bucket[5m])",
            "format": "heatmap"
          }
        ],
        "fieldConfig": {
          "defaults": {
            "thresholds": {
              "mode": "absolute",
              "steps": [
                { "value": 50, "color": "green" },
                { "value": 100, "color": "yellow" },
                { "value": 200, "color": "red" }
              ]
            }
          }
        }
      },
      {
        "title": "Bandwidth Usage",
        "type": "timeseries",
        "targets": [
          {
            "expr": "network_bandwidth_mbps",
            "legendFormat": "{{instance}}"
          }
        ],
        "fieldConfig": {
          "defaults": {
            "unit": "mbps",
            "min": 0
          }
        }
      }
    ]
  }
}
```

### 2. Network Quality Score Dashboard
```json
{
  "dashboard": {
    "title": "Network Quality Scores",
    "panels": [
      {
        "title": "Overall Quality Score",
        "type": "gauge",
        "datasource": "Prometheus",
        "targets": [
          {
            "expr": "(100 - avg(network_latency_milliseconds) / 2) * 0.4 + (min(network_bandwidth_mbps, 100)) * 0.4 + (100 - avg(packet_loss_ratio) * 100) * 0.2"
          }
        ],
        "fieldConfig": {
          "defaults": {
            "max": 100,
            "min": 0,
            "thresholds": {
              "mode": "absolute",
              "steps": [
                { "value": 0, "color": "red" },
                { "value": 60, "color": "yellow" },
                { "value": 80, "color": "green" }
              ]
            }
          }
        }
      }
    ]
  }
}
```

## Real-time Monitoring Script

```python
#!/usr/bin/env python3
import time
import psutil
import requests
from prometheus_client import start_http_server, Gauge

# Define metrics
LATENCY = Gauge('network_latency_milliseconds', 'Network latency in ms')
BANDWIDTH = Gauge('network_bandwidth_mbps', 'Network bandwidth in Mbps')
PACKET_LOSS = Gauge('packet_loss_ratio', 'Packet loss ratio')
JITTER = Gauge('network_jitter_milliseconds', 'Network jitter in ms')

class NetworkMonitor:
    def __init__(self, interval=5):
        self.interval = interval
        self.previous_stats = None
        
    def measure_latency(self, target):
        try:
            start = time.time()
            requests.get(f"http://{target}/health")
            return (time.time() - start) * 1000
        except:
            return float('inf')
    
    def measure_bandwidth(self):
        if self.previous_stats is None:
            self.previous_stats = psutil.net_io_counters()
            time.sleep(1)
        
        current = psutil.net_io_counters()
        duration = self.interval
        
        bytes_sent = current.bytes_sent - self.previous_stats.bytes_sent
        bytes_recv = current.bytes_recv - self.previous_stats.bytes_recv
        
        self.previous_stats = current
        
        return {
            'upload': (bytes_sent * 8) / (duration * 1000000),
            'download': (bytes_recv * 8) / (duration * 1000000)
        }
    
    def run(self, target):
        while True:
            # Measure metrics
            latency = self.measure_latency(target)
            bandwidth = self.measure_bandwidth()
            
            # Update Prometheus metrics
            LATENCY.set(latency)
            BANDWIDTH.set(bandwidth['upload'])  # Focus on upload for P2P
            
            time.sleep(self.interval)

if __name__ == '__main__':
    # Start Prometheus metrics server
    start_http_server(9100)
    
    # Start monitoring
    monitor = NetworkMonitor()
    monitor.run('coordinator.example.com')
```

## Quality-Based Load Balancing

```python
def calculate_host_score(metrics):
    """Calculate a host's quality score based on network metrics."""
    latency_score = max(0, 100 - metrics['latency'] / 2)  # 50ms = 75 points
    bandwidth_score = min(100, metrics['bandwidth'] * 10)  # 10Mbps = 100 points
    stability_score = 100 - (metrics['packet_loss'] * 1000)  # 0.1% loss = 90 points
    
    # Weighted average
    return (
        latency_score * 0.4 +
        bandwidth_score * 0.4 +
        stability_score * 0.2
    )

def select_best_host(available_hosts, network_metrics):
    """Select the best host based on network quality."""
    scored_hosts = [
        (host, calculate_host_score(network_metrics[host]))
        for host in available_hosts
    ]
    
    # Sort by score, highest first
    scored_hosts.sort(key=lambda x: x[1], reverse=True)
    
    # Return the best host and its score
    return scored_hosts[0] if scored_hosts else (None, 0)
```

## Network Quality Reports

### 1. Daily Summary Report
```sql
SELECT 
    DATE(recorded_at) as date,
    host_id,
    AVG(network_latency) as avg_latency,
    MIN(network_latency) as min_latency,
    MAX(network_latency) as max_latency,
    AVG(bandwidth_usage) as avg_bandwidth,
    COUNT(DISTINCT CASE WHEN network_latency > 200 THEN metric_id END) as high_latency_incidents
FROM 
    p2p_host_metrics
WHERE 
    recorded_at >= DATE_SUB(NOW(), INTERVAL 7 DAY)
GROUP BY 
    DATE(recorded_at),
    host_id
ORDER BY 
    date DESC, 
    avg_latency ASC;
```

### 2. Quality Trend Analysis
Monitor network quality trends to identify patterns and potential issues:

```python
def analyze_quality_trends(host_id, days=7):
    """Analyze network quality trends for a host."""
    query = """
    SELECT 
        HOUR(recorded_at) as hour,
        AVG(network_latency) as avg_latency,
        AVG(bandwidth_usage) as avg_bandwidth,
        COUNT(*) as samples
    FROM 
        p2p_host_metrics
    WHERE 
        host_id = %s 
        AND recorded_at >= DATE_SUB(NOW(), INTERVAL %s DAY)
    GROUP BY 
        HOUR(recorded_at)
    ORDER BY 
        hour;
    """
    
    results = execute_query(query, (host_id, days))
    return analyze_patterns(results)
```

## Best Practices

1. **Regular Testing**
   - Run full network quality tests every 5 minutes
   - Perform detailed analysis during low-traffic periods
   - Keep historical data for trend analysis

2. **Alert Thresholds**
   - Critical: Latency > 200ms, Packet Loss > 2%
   - Warning: Latency > 100ms, Packet Loss > 0.5%
   - Info: Bandwidth < 50% of baseline

3. **Performance Optimization**
   - Use UDP for latency-sensitive operations
   - Implement automatic quality-based routing
   - Cache network quality scores

4. **Monitoring Strategy**
   - Real-time metrics for active hosts
   - Historical analysis for capacity planning
   - Correlation with player experience metrics