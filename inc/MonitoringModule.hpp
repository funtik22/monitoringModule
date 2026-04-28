//MonitoringMudule.hpp

#pragma once

#include "MetricHistory.hpp"

#include "BaseMemory.hpp"
#include "BaseModule.hpp"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>

#define MESSAGE_BUS_NAME "/monitoring_module"
#define SUBSCRIBE_TO_TAG "measurement_configure"

class MonitoringModule final : public BaseModule {
public:
    explicit MonitoringModule();
    ~MonitoringModule();

    void initialize() override;
    void start()      override;  
    void stop()       override;   

    void collectMetrics();                     
    void storeMetrics();                      
    void publishMetricsReport();               
    void checkThresholds();                   
    void generateThresholdAlarm(const MetricValue& m,
                                const Sample&      s,
                                AlarmSeverity      sev);
    void registerStandardMetrics();            

    void publishReportFor(const std::string& metricId);

private:
    std::string buildReport(const MetricHistory& h,
                            std::chrono::system_clock::time_point from,
                            std::chrono::system_clock::time_point to) const;

    void onConfigureMetric(const oran::platform::Message& msg);
    void onMetricsQuery   (const oran::platform::Message& msg);

    void publishLog(const std::string& tag, const std::string& text);
    void publishLogInfo   (const std::string& t) { publishLog("log_info",    t); }
    void publishLogWarning(const std::string& t) { publishLog("log_warning", t); }
    void publishLogError  (const std::string& t) { publishLog("log_error",   t); }

    void collectorLoop();
    void reporterLoop();
    void listenerLoop();

    static std::chrono::steady_clock::time_point
    alignFutureTick(std::chrono::steady_clock::time_point now,
                    std::chrono::milliseconds             interval);

    std::chrono::steady_clock::time_point
    nextWake(bool collect,
             std::chrono::steady_clock::time_point now,
             std::chrono::milliseconds             fallback) const;

    std::unordered_map<std::string, MetricHistory>                   collectors;
    std::unordered_map<std::string, std::unique_ptr<IMetricCollector>> sources;
    std::unordered_map<std::string, std::string>                     metricToSource;

    std::unique_ptr<BaseMemory>          messageBus;

    std::thread collectorThread_, reporterThread_, listenerThread_;

    std::atomic<bool>       running_{false};
    mutable std::mutex      collectorsMutex_;
    std::mutex              shutdownMu_;
    std::condition_variable shutdownCv_;
};
