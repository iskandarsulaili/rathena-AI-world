// metrics_exporter.cpp
// Implementation of MetricsExporter for Prometheus/Loki

#include "metrics_exporter.hpp"
#include <iostream>

MetricsExporter::MetricsExporter() {}

MetricsExporter::~MetricsExporter() {
    flush();
}

void MetricsExporter::export_metric(const MetricEvent& event) {
    std::lock_guard<std::mutex> lock(metrics_mutex_);
    metrics_buffer_.push_back(event);
    // Structured log for Prometheus
    nlohmann::json metric_log = {
        {"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())},
        {"metric", event.name},
        {"type", static_cast<int>(event.type)},
        {"labels", event.labels},
        {"value", event.value},
        {"attributes", event.attributes}
    };
    std::cout << metric_log.dump() << std::endl;
}

void MetricsExporter::export_log(const nlohmann::json& log) {
    std::lock_guard<std::mutex> lock(metrics_mutex_);
    logs_buffer_.push_back(log);
    // Structured log for Loki
    std::cout << log.dump() << std::endl;
}

void MetricsExporter::flush() {
    std::lock_guard<std::mutex> lock(metrics_mutex_);
    metrics_buffer_.clear();
    logs_buffer_.clear();
}