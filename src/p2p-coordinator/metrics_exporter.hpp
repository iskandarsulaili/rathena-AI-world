// metrics_exporter.hpp
// Prometheus/Loki metrics exporter for P2P Coordinator

#pragma once

#include <string>
#include <vector>
#include <mutex>
#include <nlohmann/json.hpp>

enum class MetricType {
    COUNTER,
    GAUGE,
    HISTOGRAM
};

struct MetricEvent {
    std::string name;
    MetricType type;
    std::vector<std::string> labels;
    double value;
    nlohmann::json attributes;
};

class MetricsExporter {
public:
    MetricsExporter();
    ~MetricsExporter();

    // Export a metric event (thread-safe)
    void export_metric(const MetricEvent& event);

    // Export a structured log to Loki
    void export_log(const nlohmann::json& log);

    // Flush metrics/logs (if batching)
    void flush();

private:
    std::mutex metrics_mutex_;
    std::vector<MetricEvent> metrics_buffer_;
    std::vector<nlohmann::json> logs_buffer_;
};