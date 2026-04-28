//MetricHistory.hpp

#pragma once

#include "MetricValue.hpp"

#include <boost/circular_buffer.hpp>

#include <chrono>
#include <cstdint>
#include <string>
#include <variant>


struct Sample {
    std::chrono::system_clock::time_point     timestamp{};
    std::variant<std::int64_t, double, std::string> value{};
    MetricQuality                             quality{MetricQuality::GOOD};
};


struct MetricHistory {
    MetricValue                   descriptor;
    boost::circular_buffer<Sample> samples;

    //AlarmSeverity lastSeverity{AlarmSeverity::NORMAL};

    //std::chrono::system_clock::time_point reportWindowBegin{};

    // MetricHistory() = default;
    // explicit MetricHistory(MetricValue d)
    //     : descriptor(std::move(d)),
    //       samples(descriptor.windowSize ? descriptor.windowSize : 1) {}
};
