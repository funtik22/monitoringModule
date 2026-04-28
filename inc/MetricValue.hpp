//MetricValue.hpp

#pragma once

#include <chrono>
#include <cstdint>
#include <string>

enum class MetricType     { GAUGE, COUNTER, HISTOGRAM };
enum class MetricCategory { SYSTEM, NETWORK, RADIO };
enum class MetricQuality  { GOOD, BAD, UNCERTAIN };

enum class AlarmSeverity  { NORMAL = 0, WARNING = 1, CRITICAL = 2 };

struct MetricValue {
    std::string       metricId;       
    std::string       metricName;    
    MetricType        type;
    MetricCategory    category;
    std::string       description;

    double            warningThreshold{0.0};
    double            criticalThreshold{0.0};
    double            minAllowedValue{0.0};
    double            maxAllowedValue{0.0};
    std::string       stableValue;    

    std::chrono::milliseconds collectMetricsInterval{std::chrono::seconds(1)};
    std::chrono::milliseconds reportInterval{std::chrono::seconds(5)};
    std::uint16_t             windowSize{60};

    std::chrono::steady_clock::time_point nextCollectAt{};
    std::chrono::steady_clock::time_point nextReportAt{};
};


inline const char* toString(MetricType t) {
    switch (t) {
        case MetricType::GAUGE:     return "GAUGE";
        case MetricType::COUNTER:   return "COUNTER";
        case MetricType::HISTOGRAM: return "HISTOGRAM";
    }
    return "UNKNOWN";
}

inline const char* toString(MetricCategory c) {
    switch (c) {
        case MetricCategory::SYSTEM:  return "SYSTEM";
        case MetricCategory::NETWORK: return "NETWORK";
        case MetricCategory::RADIO:   return "RADIO";
    }
    return "UNKNOWN";
}

inline const char* toString(MetricQuality q) {
    switch (q) {
        case MetricQuality::GOOD:      return "GOOD";
        case MetricQuality::BAD:       return "BAD";
        case MetricQuality::UNCERTAIN: return "UNCERTAIN";
    }
    return "UNKNOWN";
}

inline const char* toString(AlarmSeverity s) {
    switch (s) {
        case AlarmSeverity::NORMAL:   return "NORMAL";
        case AlarmSeverity::WARNING:  return "WARNING";
        case AlarmSeverity::CRITICAL: return "CRITICAL";
    }
    return "UNKNOWN";
}
